cmake_minimum_required (VERSION 3.13)

target_sources (lvrk PRIVATE
	"pxtnDelay.cpp"
	"pxtnDescriptor.cpp"
	"pxtnError.cpp"
	"pxtnEvelist.cpp"
	"pxtnMaster.cpp"
	"pxtnMem.cpp"
	"pxtnOverDrive.cpp"
	"pxtnPulse_Frequency.cpp" "pxtnPulse_Noise.cpp"
	"pxtnPulse_NoiseBuilder.cpp" "pxtnPulse_Oggv.cpp"
	"pxtnPulse_Oscillator.cpp" "pxtnPulse_PCM.cpp"
	"pxtnService.cpp" "pxtnService_moo.cpp"
	"pxtnText.cpp"
	"pxtnUnit.cpp"
	"pxtnWoice.cpp" "pxtnWoice_io.cpp" "pxtnWoicePTV.cpp"
	"pxtoneNoise.cpp"
)
