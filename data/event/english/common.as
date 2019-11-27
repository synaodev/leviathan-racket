// Commons Room

void main()
{
	msg::setFieldText("Common Room");
	msg::fadeIn();
}

void door_to_naomi()
{
	const sint32 kThisDoorID = 100;
	const sint32 kThatDoorID = 100;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("naomi", kThatDoorID);
}

void door_to_practice()
{
	const sint32 kThisDoorID = 300;
	const sint32 kThatDoorID = 100;

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