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

#include "Nop.h"
#include "Resource.h"

namespace goat {

	Nop::Nop(Semicolon *semicolon) {
		if (semicolon)
			loc = semicolon->loc;
		else
			loc = nullptr;
	}

	Nop * Nop::toNop() {
		return this;
	}

	State * Nop::createState(State *_prev) {
		return new StateImpl(_prev);
	}

	State * Nop::StateImpl::next() {
		State *p = prev;
		delete this;
		return p;
	}

	String Nop::toString() {
		return Resource::s_semicolon;
	}

	Token * Nop::StateImpl::token() {
		return nullptr;
	}
}
