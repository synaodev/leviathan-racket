cmake_minimum_required (VERSION 3.15)

target_sources (naomi PRIVATE
	"pxtn.h"
	"pxtnDelay.cpp" "pxtnDelay.h"
	"pxtnDescriptor.cpp" "pxtnDescriptor.h"
	"pxtnError.cpp" "pxtnError.h"
	"pxtnEvelist.cpp" "pxtnEvelist.h"
	"pxtnMaster.cpp" "pxtnMaster.h"
	"pxtnMax.h"
	"pxtnMem.cpp" "pxtnMem.h"
	"pxtnOverDrive.cpp" "pxtnOverDrive.h"
	"pxtnPulse_Frequency.cpp" "pxtnPulse_Frequency.h"
	"pxtnPulse_Noise.cpp" "pxtnPulse_Noise.h"
	"pxtnPulse_NoiseBuilder.cpp" "pxtnPulse_NoiseBuilder.h"
	"pxtnPulse_Oggv.cpp" "pxtnPulse_Oggv.h"
	"pxtnPulse_Oscillator.cpp" "pxtnPulse_Oscillator.h"
	"pxtnPulse_PCM.cpp" "pxtnPulse_PCM.h"
	"pxtnService.cpp" "pxtnService_moo.cpp" "pxtnService.h"
	"pxtnText.cpp" "pxtnText.h"
	"pxtnUnit.cpp" "pxtnUnit.h"
	"pxtnWoice.cpp" "pxtnWoice_io.cpp" "pxtnWoicePTV.cpp" "pxtnWoice.h"
	"pxtoneNoise.cpp" "pxtoneNoise.h"
)
