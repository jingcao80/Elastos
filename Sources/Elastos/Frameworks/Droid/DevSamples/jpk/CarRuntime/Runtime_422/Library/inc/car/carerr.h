//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#ifndef __CAR_CARERR_H__
#define __CAR_CARERR_H__

//  ECODEs' layout:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+-+-+-+---------------+-------------------------------+
//  |S| family code |   info code   |         debug code            |
//  +-+-+-+-+-+-+-+-+---------------+-------------------------------+
//
//

#include <elatypes.h>

// Success codes
//
#ifndef NOERROR
#define NOERROR         ((_ELASTOS ECode)0x00000000L)
#endif

#define NOERROR_EXIT    ((_ELASTOS ECode)0x00000001L)

#define S_FALSE         ((_ELASTOS ECode)0x00000001L)
//#define FALSE    0
//
// Severity values
//
#define SEVERITY_SUCCESS    0
#define SEVERITY_ERROR      1

//
// Create an ECode value from component pieces
//

#define MAKE_ECODE(sev, family, code) \
        (_ELASTOS ECode)(((family << 24) | (code << 16 )) | (sev << 31))

#define MAKE_SUCCESS(family, code) \
        (_ELASTOS ECode)(((family << 24) | (code << 16 )) & 0x7FFFFFFF)

#define ERROR_DETAIL(c) \
        (_ELASTOS ECode)((c) & 0x0000FFFF)

#define ERROR(c) \
        (_ELASTOS ECode)((c) & 0xFFFF0000)

#define SUCCEEDED(x)    ((_ELASTOS ECode)(x) >= 0)
#define FAILED(x)       ((_ELASTOS ECode)(x) < 0)
//
// Define the family codes
//
#define FAMILY_NULL         0x00    // Elastos interface error codes
#define FAMILY_KERNEL       0x01    // Elastos kernel error codes
#define FAMILY_DRIVER       0x02    // Driver error codes
#define FAMILY_CRT          0x03    // ECode from CRT error number
#define FAMILY_FILESYS      0x04    // Elastos file system error codes
#define FAMILY_TCPIP        0x05    // Elastos network error codes
#define FAMILY_DRM          0x06    // Elastos drm error codes
#define FAMILY_CAR          0x07    // CAR Error codes

#define FAMILY_MOBILE       0x08    // Smartphone error codes
#define FAMILY_GRAPHIC      0x09    // graphic error codes
#define FAMILY_DATABASE     0x0B    // Database error codes
#define FAMILY_XML          0x0E    // XML error codes
#define FAMILY_XMLGLUE      0x0F    // XML_Glue error codes

#define FAMILY_REFLECTION   0x10    // Reflection Error codes
#define FAMILY_STREAM       0X11    // Stream Error codes

//==========================================================================
// BUGBUG: Remove these in future. Only for passing compilation.
#define ECODE_FROM_WIN32(x) (x)
#define ELASTOS_ERROR(x) (x)
#define FACILITY_ITF 0x80808080
//==========================================================================

//==========================================================================
// Macros and constants for FAMILY_NULL error codes
//==========================================================================
// BUGBUG
#define E_UNEXPECTED            MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x00) // 0x80000000

// Not implemented
#define E_NOT_IMPLEMENTED       MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x01) // 0x80010000

// No such interface supported
#define E_NO_INTERFACE          MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x02) // 0x80020000

// Invalid pointer
// TODO: Use E_INVALIDARG replaceing E_INVALID_POINTER
#define E_INVALID_POINTER       MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x03) // 0x80030000

// Operation aborted
#define E_ABORT                 MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x04) // 0x80040000

// Unspecified error
#define E_FAIL                  MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x05) // 0x80050000

// No default constructor in class
#define E_NO_DEFAULT_CTOR       MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x06) // 0x80060000

// Class does not support aggregation (or class object is remote,
// SEVERITY_ERROR)
#define E_CLASS_NO_AGGREGATION  MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x07) // 0x80070000

// ClassFactory cannot supply requested class
#define E_CLASS_NOT_AVAILABLE   MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x08) // 0x80080000

// CATID does not exist
#define E_CATID_NOT_EXIST       MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x09) // 0x80090000

// Description not found
#define CAT_E_NODESCRIPTION     MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0A) // 0x800A0000

//aspect cannot Aggregate aspect
#define E_ASPECT_CANNOT_AGGREGATE_ASPECT \
                                MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0B) // 0x800B0000

//Unaggregate failed
#define E_UNAGGREGATE_FAILED    MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0C) // 0x800C0000

// No generic defined
#define E_NO_GENERIC            MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0D) // 0x800D0000

// Wrong generic defined
#define E_WRONG_GENERIC         MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0E) // 0x800E0000

//Aspect refuse to attach
#define E_ASPECT_REFUSE_TO_ATTACH \
                                MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x0F) // 0x800F0000

//Aspect object has been unaggregated
#define E_ZOMBIE_ASPECT         MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x10) // 0x80100000

//Aspect has been aggregated
#define E_DUPLICATE_ASPECT      MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x11) // 0x80110000

//A class with local constructor or interface cannot be created in diff process
#define E_CONFLICT_WITH_LOCAL_KEYWORD \
                                MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x12) // 0x80120000

#define E_NOT_IN_PROTECTED_ZONE \
                                MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x13) // 0x80130000

#define CO_NOTALLINTERFACES     MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x14) // 0x80140000

// Reference count of dll is not zero
#define E_DLL_CANNOT_UNLOAD_NOW MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x15) // 0x80150000

// It is not standard car dll
#define E_NOT_CAR_DLL           MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x16) // 0x80160000

// Dll is not in active dll list, maybe has been unloaded or hasn't been loaded.
#define E_DLL_NOT_LOADED        MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x17) // 0x80170000

// Removing failed for the event is being called back.
#define E_EVENT_BEING_CALLED   MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x18) // 0x80180000

#define E_REMOTE_EXCEPTION     MAKE_ECODE(SEVERITY_ERROR, FAMILY_NULL, 0x19) // 0x80190000
#endif // __CAR_CARERR_H__
