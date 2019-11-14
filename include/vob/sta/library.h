#pragma once

#include <vob/sta/platform.h>

#if defined(VOB_STA_WINDOWS)
#	define VOB_STD_LIB_EXPORT __declspec(dllexport)
#	define VOB_STD_LIB_IMPORT __declspec(dllimport)
#	define VOB_STD_LIB_DEPRECATED __declspec(deprecated)
#	ifdef _MSC_VER
#		pragma warning(disable: 4251)
#	endif
#elif __GNUC__ >= 4
#	define VOB_STD_LIB_EXPORT __attribute__ ((__visibility__ ("default")))
#	define VOB_STD_LIB_IMPORT __attribute__ ((__visibility__ ("default")))
#	define VOB_STD_LIB_DEPRECATED __attribute__ ((deprecated))
#else
#	define VOB_STD_LIB_EXPORT
#	define VOB_STD_LIB_IMPORT
#	define VOB_STD_LIB_DEPRECATED __attribute__ ((deprecated))
#endif
