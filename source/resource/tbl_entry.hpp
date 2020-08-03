#ifndef SYNAO_RESOURCE_TBL_ENTRY_HPP
#define SYNAO_RESOURCE_TBL_ENTRY_HPP

#include "../utility/hash.hpp"

struct tbl_entry_t {
public:
	constexpr tbl_entry_t() :
		name(nullptr),
		hash(0) {}
	constexpr tbl_entry_t(const byte_t* name) :
		name(name),
		hash(SYNAO_HASH(name)) {}
public:
	const byte_t* name;
	arch_t hash;
};

#endif // SYNAO_RESOURCE_ENTRY_HPP