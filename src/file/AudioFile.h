/* wave to mp3 encoding using Lame lib
   @mpathak
*/
#ifndef _AUDIOFILE_H_
#define _AUDIOFILE_H_

class AudioFile {
protected:
	static const int MAX_EXTENSION				= 2;
	const char* m_FileExtension[MAX_EXTENSION]	= { ".mp3", ".wav" };

	AudioFile()									= default;
	AudioFile(const AudioFile&)					= default;
	AudioFile& operator = (const AudioFile&)	= default;
	AudioFile(AudioFile&&)						= delete;
	AudioFile& operator =(AudioFile&&)			= delete;

	virtual ~AudioFile() {
	}
};
#endif //_AUDIOFILE_H_