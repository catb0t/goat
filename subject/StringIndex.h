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

#include "Map.h"
#include "Vector.h"
#include "Type.h"
#include "String.h"

namespace goat {

	class StringIndex {
	public:
		Vector<String> vector;
		Map<String, int32> tree;

		int32 createIndex(String string) {
			int32 *p_index = tree.find(string);
			if (p_index)
				return *p_index;

			int32 index = vector.len();
			vector.pushBack(string);
			tree.insert(string, index);
			return index;
		}

		int32 searchIndex(String string) {
			int32 *p_index = tree.find(string);
			if (p_index)
				return *p_index;

			return -1;
		}

		String& getString(int32 index) {
			return vector[index];
		}
	};
}
