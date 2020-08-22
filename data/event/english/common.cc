// Commons Room

void main() {
	msg::set_field_text("Common Room");
	msg::fade_in();
}

void door_to_naomi() {
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("naomi", kThatDoorID);
}

void door_to_practice() {
	const sint32_t kThisDoorID = 300;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("practice", kThatDoorID);
}

void check_fireplace() {
	sys::lock();
	msg::top_box();
	msg::say("Crackle!\nSnap!");
	sys::wait();
	msg::clear();
	msg::say("It's hot...");
	sys::wait();
}

void transfer_common_stupid_face() {
	msg::set_field_text("Stupid Face");
	msg::fade_in();
}
