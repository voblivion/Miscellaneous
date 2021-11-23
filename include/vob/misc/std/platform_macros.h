#pragma once

//
#if defined(_WIN32)
#	define VOB_MISTD_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
#	include "TargetConditionals.h"
#	if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#		define VOB_MISTD_IOS
#	elif TARGET_OS_MAC
#		define VOB_MISTD_MACOS
#	else
#		error This Apple operating system is not supported
#	endif
#elif defined(__unix__)
#	if defined(__ANDROID__)
#		define VOB_MISTD_ANDROID
#	elif defined(__linux__)
#		define VOB_MISTD_LINUX
#	elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#		define VOB_MISTD_FREEBSD
#	elif defined(__OpenBSD__)
#		define VOB_MISTD_OPENBSD
#	else
#		error This UNIX operating system is not supported
#	endif
#else
#	error This operating system is not supported
#endif

#if defined(VOB_MISTD_WINDOWS)
#	define VOB_MISTD_LIB_EXPORT __declspec(dllexport)
#	define VOB_MISTD_LIB_IMPORT __declspec(dllimport)
#	define VOB_MISTD_LIB_DEPRECATED __declspec(deprecated)
#	ifdef _MSC_VER
#		pragma warning(disable: 4251)
#	endif
#elif __GNUC__ >= 4
#	define VOB_MISTD_LIB_EXPORT __attribute__ ((__visibility__ ("default")))
#	define VOB_MISTD_LIB_IMPORT __attribute__ ((__visibility__ ("default")))
#	define VOB_MISTD_LIB_DEPRECATED __attribute__ ((deprecated))
#else
#	define VOB_MISTD_LIB_EXPORT
#	define VOB_MISTD_LIB_IMPORT
#	define VOB_MISTD_LIB_DEPRECATED __attribute__ ((deprecated))
#endif
