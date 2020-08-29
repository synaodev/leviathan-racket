cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"dialogue_gui.cpp"
	"inventory_gui.cpp"
	"stack_gui.cpp"
	"wgt_audio.cpp"
	"wgt_field.cpp"
	"wgt_file.cpp"
	"wgt_input.cpp"
	"wgt_language.cpp"
	"wgt_option.cpp"
	"wgt_video.cpp"
	"widget.cpp"
)

target_sources (editor PRIVATE "attribute_gui.cpp")
