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

#include "WideString.h"
#include "String.h"
#include "Utils.h"

namespace goat {

	WideString::WideString() {
		buff = nullptr;
	}

	WideString::WideString(const wchar *cwstr) {
		uint32 i, len = Utils::wstrLen(cwstr);
		if (len > 0) {
			buff = new Buffer(len);
			for (i = 0; i < len; i++) {
				if (cwstr[i] > 127)
					buff->ascii = false;
				buff->data[i] = cwstr[i];
			}
		}
		else {
			buff = nullptr;
		}
	}

	WideString::WideString(uint32 len) {
		buff = len > 0 ? new Buffer(len) : nullptr;
	}

	WideString::WideString(const wchar *cwstr, uint32 len) {
		uint32 i;
		if (len > 0) {
			buff = new Buffer(len);
			for (i = 0; i < len; i++) {
				if (cwstr[i] > 127)
					buff->ascii = false;
				buff->data[i] = cwstr[i];
			}
		}
		else {
			buff = nullptr;
		}
	}

	WideString::WideString(const WideString &wstr) {
		buff = wstr.buff;
		if (buff) {
			buff->refs++;
		}
	}

	WideString::WideString(WideString &&wstr) {
		buff = wstr.buff;
		wstr.buff = nullptr;
	}

	WideString::~WideString() {
		if (buff) {
			buff->release();
		}
	}

	WideString & WideString::operator=(const wchar *cwstr) {
		if (buff) {
			buff->release();
		}
		uint32 i, len = Utils::wstrLen(cwstr);
		if (len > 0) {
			buff = new Buffer(len);
			for (i = 0; i < len; i++) {
				if (cwstr[i] > 127)
					buff->ascii = false;
				buff->data[i] = cwstr[i];
			}
		}
		else {
			buff = nullptr;
		}
		return *this;
	}

	WideString & WideString::operator=(const WideString &wstr) {
		if (buff != wstr.buff) {
			if (buff) {
				buff->release();
			}
			buff = wstr.buff;
			if (buff) {
				buff->refs++;
			}
		}
		return *this;
	}

	wchar WideString::operator[](uint32 idx) {
		if (!buff || idx >= buff->len) {
			throw OutOfBounds();
		}

		return buff->data[idx];
	}

	uint32 WideString::len() const {
		return buff ? buff->len : 0;
	}

	const wchar * WideString::cwstr() const {
		return buff ? buff->data : nullptr;
	}

