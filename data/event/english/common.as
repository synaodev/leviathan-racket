// Commons Room

void main()
{
	msg::setFieldText("Common Room");
	msg::fadeIn();
}

void door_to_naomi()
{
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("naomi", kThatDoorID);
}

void door_to_practice()
{
	const sint32_t kThisDoorID = 300;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("practice", kThatDoorID);
}

void check_fireplace()
{
	sys::lock();
	msg::topBox();
	msg::say("Crackle!\nSnap!");
	sys::wait();
	msg::clear();
	msg::say("It's hot...");
	sys::wait();
}