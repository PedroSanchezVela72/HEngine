#ifndef AUDIO_SOURCE_H
#define AUDIO_SOURCE_H

#include "Component.h"

namespace FMOD {
	class Sound;
	class Channel; 
}
namespace H{

	class AudioSource : public Component {
	public:
		static constexpr cmpId_type id = _cmp_AUDIO_SOURCE;

		AudioSource(const std::list<std::string>& soundNameList, const std::unordered_map<std::string, std::string>& filePathMap, const std::unordered_map<std::string, FMOD::Sound*>& soundMap, float volume, unsigned int mode) :
			_filePaths(),
			_volume(volume),
			_mode(mode),
			_sounds(),
			_channel(nullptr)
		{
			_filePaths = filePathMap;
			_soundNames = soundNameList;
			_sounds = soundMap;
		};
		std::list<std::string> _soundNames;
		std::unordered_map<std::string, std::string> _filePaths;
		float _volume;
		unsigned int _mode;
		std::unordered_map<std::string, FMOD::Sound*> _sounds;
		FMOD::Channel* _channel;
	};
}


#endif