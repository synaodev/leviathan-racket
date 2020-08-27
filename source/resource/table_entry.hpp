#ifndef LEVIATHAN_INCLUDED_RESOURCE_TABLE_ENTRY_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_TABLE_ENTRY_HPP

#include "../utility/hash.hpp"

struct table_entry_t {
public:
	constexpr table_entry_t() :
		name(nullptr),
		hash(0) {}
	constexpr table_entry_t(const byte_t* name) :
		name(name),
		hash(synao_hash(name)) {}
public:
	const byte_t* name;
	arch_t hash;
};

#endif // LEVIATHAN_INCLUDED_RESOURCE_TABLE_ENTRY_HPP
