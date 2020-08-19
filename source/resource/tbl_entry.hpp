#ifndef LEVIATHAN_INCLUDED_RESOURCE_TBL_ENTRY_HPP
#define LEVIATHAN_INCLUDED_RESOURCE_TBL_ENTRY_HPP

#include "../utility/hash.hpp"

struct tbl_entry_t {
public:
	constexpr tbl_entry_t() :
		name(nullptr),
		hash(0) {}
	constexpr tbl_entry_t(const byte_t* name) :
		name(name),
		hash(synao_hash(name)) {}
public:
	const byte_t* name;
	arch_t hash;
};

#endif // LEVIATHAN_INCLUDED_RESOURCE_ENTRY_HPP
