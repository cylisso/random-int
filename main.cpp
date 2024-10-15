#include <windows.h>
#include <stdint.h>
#include <stdio.h>

extern "C" void fnGetSystemTimeAsFileTime(LPFILETIME lpSystemTimeAsFileTime);

DWORD64 fnXtime_get_ticks()
{
	_FILETIME SystemTimeAsFileTime;
	fnGetSystemTimeAsFileTime(&SystemTimeAsFileTime);

	DWORD64 a1 = static_cast<DWORD64>(SystemTimeAsFileTime.dwLowDateTime);
	DWORD64 a2 = static_cast<DWORD64>(SystemTimeAsFileTime.dwHighDateTime);
	return (a1 + (a2 << 32)) - 116444736000000000llu;
}

template <typename T>
T randuint(T min, T max)
{
	if (min > max) return min;

	static uint64_t seed = fnXtime_get_ticks();

	for (int i = 1; i < 64; i++) {
		seed = i + 0x5851F42D4C957F2Dllu * ((seed >> 62) ^ seed);
	}

	uint64_t state = seed;
	for (int i = 0; i < 312; i++) {
		state = (state ^ (state >> 1)) & 0xFFFFFFFF80000000ull;
		state ^= (state >> 1) ^ 0xB5026F5AA96619E9ull;
	}

	uint64_t value = (state >> 29) ^ state;
	uint64_t p1 = (((((value & 0x38EB3FFFF6D3) << 17) ^ value) & 0xFFFFFFFFFFFFBF77) << 37);
	uint64_t rn = static_cast<uint64_t>(p1 ^ ((value & 0x38EB3FFFF6D3) << 17) ^ value) ^ (value >> 43);
	return min + static_cast<T>(rn % (static_cast<uint64_t>(max - (min + 1) + 1)));
}

void main()
{
	for (int i = 0; i < 30; i++)
	{
		auto u8 = randuint<uint8_t>(0, UINT8_MAX);
		auto u16 = randuint<uint16_t>(0, UINT16_MAX);
		auto u32 = randuint<uint32_t>(0, UINT32_MAX);
		auto u64 = randuint<uint64_t>(0, UINT64_MAX);
		printf("%u, %u, %u, %llu\n", u8, u16, u32, u64);
	}
}