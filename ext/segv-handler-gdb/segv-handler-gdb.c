/* -*- coding: utf-8; mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
  Copyright (C) 2013  Kouhei Sutou <kou@clear-code.com>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <ruby.h>

#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define MESSAGE_BUFFER_SIZE 4096

typedef int shg_bool;

#define SHG_TRUE  (!SHG_FALSE)
#define SHG_FALSE (0)

static struct sigaction original_segv_action;

static const char *
temporary_diretory_path (void)
{
    const char *path = NULL;

    if (!path) {
        path = getenv("TMPDIR");
    }
    if (!path) {
        path = getenv("TMP");
    }
    if (!path) {
        path = getenv("TEMP");
    }
    if (!path) {
        path = "/tmp";
    }

    return path;
}

static void
print_system_error (int output_fd, const char *message)
{
    FILE *output;
    output = fdopen(dup(output_fd), "w");
    fprintf(output, "%s: %s\n", message, strerror(errno));
    fclose(output);
}

static shg_bool
create_gdb_command_file (char *gdb_command_path, int output_fd)
{
    int gdb_command_fd;
    const char *gdb_command =
        "thread apply all backtrace full\n"
        "quit\n";
    ssize_t written;

    gdb_command_fd = mkstemp(gdb_command_path);
    if (gdb_command_fd == -1) {
        print_system_error(output_fd,
                           "[segv-handler-gdb] failed to create temporary file");
        return SHG_FALSE;
    }

    written = write(gdb_command_fd, gdb_command, strlen(gdb_command));
    close(gdb_command_fd);

    if (written == -1) {
        print_system_error(output_fd,
                           "[segv-handler-gdb] failed to write GDB command");
        unlink(gdb_command_path);
        return SHG_FALSE;
    }

    return SHG_TRUE;
}

static void
run_gdb (const char *gdb_command_path, int output_fd)
{
    pid_t gdb_pid;

    gdb_pid = fork();
    if (gdb_pid == 0) {
        char target_pid[256];
        sprintf(target_pid, "%u", getppid());
        dup2(output_fd, STDOUT_FILENO);
        dup2(output_fd, STDERR_FILENO);
        execlp("gdb",
               "gdb",
               "--batch",
               "--command", gdb_command_path,
               "--pid", target_pid,
               NULL);
        _exit(EXIT_FAILURE);
    } else if (gdb_pid == -1) {
        print_system_error(output_fd,
                           "[segv-handler-gdb] failed to fork gdb");
    } else {
        int status;
        int options = 0;
        waitpid(gdb_pid, &status, options);
    }
}

static void
show_gdb_backtrace (void)
{
    const char *segv_handler_gdb_path;
    char segv_handler_gdb_path_default[PATH_MAX];
    char gdb_command_path[PATH_MAX];
    int output_fd;
    shg_bool output_fd_need_close = SHG_FALSE;

    sprintf(gdb_command_path,
            "%s/segv-handler-gdb-XXXXXX",
            temporary_diretory_path());
    segv_handler_gdb_path = getenv("SEGV_HANDLER_GDB_PATH");
    if (!segv_handler_gdb_path) {
        sprintf(segv_handler_gdb_path_default,
                "%s/segv-handler-gdb.%u.log",
                temporary_diretory_path(),
                getpid());
        segv_handler_gdb_path = segv_handler_gdb_path_default;
    }
    if (strcmp(segv_handler_gdb_path, "-") == 0) {
        output_fd = STDOUT_FILENO;
    } else if (strcmp(segv_handler_gdb_path, "+") == 0) {
        output_fd = STDERR_FILENO;
    } else {
        output_fd = open(segv_handler_gdb_path,
                         O_WRONLY | O_CREAT | O_APPEND,
                         S_IRUSR | S_IWUSR);
        if (output_fd == -1) {
            char message[MESSAGE_BUFFER_SIZE];
            snprintf(message,
                     MESSAGE_BUFFER_SIZE,
                     "[segv-handler-gdb] failed to open output path. "
                     "stderr is used instead: <%s>",
                     segv_handler_gdb_path);
            perror(message);
            output_fd = STDERR_FILENO;
        } else {
            output_fd_need_close = SHG_TRUE;
        }
    }

    if (create_gdb_command_file(gdb_command_path, output_fd)) {
        run_gdb(gdb_command_path, output_fd);
        unlink(gdb_command_path);
    }

    if (output_fd_need_close) {
        close(output_fd);
    }
}

static void
show_gdb_backtrace_handler (int signum)
{
    show_gdb_backtrace();
    if (sigaction(signum, &original_segv_action, NULL) == 0) {
        kill(getpid(), signum);
    }
}

void
Init_segv_handler_gdb (void)
{
    struct sigaction segv_action;

    segv_action.sa_handler = show_gdb_backtrace_handler;
    sigemptyset(&(segv_action.sa_mask));
    segv_action.sa_flags = 0;
    if (sigaction(SIGSEGV, &segv_action, &original_segv_action) == -1) {
        rb_sys_fail("[segv-handler-gdb] failed to set SEGV handler");
    }
}
