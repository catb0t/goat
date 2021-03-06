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

#include "InputStream.h"
#include "OutputStream.h"
#include "Type.h"
#include "WideString.h"

namespace goat {

	class Console {
	protected:
		InputStream<wchar> *input;
		OutputStream<wchar> *output;
		wchar terminator;
		const wchar *ignore;
		
	public:
		Console(InputStream<wchar> *_input, OutputStream<wchar> *_output, wchar _terminator, const wchar *_ignore) :
			input(_input), output(_output), terminator(_terminator), ignore(_ignore) {
		}

		WideString read();
		void write(wchar c);
		void write(const wchar *s);
		void write(WideString ws);
	};

}