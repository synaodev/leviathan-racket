// Practice

void main() {
	msg::fade_in();
}

void door_to_common() {
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 300;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("common", kThatDoorID);
}

void door_to_silver() {
	const sint32_t kThisDoorID = 300;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("silver", kThatDoorID);
}