#include "FileReader.h"

namespace ion
{
	namespace audio
	{
		FileReader::FileReader(const char* filename)
		{
			m_filename = filename;
		}

		FileReader::~FileReader()
		{
		}
	}
}