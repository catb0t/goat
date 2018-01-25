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

#include "Resource.h"

namespace goat {

	String Resource::arguments = "arguments";
	String Resource::init = "init";
	String Resource::trace = "trace";
	String Resource::s_true = "true";
	String Resource::s_false = "false";
	String Resource::s_undefined = "undefined";
	String Resource::s_null = "null";
	String Resource::s_breakStmt = "break;";
	String Resource::s_continueStmt = "continue;";
	String Resource::s_var = "var";
	String Resource::s_function = "$";
	String Resource::s_return = "return";
	String Resource::s_if = "if";
	String Resource::s_else = "else";
	String Resource::s_for = "for";
	String Resource::s_do = "do";
	String Resource::s_while = "while";
	String Resource::s_new = "new";
	String Resource::s_thread = "thread";
	String Resource::s_break = "break";
	String Resource::s_continue = "continue";
	String Resource::s_in = "in";
	String Resource::s_switch = "switch";
	String Resource::s_case = "case";
	String Resource::s_default = "default";
	String Resource::s_try = "try";
	String Resource::s_catch = "catch";
	String Resource::s_finally = "finally";
	String Resource::s_throw = "throw";
	String Resource::s_import = "import";
	String Resource::s_this = "this";

	String Resource::s_colon = ":";
	String Resource::s_semicolon = ";";
	String Resource::s_comma = ",";
	String Resource::s_dot = ".";

	WideString Resource::w_undefined = L"undefined";
	WideString Resource::w_null = L"null";
	WideString Resource::w_File = L"File";
}