	bool WideString::operator==(const wchar *cwstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, cwstr) == 0;
	}

	bool WideString::operator!=(const wchar *cwstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, cwstr) != 0;
	}

	bool WideString::operator==(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) == 0;
	}

	bool WideString::operator!=(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) != 0;
	}

	bool WideString::operator<(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) < 0;
	}

	bool WideString::operator<=(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) <= 0;
	}

	bool WideString::operator>(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) > 0;
	}

	bool WideString::operator>=(const WideString &wstr) {
		return Utils::wstrCmp(buff ? buff->data : nullptr, wstr.buff ? wstr.buff->data : nullptr) >= 0;
	}

	bool WideString::operator==(const String &str) {
		return Utils::strWstrCmp(str.buff ? str.buff->data : nullptr, buff ? buff->data : nullptr) == 0;
	}

	bool WideString::operator!=(const String &str) {
		return Utils::strWstrCmp(str.buff ? str.buff->data : nullptr, buff ? buff->data : nullptr) != 0;
	}

	bool WideString::operator<(const String &str) {
		return Utils::strWstrCmp(str.buff ? str.buff->data : nullptr, buff ? buff->data : nullptr) >= 0;
	}

	WideString WideString::operator+(const WideString &wstr) {
		if (!buff) {
			return wstr;
		}

		if (!wstr.buff) {
			return *this;
		}

		WideString result;
		result.buff = new Buffer(buff->len + wstr.buff->len);
		Utils::memCopy(result.buff->data, buff->data, buff->len);
		Utils::memCopy(result.buff->data + buff->len, wstr.buff->data, wstr.buff->len);
		result.buff->ascii = buff->ascii && wstr.buff->ascii;
		return result;
	}

	WideString::Buffer::Buffer(uint32 len) {
		refs = 1;
		data = new wchar[len + 1];
		data[len] = '\0';
		this->len = len;
		ascii = true;
	}

	WideString::Buffer::~Buffer() {
		delete[] data;
	}

	void WideString::Buffer::release() {
		if (!(--refs)) {
			delete this;
		}
	}

	WideString WideString::valueOf(lint val) {
		bool neg = false;
		if (val < 0) {
			neg = true;
			val = -val;
		}
		wchar buff[23];
		wchar *ptr = &buff[22];
		*ptr = '\0';
		do {
			*--ptr = (char)(val % 10) + '0';
			val /= 10;
		} while (val);
		if (neg) {
			*--ptr = '-';
		}
		return WideString(ptr);
	}

	WideString WideString::valueOf(ldouble val, uint8 precision, bool trim) {
		if (precision < 1) {
			precision = 1;
		}
		bool neg = false;
		if (val < 0) {
			neg = true;
			val = -val;
		}
		wchar buff[32];
		wchar *fptr = &buff[31];
		*fptr = '\0';
		wchar *iptr = fptr - precision - 1;
		*iptr = '.';
		lint ival = (lint)val;
		auto tail = val - (ldouble)ival;
		lint fval = (lint)(tail * Utils::pow10(precision + 1));
		auto lastDigit = fval % 10;
		fval /= 10;
		if (lastDigit >= 5) {
			fval++;
			if (fval >= Utils::pow10(precision)) {
				fval = 0;
				ival++;
			}
		}
		if (fval > 0) {
			bool zero = true;
			for (uint32 k = 0; k < precision; k++) {
				auto digit = fval % 10;
				if (digit == 0) {
					if (zero && trim) {
						*--fptr = '\0';
					}
					else {
						*--fptr = '0';
					}
				}
				else {
					zero = false;
					*--fptr = (char)(digit)+'0';
				}
				fval /= 10;
			}
		}
		else {
			*(iptr + 1) = '0';
			*(iptr + 2) = '\0';
		}
		do {
			*--iptr = (char)(ival % 10) + '0';
			ival /= 10;
		} while (ival);
		if (neg) {
			*--iptr = '-';
		}
		return WideString(iptr);
	}

	RawString WideString::toRawString() {
		RawString rs;
		if (buff) {
			rs.data = new wchar[buff->len + 1];
			rs.len = buff->len;
			rs.dynamic = true;
			Utils::memCopy(rs.data, buff->data, buff->len);
			rs.data[rs.len] = '\0';
		}
		return rs;
	}

	bool WideString::isAscii() {
		return buff ? buff->ascii : true;
	}

	String WideString::toString() {
		if (buff) {
			String s(buff->len);
			for (uint32 i = 0; i < buff->len; i++) {
				wchar w = buff->data[i];
				char c = w > 127 ? '?' : (char)w;
				s.buff->data[i] = c;
			}
			return s;
		}
		return nullptr;
	}

	WideString WideString::subString(uint32 start, uint32 count) {
		if (buff && buff->len > start) {
			uint32 tail = buff->len - start;
			if (count > tail)
				count = tail;
		}
		else
			count = 0;

		WideString result;
		if (count > 0) {
			uint32 i;
			result.buff = new Buffer(count);
			for (i = 0; i < count; i++) {
				if (buff->data[i + start] > 127)
					result.buff->ascii = false;
				result.buff->data[i] = buff->data[i + start];
			}
		}
		return result;
	}

	RawString WideString::OutOfBounds::toRawString() {
		return L"wide string: index out of bounds";
	}
}
