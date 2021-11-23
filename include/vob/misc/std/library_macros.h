#pragma once

#include "platform_macros.h"

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
