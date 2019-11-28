// Practice

void main()
{
	msg::fadeIn();
}

void door_to_common()
{
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 300;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("common", kThatDoorID);
}

void door_to_silver()
{
	const sint32_t kThisDoorID = 300;
	const sint32_t kThatDoorID = 100;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("silver", kThatDoorID);
}