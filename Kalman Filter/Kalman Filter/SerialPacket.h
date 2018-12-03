#pragma once
#pragma pack(push,1)
template <typename T>
struct Packet
{
	T data;
	uint8_t bytes[sizeof(T)];

	const uint8_t *begin() { return bytes; }
	const uint8_t *end() { return bytes + sizeof(T); }
};
#pragma pack(pop)