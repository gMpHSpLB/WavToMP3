/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _STREAMINFO_H_
#define _STREAMINFO_H_
#include "lame.h"

class StreamInfo {
	int m_Quality					= 3; 
	int m_SampleRate				= 44100; // Hz
	vbr_mode m_VbrMode				= vbr_default;
	MPEG_mode m_MpegMode			= JOINT_STEREO;
	int m_NumberOfChannels			= 2;
	unsigned long m_NumberOfSamples = 0;
	int m_ReplayGainAnalysis		= 1;
	unsigned int m_NumBitsPerSample = 0;
	unsigned int m_FrameSize		= 0;

public:
	StreamInfo()								= default;
	StreamInfo(const StreamInfo&)				= default;
	StreamInfo& operator = (const StreamInfo&)	= default;
	StreamInfo(StreamInfo&&)					= default;
	StreamInfo& operator = (StreamInfo&&)		= default;

	void SetQuality(int quality) { m_Quality = quality; }
	int GetQuality() const { return(m_Quality); }
	void SetNumberOfChannels(int numChannels) { m_NumberOfChannels = numChannels; }
	int GetNumberOfChannels() const { return(m_NumberOfChannels); }
	void SetSampleRate(int sampleRate) { m_SampleRate = sampleRate; }
	int GetSampleRate() const { return(m_SampleRate); }
	void SetBitsPerSample(int bitsPerSample) { m_NumBitsPerSample = bitsPerSample; }
	int GetBitsPerSample() const { return(m_NumBitsPerSample); }
	void SetVbrMode(vbr_mode vbrMode) { m_VbrMode = vbrMode; }
	vbr_mode GetVbrMode() const { return(m_VbrMode); }
	void SetMPEGMode(MPEG_mode mpegMode) { m_MpegMode = mpegMode; }
	MPEG_mode GetMPEGMode() const { return(m_MpegMode); }
	void SetNumberOfSamples(unsigned long numOfSamples) { m_NumberOfSamples = numOfSamples; }
	unsigned long GetNumberOfSamples() const { return(m_NumberOfSamples); }
	void SetReplayGainAnalysis(int replayGainAnalysis) { m_ReplayGainAnalysis = replayGainAnalysis; }
	int GetReplayGainAnalysis() const { return(m_ReplayGainAnalysis); }
	void SetFrameSize(int frameSize) { m_FrameSize = frameSize; }
	int GetFrameSize() const { return(m_FrameSize); }
};
#endif //_STREAMINFO_H_