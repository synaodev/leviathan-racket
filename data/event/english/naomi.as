// Naomi's Dorm

void main()
{
	pxt::exit();
	msg::setFieldText("Naomi's Dorm");
	msg::fadeIn();
}

void door_to_hallway()
{
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("common", kThatDoorID);
}

void use_cmptr()
{
	const arch_t kFileGUI = 1;
	const sint32_t kCmptrID = 200;

	sys::freeze();
	ctx::animate(kCmptrID, 1, Dir::Right);
	aud::play("cmptr");
	sys::pushWidget(kFileGUI, 0);
}

void shoshi_bed()
{
	sys::lock();
	msg::topBox();
	msg::setFace(0, 2);
	msg::say("Shoshi's Bed...");
	sys::wait();
}