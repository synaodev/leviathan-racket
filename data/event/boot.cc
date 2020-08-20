// Global

void init() {
	pxt::exit();
	msg::push_card("Leviathan Racket", font_t::One);
	msg::set_card_position(0, 160.0f, 48.0f);
	msg::set_card_centered(0, true, false);
	sys::wait(1.5f);
	msg::clear_cards();
	msg::fade_in();
}

void is_empty() {
	sys::freeze();
	msg::top_box();
	msg::say(sys::locale("IsEmpty", 0));
	sys::wait();
}

void use_bed() {
	sys::freeze();
	msg::top_box();

	std::array<std::string> question = {
		sys::locale("UseBed", 0),
		sys::locale("UseBed", 1),
		sys::locale("UseBed", 2)
	};
	msg::ask(question);
	arch_t answer = msg::get_answer();

	switch (answer) {
		case 0: /* If "Save Progress" */ {
			const arch_t kFileGUI = 1;
			sys::set_flag(10, true);
			msg::close();
			aud::play("inven");
			sys::push_widget(kFileGUI, 1);
			break;
		}
		case 1: /* If "Rest in Bed" */ {
			msg::close();
			pxt::fade(2.0f);
			msg::fade_out();
			sys::wait(1.0f);
			msg::top_box();
			msg::say(sys::locale("UseBed", 3));
			sys::wait();
			msg::close();
			nao::life_up(20);
			sys::wait(1.0f);
			msg::fade_in();
			pxt::redo(1.0f);
			sys::wait(0.5f);
			msg::top_box();
			msg::say(sys::locale("UseBed", 4));
			sys::wait();
			break;
		}
		default: {
			break;
		}
	}
}

void death(arch_t type) {
	std::string death_print;
	switch (type) {
		case 1: /* Disintegration */ {
			aud::play("player_death");
			aud::play("crash");
			death_print = sys::locale("Death", 1);
			break;
		}
		case 2: /* Normal */ {
			aud::play("player_death");
			death_print = sys::locale("Death", 0);
			break;
		}
		case 3: /* Fell Off Map */ {
			death_print = sys::locale("Death", 2);
			break;
		}
		case 4: /* Drowned */ {
			nao::animate(2, dir_t::Right);
			death_print = sys::locale("Death", 3);
			break;
		}
		case 5: /* Leviathan Poison */ {
			death_print = sys::locale("Death", 4);
			break;
		}
		default: /* Unknown */ {
			death_print = sys::locale("Death", 5);
			break;
		}
	}

	sys::lock();
	pxt::exit();
	sys::wait(1.5f);

	pxt::load("game_over", 0.0f, 0.0f);
	sys::freeze();
	msg::top_box();
	msg::say(death_print);
	sys::wait();

	if (sys::get_flag(10)) {
		msg::clear();
		msg::say(sys::locale("Death", 6));
		sys::wait();
		std::array<std::string> question = {
			sys::locale("Main", 1),
			sys::locale("Main", 2)
		};
		msg::ask(question);
		if (msg::get_answer() == 0) {
			msg::close();
			msg::fade_out();
			sys::wait(0.5f);
			pxt::exit();
			sys::load_progress();
		} else {
			msg::close();
			msg::fade_out();
			pxt::fade(2.0f);
			sys::wait(2.0f);
			pxt::exit();
			sys::boot();
		}
	} else {
		msg::close();
		msg::fade_out();
		pxt::fade(2.0f);
		sys::wait(2.0f);
		pxt::exit();
		sys::boot();
	}
}

void inventory(arch_t type, arch_t index) /* Inventory */ {
	if (sys::get_key_held(input_t::Yes)) {
		msg::low_box();
		if (type >= sys::locale("Inven")) {
			msg::say(sys::locale("Main", 3));
			sys::wait();
		} else if (type != 0) {
            if (index != sys::get_item_ptr_index()) {
                sys::set_item_ptr_index(index);
            } else {
                msg::say(sys::locale("Inven", type));
                sys::wait();
            }
		} else {
			msg::say(sys::locale("Inven", 0));
			sys::wait();
		}
	} else if (sys::get_key_held(input_t::No)) {
		sys::set_item_ptr_index();
		aud::play("selected");
	}
}
