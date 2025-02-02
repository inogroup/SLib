/*
 *   Copyright (c) 2008-2022 SLIBIO <https://github.com/SLIBIO>
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in
 *   all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *   THE SOFTWARE.
 */

#include "slib/core/setting.h"
#include "slib/core/ini.h"

#include "slib/core/file.h"

namespace slib
{

	namespace priv
	{
		namespace setting
		{

			template <class VIEW>
			sl_bool ParseUint32Range(const VIEW& str, sl_uint32* _from, sl_uint32* _to)
			{
				sl_uint32 from;
				sl_uint32 to;

				sl_reg index = str.indexOf('-');
				if (index > 0) {
					if (str.substring(0, index).parseUint32(10, &from)) {
						if (str.substring(index + 1).parseUint32(10, &to)) {
							if (to >= from) {
								if (_from) {
									*_from = from;
								}
								if (_to) {
									*_to = to;
								}
								return sl_true;
							}
						}
					}
				} else {
					if (str.substring(0, index).parseUint32(10, &from)) {
						if (_from) {
							*_from = from;
						}
						if (_to) {
							*_to = from;
						}
						return sl_true;
					}
				}
				return sl_false;
			}

		}
	}

	using namespace priv::setting;

	Ini::Ini()
	{
	}

	Ini::~Ini()
	{
	}

	void Ini::initialize()
	{
		m_mapValues.removeAll();
	}

	sl_bool Ini::parseTextFile(const StringParam& filePath)
	{
		if (File::exists(filePath)) {
			return parseText(File::readAllText(filePath));
		} else {
			return sl_false;
		}
	}

	sl_bool Ini::parseText(const StringParam& _text)
	{
		StringData text(_text);
		sl_size len = text.getLength();
		const sl_char8* buf = text.getData();
		sl_reg indexAssign = -1;
		sl_reg indexComment = -1;
		sl_reg indexStart = 0;
		ObjectLocker lock(&m_mapValues);
		for (sl_size i = 0; i <= len; i++) {
			sl_char16 ch = buf[i];
			if (ch == '=') {
				if (indexAssign < 0 && indexComment < 0) {
					indexAssign = i;
				}
			} else if (ch == '#' || ch == ';') {
				if (indexComment < 0) {
					indexComment = i;
				}
			} else if (ch == '\r' || ch == '\n' || ch == 0) {
				if (indexAssign > 0) {
					String key = String(buf + indexStart, indexAssign - indexStart).trim();
					String value;
					if (indexComment < 0) {
						value = String(buf + indexAssign + 1, i - indexAssign - 1).trim();
					} else {
						value = String(buf + indexAssign + 1, indexComment - indexAssign - 1).trim();
					}
					m_mapValues.put_NoLock(key, value);
				}
				indexAssign = -1;
				indexComment = -1;
				indexStart = i + 1;
			}
		}
		return sl_true;
	}

	String Ini::getValue(const String& name)
	{
		return m_mapValues.getValue_NoLock(name, sl_null);
	}


	sl_bool SettingUtil::parseUint32Range(const StringParam& str, sl_uint32* from, sl_uint32* to)
	{
		if (str.isEmpty()) {
			return sl_false;
		}
		if (str.is8BitsStringType()) {
			return ParseUint32Range(StringData(str), from, to);
		} else if (str.is16BitsStringType()) {
			return ParseUint32Range(StringData16(str), from, to);
		} else {
			return ParseUint32Range(StringData32(str), from, to);
		}
		return sl_false;
	}

}
