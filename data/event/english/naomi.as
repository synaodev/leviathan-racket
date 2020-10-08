// Naomi's Dorm

void main() {
	pxt::exit();
	msg::set_field_text("Naomi's Dorm");
	msg::fade_in();
}

void door_to_hallway() {
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ktx::animate(kThisDoorID, 0, dir_t::Left);
	aud::play("door");
	msg::fade_out();
	sys::set_field("common", kThatDoorID);
}

void use_computer() {
	const arch_t kFileGUI = 1;
	const sint32_t kCmptrID = 200;

	sys::freeze();
	ktx::animate(kCmptrID, 1, dir_t::Right);
	aud::play("cmptr");
	sys::push_widget(kFileGUI, 0);
}

void shoshi_bed() {
	sys::lock();
	msg::top_box();
	msg::set_face(0, 2);
	msg::say("Shoshi's Bed...");
	sys::wait();
}

void transfer_naomi_introduction() {
	pxt::exit();
	msg::push_card("Leviathan Racket", font_t::One);
	msg::set_card_position(0, 160.0f, 48.0f);
	msg::set_card_centered(0, true, false);
	sys::wait(1.5f);
	msg::clear_cards();
	msg::fade_in();
}
