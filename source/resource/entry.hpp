#ifndef SYNAO_RESOURCE_ENTRY_HPP
#define SYNAO_RESOURCE_ENTRY_HPP

#include "../utility/hash.hpp"

struct resource_entry_t {
public:
	constexpr resource_entry_t() : 
		filename(nullptr),
		hash(0) {}
	constexpr resource_entry_t(const byte_t* filename) :
		filename(filename),
		hash(SYNAO_HASH(filename)) {}
public:
	const byte_t* filename;
	arch_t hash;
};

#endif // SYNAO_RESOURCE_ENTRY_HPP