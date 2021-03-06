/*

Copyright (C) 2017, 2018 Ivan Kniazkov

This file is part of interpreter of programming language
codenamed "Goat" ("Goat interpreter").

Goat interpreter is free software: you can redistribute it and/or
modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Goat interpreter is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with Goat interpreter.  If not, see <http://www.gnu.org/licenses/>.

*/

#pragma once

#include "Environment.h"
#include "Source.h"
#include "Scope.h"
#include "Root.h"
#include "GarbageCollector.h"

namespace goat {

	class Launcher {
	protected:
		class Options {
		public:
			GarbageCollector *gc;
			bool debug;
			String path;
			Vector<String> libs;

			Options() : gc(nullptr), debug(false) {
			}
		};

	public:
		static int run(Source *src, Environment *env, Scope *scope, Root **proot, Options *opt);
		static int runCmdLine(int argc, char **argv);
		static int interactive();
	};

}