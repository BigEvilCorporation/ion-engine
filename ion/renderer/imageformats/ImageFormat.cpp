#include "ImageFormat.h"
#include "ImageFormatBMP.h"
#include "ImageFormatPNG.h"

#include <ion/core/string/String.h>

namespace ion
{
	ImageFormat* ImageFormat::CreateReader(const std::string& fileExtension)
	{
		std::string lowerCase = ion::string::ToLower(fileExtension);

		if (lowerCase == "bmp")
			return new ImageFormatBMP;
		else if (lowerCase == "png")
			return new ImageFormatPNG;

		return nullptr;
	}
}