#pragma once

#include <string>
#include "utf8.h"

namespace mlnserver::stringUtils {

	static void decode_utf8(const std::string& bytes, std::wstring& wstr)
	{
		utf8::utf8to32(bytes.begin(), bytes.end(), std::back_inserter(wstr));
	}

	static void encode_utf8(const std::wstring& wstr, std::string& bytes)
	{
		utf8::utf32to8(wstr.begin(), wstr.end(), std::back_inserter(bytes));
	}

	static std::string& convUtf8(std::string& bytes)
	{
		std::wstring im(bytes.begin(), bytes.end());
		bytes.clear();
		encode_utf8(im, bytes);
		return bytes;
	}

	static std::string makeComment(const std::wstring& wideString)
	{
		std::string out;
		utf8::utf16to8(wideString.begin(), wideString.end(), std::back_inserter(out));
		return out;
	}

}//namespace mlnserver::stringUtils {