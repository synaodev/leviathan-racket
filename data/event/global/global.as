// Head Events (Global)

enum Input
{
	Yes	= 0,
	No 	= 1
};

//////////////////////////////////////////////////////////////////////////////////////////

void boot()
{
	pxt::exit();
	msg::setCard("Leviathan Racket", Font::One, true, false, 160.0f, 48.0f);
	sys::wait(1.5);
	msg::setCard();
	msg::fadeIn();
}

void is_empty() // Empty Chest
{
	sys::freeze();
	msg::topBox();
	msg::say(sys::locale("IsEmpty", 0));
	sys::wait();
}

void use_bed() // Bed
{
	sys::freeze();
	msg::topBox();

	std::array<std::string> question = {
		sys::locale("UseBed", 0),
		sys::locale("UseBed", 1),
		sys::locale("UseBed", 2)
	};

	msg::ask(question);

	arch_t answer = msg::getAnswer();

	switch(answer)
	{
		case 0: // If "Save Progress"
		{
			const arch_t kFileGUI = 1;
			
			sys::setFlag(10, true);
			msg::close();
			aud::play("inven");
			sys::pushWidget(kFileGUI, 1);

			break;
		}

		case 1: // If "Rest in Bed"
		{
			msg::close();
			pxt::fade(2.0f);
			msg::fadeOut();
			sys::wait(1.0);
			msg::topBox();
			msg::say(sys::locale("UseBed", 3));
			sys::wait();
			msg::close();
			nao::lifeUp(20);
			sys::wait(1.0);
			msg::fadeIn();
			pxt::redo(1.0f);
			sys::wait(0.5);
			msg::topBox();
			msg::say(sys::locale("UseBed", 4));
			sys::wait();

			break;
		}

		default:
		{
			break;
		}
	}
}

void death(arch_t type) // Naomi Dies
{
	std::string death_print;

	switch(type)
	{
		case 1: // Disintegration
		{
			aud::play("player_death");
			aud::play("crash");
			death_print = sys::locale("Death", 1);
			break;
		}

		case 2: // Normal
		{
			aud::play("player_death");
			death_print = sys::locale("Death", 0);
			break;
		}

		case 3: // Fell Off Map
		{
			death_print = sys::locale("Death", 2);
			break;
		}

		case 4: // Drowned
		{
			nao::animate(2, Dir::Right);
			death_print = sys::locale("Death", 3);
			break;
		}

		case 5: // Angel Poison
		{
			death_print = sys::locale("Death", 4);
			break;
		}

		default: // Unknown
		{
			death_print = sys::locale("Death", 5);
			break;
		}
	}

	sys::lock();
	pxt::exit();
	sys::wait(1.5);

	pxt::load("game_over", 0.0f, 0.0f);

	sys::freeze();
	msg::topBox();
	msg::say(death_print);
	sys::wait();

	if(sys::getFlag(10))
	{
		msg::clear();
		msg::say(sys::locale("Death", 6));
		sys::wait();

		std::array<std::string> question = { 
			sys::locale("Main", 1), 
			sys::locale("Main", 2) 
		};

		msg::ask(question);

		if(msg::getAnswer() == 0)
		{
			msg::close();
			msg::fadeOut();
			sys::wait(0.5);
			pxt::exit();
			sys::loadProg();
		}

		else
		{
			msg::close();
			msg::fadeOut();
			pxt::fade(2.0f);
			sys::wait(2.0);
			pxt::exit();
			sys::reset();
		}
	}

	else
	{
		msg::close();
		msg::fadeOut();
		pxt::fade(2.0f);
		sys::wait(2.0);
		pxt::exit();
		sys::reset();
	}
}

//////////////////////////////////////////////////////////////////////////////////////////

void assign_item(const std::string &in wpn_name, arch_t index) // Set Weapon
{
	if(index != sys::getItemPtrIndex())
	{
		sys::setItemPtrIndex(index);
	}

	else
	{
		msg::say(wpn_name);
		sys::wait();
	}
}

void inven(arch_t type, arch_t index) // Using Inventory
{
	if(sys::getKeyHeld(Input::Yes))
	{
		msg::lowBox();

		if(type >= sys::locale("Inven"))
		{
			msg::say(sys::locale("Main", 3));
			sys::wait();
		}

		else if(type != 0)
		{
			assign_item(sys::locale("Inven", type), index);
		}

		else
		{
			msg::say(sys::locale("Inven", 0));
			sys::wait();
		}
	}

	else if(sys::getKeyHeld(Input::No))
	{
		sys::setItemPtrIndex();
		aud::play("selected");
	}
}