#include "./rect.hpp"

#include <utility>

rect_t::rect_t(glm::vec2 pos, glm::vec2 dim) : 
	x(pos.x), y(pos.y), 
	w(dim.x), h(dim.y) 
{

}

rect_t::rect_t(glm::vec2 dim) : 
	x(0.0f), y(0.0f), 
	w(dim.x), h(dim.y)
{
	
}
rect_t::rect_t(real_t x, real_t y, real_t w, real_t h) : 
	x(x), y(y), 
	w(w), h(h)
{
	
}

rect_t::rect_t() : 
	x(0.0f), y(0.0f), 
	w(0.0f), h(0.0f) 
{
	
}

rect_t::rect_t(const rect_t& that) noexcept : rect_t() {
	x = that.x;
	y = that.y;
	w = that.w;
	h = that.h;
}

rect_t::rect_t(rect_t&& that) noexcept : rect_t() {
	std::swap(x, that.x);
	std::swap(y, that.y);
	std::swap(w, that.w);
	std::swap(h, that.h);
}

rect_t& rect_t::operator=(const rect_t& that) noexcept {
	x = that.x;
	y = that.y;
	w = that.w;
	h = that.h;
	return *this;
}

rect_t& rect_t::operator=(rect_t&& that) noexcept {
	std::swap(x, that.x);
	std::swap(y, that.y);
	std::swap(w, that.w);
	std::swap(h, that.h);
	return *this;
}

void rect_t::push_fix(rect_t* that) const {
	if (that->x < x) {
		that->x = x;
	}
	if (that->y < y) {
		that->y = y;
	}
	if (that->right() > this->right()) {
		that->x = this->right() - that->w;
	}
	if (that->bottom() > this->bottom()) {
		that->y = this->bottom() - that->h;
	}
}

void rect_t::push_fix(glm::vec2* mid, glm::vec2 dim) const {
	glm::vec2 pos = *mid - (dim / 2.0f);
	if (pos.x < x) {
		pos.x = x;
	}
	if (pos.y < y) {
		pos.y = y;
	}
	if ((pos.x + dim.x) > this->right()) {
		pos.x = this->right() - dim.x;
	}
	if ((pos.y + dim.y) > this->bottom()) {
		pos.y = this->bottom() - dim.y;
	}
	*mid = pos + (dim / 2.0f);
}

bool rect_t::contains(glm::vec2 v) const {
	return this->right() >= v.x and
		x <= v.x and
		y <= v.y and
		this->bottom() >= v.y;
}

bool rect_t::contains(const rect_t& that) const {
	return this->right() >= that.right() and
		x <= that.x and
		y <= that.y and
		this->bottom() >= that.bottom();
}

bool rect_t::overlaps(const rect_t& that) const {
	return this->right() > that.x and
		x < that.right() and
		y < that.bottom() and
		this->bottom() > that.y;
}

bool rect_t::overlaps(glm::vec2 that_pos, glm::vec2 that_dim) const {
	const rect_t that = rect_t(that_pos, that_dim);
	return this->overlaps(that);
}

bool rect_t::intersects(glm::vec2 ray_pos, glm::vec2 ray_dir) const {
	real_t tx1 = (x - ray_pos.x) / ray_dir.x;
	real_t tx2 = (this->right() - ray_pos.x) / ray_dir.x;

	real_t tmin = glm::min(tx1, tx2);
	real_t tmax = glm::max(tx1, tx2);

	real_t ty1 = (y - ray_pos.y) / ray_dir.y;
	real_t ty2 = (this->bottom() - ray_pos.y) / ray_dir.y;

	tmin = glm::max(tmin, glm::min(ty1, ty2));
	tmax = glm::min(tmax, glm::max(ty1, ty2));

	return tmax >= tmin;
}

bool rect_t::intersects(const rect_t& that, rect_t* intersection) const {
	real_t r1_min_x = glm::min(x, x + w);
	real_t r1_max_x = glm::max(x, x + w);
	real_t r1_min_y = glm::min(y, y + h);
	real_t r1_max_y = glm::max(y, y + h);

	real_t r2_min_x = glm::min(that.x, that.x + that.w);
	real_t r2_max_x = glm::max(that.x, that.x + that.w);
	real_t r2_min_y = glm::min(that.y, that.y + that.h);
	real_t r2_max_y = glm::max(that.y, that.y + that.h);

	real_t inter_left = glm::max(r1_min_x, r2_min_x);
	real_t inter_top = glm::max(r1_min_y, r2_min_y);
	real_t inter_right = glm::min(r1_max_x, r2_max_x);
	real_t inter_bottom = glm::min(r1_max_y, r2_max_y);

	if ((inter_left < inter_right) && (inter_top < inter_bottom)) {

		*intersection = rect_t(
			inter_left,
			inter_top,
			inter_right - inter_left,
			inter_bottom - inter_top
		);

		return true;
	}

	return false;
}

bool rect_t::cmp_round(const rect_t& that) const {
	const glm::ivec4 lhv = glm::ivec4(x, y, w, h);
	const glm::ivec4 rhv = glm::ivec4(that.x, that.y, that.w, that.h);
	return lhv == rhv;
}

bool rect_t::operator==(const rect_t& that) const {
	return x != that.x or
		y != that.y or
		w != that.w or
		h != that.h;
}

bool rect_t::operator!=(const rect_t& that) const {
	return !(*this == that);
}

bool rect_t::operator==(const glm::vec4& that) const {
	return x != that.x or
		y != that.y or
		w != that.z or
		h != that.w;
}

bool rect_t::operator!=(const glm::vec4& that) const {
	return !(*this == that);
}