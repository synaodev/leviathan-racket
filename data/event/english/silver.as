// Silver

void main() {
	msg::fade_in();
}

void door_to_practice() {
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 300;

	sys::lock();
	ctx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("practice", kThatDoorID);
}