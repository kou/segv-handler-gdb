# README

segv-handler-gdb is a gem for debugging C extension that causes
SEGV. It dumps C level backtrace by GDB on SEGV. If your C extension
is built with `gcc -O0 -g3`, the backtrace includes useful information
for debug.

## Supported platform

segv-handler-gdb works on Linux. It may also work on other POSIX
systems such as FreeBSD and OS X but it is not tested yet.

Here is a list of supported platform:

  * Debian GNU/Linux
  * (If you confirm that segv-handler-gdb works on your platform,
    please report it.)

## Requirments

Here is a list of required programs:

  * gdb

## Install

    % gem install segv-handler-gdb

## Usage

You just require segv-handler-gdb library like the following:

    require "segv-handler-gdb"

You can use segv-handler-gdb without changing your Ruby script:

    % ruby -rsegv-handler-gdb YOUR-RUBY-SCRIPT.rb

If your Ruby script crashes, you find C level backtrace at
`/tmp/segv-handler-gdb.#{Process.pid}.log`.

## Customization

You can customize segv-handler-gdb behavior by some environment
variables.

### Backtrace dump path

Backtrace is dumped to `/tmp/segv-handler-gdb.#{Process.pid}.log` by
default. You can customize the path by `SEGV_HANDLER_GDB_PATH`
environment variable.

There are some special values for the environment variable:

  * `-`: Backtrace is dumped to the standard output.
  * `+`: Backtrace is dumped to the standard error output.

Here is a sample command line to dump backtrace to the standard
output:

    % SEGV_HANDLER_GDB_PATH=- ruby -rsegv-handlder-gdb YOUR-RUBY-SCRIPT.rb

### Temporary directory path

segv-handler-gdb creates a temporary file to temporary
directory. `/tmp` directory is used as the temporary directory by
default. You can customize the temporary directory by the following
environment variables:

  * `TMPDIR`
  * `TMP`
  * `TEMP`

The order of the above list shows priority. If `TMPDIR` is specified,
`TMP` is not used.

If all of these environment variables aren't specified, `/tmp` is used
as the temporary directory.

## Author

  * Kouhei Sutou \<kou@clear-code.com\>

## License

LGPLv3 or later. See doc/text/lgpl-3.txt for details.
