# -*- mode: ruby; coding: utf-8 -*-
#
# Copyright (C) 2013  Kouhei Sutou <kou@clear-code.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

base_dir = File.dirname(__FILE__)

$LOAD_PATH.unshift(File.join(base_dir, "lib"))
require "segv-handler-gdb/version"

Gem::Specification.new do |spec|
  spec.name = "segv-handler-gdb"
  spec.version = SEGVHandlerGDB::VERSION

  spec.authors = ["Kouhei Sutou"]
  spec.email = ["kou@clear-code.com"]

  spec.summary = "Dump C level backtrace by GDB on SEGV"
  spec.description = "It helps that you debug C extension that causes SEGV."

  spec.files = ["README.md", "LICENSE.md", "Rakefile", "Gemfile", ".yardopts"]
  spec.files += Dir.glob("doc/text/*.*")
  spec.files += ["#{spec.name}.gemspec"]
  Dir.chdir(base_dir) do
    spec.files += Dir.glob("lib/**/*.rb")
    spec.files += Dir.glob("ext/**/*.{c,rb}")
    spec.extensions = ["ext/segv-handler-gdb/extconf.rb"]
  end

  spec.homepage = "https://github.com/kou/segv-handler-gdb"
  spec.licenses = ["LGPLv3"]
  spec.require_paths = ["lib", "ext/segv-handler-gdb"]

  spec.required_ruby_version = ">= 1.9.3"

  spec.add_development_dependency("rake")
  spec.add_development_dependency("bundler")
end

