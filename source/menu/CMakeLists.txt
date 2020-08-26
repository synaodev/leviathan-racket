cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"dialogue_gui.cpp" "dialogue_gui.hpp"
	"inventory_gui.cpp" "inventory_gui.hpp"
	"stack_gui.cpp" "stack_gui.hpp"
	"wgt_audio.cpp" "wgt_audio.hpp"
	"wgt_field.cpp" "wgt_field.hpp"
	"wgt_file.cpp" "wgt_file.hpp"
	"wgt_input.cpp" "wgt_input.hpp"
	"wgt_language.cpp" "wgt_language.hpp"
	"wgt_option.cpp" "wgt_option.hpp"
	"wgt_video.cpp" "wgt_video.hpp"
	"widget.cpp" "widget.hpp"
)

target_sources (editor PRIVATE
	"attribute_gui.cpp" "attribute_gui.hpp"
)
