#pragma once

#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#include <cstdint>
#include <inttypes.h>
#include <random>
#include <stdio.h>
#include <string.h>
#include <stdexcept>

namespace uuidxx
{
	enum class Variant
	{
		Nil,
		Version1,
		Version2,
		Version3,
		Version4,
		Version5
	};

	class NotImplemented : public std::logic_error
	{
	public:
		NotImplemented() : std::logic_error("Function not yet implemented") { };
	};

	union uuid
	{
	private:
		static uuid Generatev4() {
			//mach-o does not support TLS and clang still has issues with thread_local
#if !defined(__APPLE__) && !defined(__clang__)
			thread_local std::random_device rd;
			thread_local auto gen = std::mt19937_64(rd());
#else
			std::random_device rd;
			std::mt19937_64 gen(rd());
#endif
			std::uniform_int_distribution<uint64_t> dis64;

			uuid newGuid;
			newGuid.WideIntegers[0] = dis64(gen);
			newGuid.WideIntegers[1] = dis64(gen);

			//RFC4122 defines (psuedo)random uuids (in big-endian notation):
			//MSB of DATA4[0] specifies the variant and should be 0b10 to indicate standard uuid,
			//and MSB of DATA3 should be 0b0100 to indicate version 4
			newGuid.Bytes.Data4[0] = (newGuid.Bytes.Data4[0] & 0x3F) | static_cast<uint8_t>(0x80);
			newGuid.Bytes.Data3[1] = (newGuid.Bytes.Data3[1] & 0x0F) | static_cast<uint8_t>(0x40);

			return newGuid;
		}

	public:
		uint64_t WideIntegers[2];
		struct _internalData
		{
			uint32_t Data1;
			uint16_t  Data2;
			uint16_t Data3;
			uint8_t Data4[8];
		} Uuid;
		struct _byteRepresentation
		{
			uint8_t Data1[4];
			uint8_t Data2[2];
			uint8_t Data3[2];
			uint8_t Data4[8];
		} Bytes;

		bool operator == (const uuid& guid2) const {
			return std::memcmp(&guid2, this, sizeof(uuid)) == 0;
		}
		bool operator != (const uuid& guid2) const {
			return !(*this == guid2);
		}
		bool operator < (const uuid& guid2) const {
			return std::memcmp(this, &guid2, sizeof(uuid)) < 0;
		}
		bool operator > (const uuid& guid2) const {
			return std::memcmp(this, &guid2, sizeof(uuid)) > 0;
		}

		uuid() = default;

		uuid(const char* uuidString)
		{
			if (uuidString == nullptr)
			{
				//special case, and prevents random bugs
				std::memset(this, 0, sizeof(uuid));
				return;
			}

			if (uuidString[0] == '{')
			{
				std::sscanf(uuidString, "{%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "}", &Uuid.Data1, &Uuid.Data2, &Uuid.Data3, &Uuid.Data4[0], &Uuid.Data4[1], &Uuid.Data4[2], &Uuid.Data4[3], &Uuid.Data4[4], &Uuid.Data4[5], &Uuid.Data4[6], &Uuid.Data4[7]);
			}
			else
			{
				std::sscanf(uuidString, "%08" SCNx32 "-%04" SCNx16 "-%04" SCNx16 "-%02" SCNx8 "%02" SCNx8 "-%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "%02" SCNx8 "", &Uuid.Data1, &Uuid.Data2, &Uuid.Data3, &Uuid.Data4[0], &Uuid.Data4[1], &Uuid.Data4[2], &Uuid.Data4[3], &Uuid.Data4[4], &Uuid.Data4[5], &Uuid.Data4[6], &Uuid.Data4[7]);
			}
		}
		uuid(const std::string& uuidString)
			: uuid(uuidString.c_str())
		{
		}

		static uuid FromString(const char* uuidString) {
			uuid temp(uuidString);
			return temp;
		}

		static uuid FromString(const std::string& uuidString) {
			uuid temp(uuidString.c_str());
			return temp;
		}

		static inline uuid Generate(Variant v = Variant::Version4)
		{
			switch (v)
			{
			case Variant::Nil:
				return uuid(nullptr); //special case;
			case Variant::Version1:
			case Variant::Version2:
			case Variant::Version3:
			case Variant::Version5:
				throw new NotImplemented();
			case Variant::Version4:
				return Generatev4();
			default:
				throw new NotImplemented();
			}
		}

		std::string ToString(bool withBraces = true) const {
			char buffer[39];
			std::sprintf(buffer, "%s%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X%s", withBraces ? "{" : "", Uuid.Data1, Uuid.Data2, Uuid.Data3, Uuid.Data4[0], Uuid.Data4[1], Uuid.Data4[2], Uuid.Data4[3], Uuid.Data4[4], Uuid.Data4[5], Uuid.Data4[6], Uuid.Data4[7], withBraces ? "}" : "");
			return buffer;
		}
	};

	static_assert(sizeof(uuid) == 2 * sizeof(int64_t), "Check uuid type declaration/padding!");
}
