// Silver

void main() {
	msg::fade_in();
}

void door_to_practice() {
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 300;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("practice", kThatDoorID);
}

void retry_fall() {
	const sint32_t kFoxieID = 200;
	sys::lock();
	nao::teleport(14.0f, 30.0f);
}
