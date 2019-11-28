// Silver

void main()
{
	msg::fadeIn();
}

void door_to_practice()
{
	const sint32_t kThisDoorID = 100;
	const sint32_t kThatDoorID = 300;

	sys::lock();
	ctx::animate(kThisDoorID, 0, Dir::Left);
	aud::play("door");
	msg::fadeOut();
	sys::setField("practice", kThatDoorID);
}