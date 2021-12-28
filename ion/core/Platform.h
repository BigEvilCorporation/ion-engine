#pragma once

#include <functional>

namespace ion
{
	extern int EntryPoint(int numargs, char** args);

	namespace platform
	{
		enum class Language
		{
			AmericanEnglish,
			BritishEnglish,
			Japanese,
			French,
			German,
			LatinAmericanSpanish,
			Spanish,
			Italian,
			Dutch,
			CanadianFrench,
			Portuguese,
			Russian,
			SimplifiedChinese,
			TraditionalChinese,
			Korean,
			Unsupported
		};

		enum class SystemMenu
		{
			Dashboard,	// Native platform dashboard/home screen
			Error,		// Native platform error screen
			Controls,	// Native platform controls settings menu
		};

		typedef std::function<void(SystemMenu menuType, bool opened)> SystemMenuCallback;

		void Initialise();
		void Shutdown();
		Language GetSystemLanguage();

		void RegisterCallbackSystemMenu(SystemMenuCallback const& callback);
	}
}

#if defined ION_PLATFORM_WINDOWS
#include "platform/windows/Windows.h"
#elif defined ION_PLATFORM_LINUX
#include "platform/linux/Linux.h"
#elif defined ION_PLATFORM_MACOSX
#include "platform/macosx/MacOSX.h"
#elif defined ION_PLATFORM_DREAMCAST
#include "platform/dreamcast/Dreamcast.h"
#elif defined ION_PLATFORM_SWITCH
#include "platform/nx/Switch.h"
#elif defined ION_PLATFORM_ANDROID
#include "platform/android/Android.h"
#endif
