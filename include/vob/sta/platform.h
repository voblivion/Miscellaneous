#pragma once

#if defined(_WIN32)
	// Windows
	#define VOB_STA_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
	#include "TargetConditionals.h"
	#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
		// iOS
		#define VOB_STA_IOS
	#elif TARGET_OS_MAC
		// MacOS
		#define VOB_STA_MACOS
	#else
		#error This Apple operating system is not supported
	#endif
#elif defined(__unix__)
	// UNIX system, see which one it is
	#if defined(__ANDROID__)
		// Android
		#define VOB_STA_ANDROID
	#elif defined(__linux__)
		 // Linux
		#define VOB_STA_LINUX
	#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
		// FreeBSD
		#define VOB_STA_FREEBSD
	#elif defined(__OpenBSD__)
		// OpenBSD
		#define VOB_STA_OPENBSD
	#else
		#error This UNIX operating system is not supported
	#endif
#else
	#error This operating system is not supported
#endif
