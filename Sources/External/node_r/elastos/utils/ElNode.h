
#ifndef __ELANDROID_H__
#define __ELANDROID_H__

#define _GNUC
#define _linux
#define _UNDEFDLLEXP
#define _DEBUG

#define ELASTOS
//#define ANDROID

//#define BUILDING_CHROMIUM__

#define _arm
//#define _x86
#define atomic_inc __atomic_inc
#define atomic_dec __atomic_dec

#ifndef __USE_MALLOC
#define __USE_MALLOC
#endif

/*
#undef WK_EXPORT
#if defined(WK_NO_EXPORT)
#define WK_EXPORT
#elif defined(__GNUC__) && !defined(__CC_ARM) && !defined(__ARMCC__)
#define WK_EXPORT __attribute__((visibility("default")))
#elif defined(WIN32) || defined(_WIN32) || defined(_WIN32_WCE) || defined(__CC_ARM) || defined(__ARMCC__)
#if BUILDING_WEBKIT
#define WK_EXPORT __declspec(dllexport)
#else
#define WK_EXPORT __declspec(dllimport)
#endif
#else // !defined(WK_NO_EXPORT)
#define WK_EXPORT
#endif // defined(WK_NO_EXPORT)
*/

#undef WK_EXPORT
#ifndef WK_EXPORT
#define WK_EXPORT __attribute__((visibility("default")))
#else
#warning XXXXXXXX_#WK_EXPORT
//#error check WKEXPORT definition first
#endif

#endif //__ELANDROID_H__
