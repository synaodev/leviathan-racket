#ifndef LEVIATHAN_INCLUDED_FLAGS_HPP
#define LEVIATHAN_INCLUDED_FLAGS_HPP

#include <type_traits>

template<typename T>
struct strict_flag_traits_t {
	enum {
		all = 0
	};
};

template<typename T>
struct strict_flags_t {
private:
	using bit_t = T;
	using mask_t = typename std::underlying_type<bit_t>::type;
	mask_t mask;
public:
	constexpr strict_flags_t() noexcept : mask(0) {}
	constexpr strict_flags_t(bit_t bits) noexcept : mask(static_cast<mask_t>(bits)) {}
	constexpr strict_flags_t(strict_flags_t<bit_t> const& that) noexcept : mask(that.mask) {}
	constexpr explicit strict_flags_t(mask_t flags) noexcept : mask(flags) {}
	constexpr bool operator<(strict_flags_t<bit_t> const& that) const noexcept { return this->mask < that.mask; }
	constexpr bool operator<=(strict_flags_t<bit_t> const& that) const noexcept { return this->mask <= that.mask; }
	constexpr bool operator>(strict_flags_t<bit_t> const& that) const noexcept { return this->mask > that.mask; }
	constexpr bool operator>=(strict_flags_t<bit_t> const& that) const noexcept { return this->mask >= that.mask; }
	constexpr bool operator==(strict_flags_t<bit_t> const& that) const noexcept { return this->mask == that.mask; }
	constexpr bool operator!=(strict_flags_t<bit_t> const& that) const noexcept { return this->mask != that.mask; }
	constexpr bool operator!() const noexcept { return !mask; }
	constexpr strict_flag_traits_t<bit_t> operator&(strict_flag_traits_t<bit_t> const& that) const noexcept { return strict_flags_t<bit_t>(this->mask & that.mask); }
	constexpr strict_flag_traits_t<bit_t> operator|(strict_flag_traits_t<bit_t> const& that) const noexcept { return strict_flags_t<bit_t>(this->mask | that.mask); }
	constexpr strict_flag_traits_t<bit_t> operator^(strict_flag_traits_t<bit_t> const& that) const noexcept { return strict_flags_t<bit_t>(this->mask ^ that.mask); }
	constexpr strict_flag_traits_t<bit_t> operator~() const noexcept { return strict_flags_t<bit_t>(mask ^ strict_flag_traits_t<bit_t>::all); }
	constexpr strict_flag_traits_t<bit_t>& operator=(strict_flag_traits_t<bit_t>& const that) noexcept { this->mask = that.mask; return *this; }
	constexpr strict_flag_traits_t<bit_t>& operator|=(strict_flag_traits_t<bit_t>& const that) noexcept { this->mask |= that.mask; return *this; }
	constexpr strict_flag_traits_t<bit_t>& operator&=(strict_flag_traits_t<bit_t>& const that) noexcept { this->mask &= that.mask; return *this; }
	constexpr strict_flag_traits_t<bit_t>& operator^=(strict_flag_traits_t<bit_t>& const that) noexcept { this->mask ^= that.mask; return *this; }
	explicit constexpr operator bool() const noexcept { return !!mask; }
	explicit constexpr operator mask_t() const noexcept { return mask; }
};

template<typename bit_t>
constexpr bool operator<(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags > bit; }
template<typename bit_t>
constexpr bool operator<=(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags >= bit; }
template<typename bit_t>
constexpr bool operator>(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags < bit; }
template<typename bit_t>
constexpr bool operator>=(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags <= bit; }
template<typename bit_t>
constexpr bool operator==(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags == bit; }
template<typename bit_t>
constexpr bool operator!=(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags != bit; }
template<typename bit_t>
constexpr bool operator&(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags & bit; }
template<typename bit_t>
constexpr bool operator|(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags | bit; }
template<typename bit_t>
constexpr bool operator^(bit_t bit, strict_flags_t<bit_t> const& flags) noexcept { return flags ^ bit; }

#endif // LEVIATHAN_INCLUDED_FLAGS_HPP
