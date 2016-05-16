//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __ELAERROR_H__
#define __ELAERROR_H__

#include <carerr.h>

//  Elastos extend ECODEs' layout:
//
//   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
//   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
//  +-+-+-+-+-+-+-+-+---------------+-------------------------------+
//  |S| family code |   info code   |         debug code            |
//  +-+-+-+-+-+-+-+-+---------------+-------------------------------+
//

#define KERNEL_ERROR(c)         MAKE_ECODE(SEVERITY_ERROR, FAMILY_KERNEL, c)
#define DRIVER_ERROR(c)         MAKE_ECODE(SEVERITY_ERROR, FAMILY_DRIVER, c)
#define FILESYS_ERROR(c)        MAKE_ECODE(SEVERITY_ERROR, FAMILY_FILESYS, c)
#define TCPIP_ERROR(c)          MAKE_ECODE(SEVERITY_ERROR, FAMILY_TCPIP, c)
#define DRM_ERROR(c)            MAKE_ECODE(SEVERITY_ERROR, FAMILY_DRM, c)
#define DATABASE_ERROR(c)       MAKE_ECODE(SEVERITY_ERROR, FAMILY_DATABASE, c)
#define XML_ERROR(c)            MAKE_ECODE(SEVERITY_ERROR, FAMILY_XML, c)
#define XMLGLUE_ERROR(c)        MAKE_ECODE(SEVERITY_ERROR, FAMILY_XMLGLUE, c)

#define FAMILY_MOBILE_ERROR(c)  MAKE_ECODE(SEVERITY_ERROR, FAMILY_MOBILE, c)
#define FAMILY_GRAPHIC_ERROR(c) MAKE_ECODE(SEVERITY_ERROR, FAMILY_GRAPHIC, c)
#define CAR_ERROR(c)            MAKE_ECODE(SEVERITY_ERROR, FAMILY_CAR, c)
// CAR Error codes
#define REFLECTION_ERROR(c)    MAKE_ECODE(SEVERITY_ERROR, FAMILY_REFLECTION, c)
#define STREAM_ERROR(c)        MAKE_ECODE(SEVERITY_ERROR, FAMILY_STREAM, c)

#define KERNEL_SUCCESS(c)       MAKE_SUCCESS(FAMILY_KERNEL, c)
#define DRIVER_SUCCESS(c)       MAKE_SUCCESS(FAMILY_DRIVER, c)
#define FILESYS_SUCCESS(c)      MAKE_SUCCESS(FAMILY_FILESYS, c)
#define TCPIP_SUCCESS(c)        MAKE_SUCCESS(FAMILY_TCPIP, c)

//
//=========================================================================
//Elastos file system error codes(Family:0x3)

#include <fserror.h>

#define ECODE_FROM_ERRNO(x)    \
    (_ELASTOS ECode) ( x ? MAKE_ECODE(FAMILY_CRT, x, SEVERITY_ERROR) : 0)

#define ECODE_TO_ERRNO(x) ((x >> 16) & 0xFF)

//==========================================================================
// Elastos kernel error codes (Family: 0x0)

#define E_PROCESS_NOT_ACTIVE            KERNEL_ERROR(0x01) // 0x81010000
#define E_PROCESS_STILL_ACTIVE          KERNEL_ERROR(0x02) // 0x81020000
#define E_PROCESS_NOT_STARTED           KERNEL_ERROR(0x03) // 0x81030000
#define E_PROCESS_ALREADY_STARTED       KERNEL_ERROR(0x04) // 0x81040000
#define E_PROCESS_ALREADY_EXITED        KERNEL_ERROR(0x05) // 0x81050000
#define E_PROCESS_NOT_EXITED            KERNEL_ERROR(0x06) // 0x81060000
#define E_THREAD_NOT_ACTIVE             KERNEL_ERROR(0x07) // 0x81070000
#define E_THREAD_STILL_ACTIVE           KERNEL_ERROR(0x08) // 0x81080000
#define E_THREAD_UNSTARTED              KERNEL_ERROR(0X09) // 0x81090000
#define E_THREAD_ALREADY_FINISHED       KERNEL_ERROR(0x0A) // 0x810A0000
#define E_THREAD_NOT_STOPPED            KERNEL_ERROR(0x0B) // 0x810B0000
#define E_DOES_NOT_EXIST                KERNEL_ERROR(0x0C) // 0x810C0000
#define E_ALREADY_EXIST                 KERNEL_ERROR(0x0D) // 0x810D0000
#define E_INVALID_OPTIONS               KERNEL_ERROR(0x0E) // 0x810E0000
#define E_INVALID_OPERATION             KERNEL_ERROR(0x0F) // 0x810F0000
#define E_TIMED_OUT                     KERNEL_ERROR(0x10) // 0x81100000
#define E_INTERRUPTED                   KERNEL_ERROR(0x11) // 0x81110000
#define E_NOT_OWNER                     KERNEL_ERROR(0x12) // 0x81120000
#define E_ALREADY_LOCKED                KERNEL_ERROR(0x13) // 0x81130000
#define E_INVALID_LOCK                  KERNEL_ERROR(0x14) // 0x81140000
#define E_NOT_READER                    KERNEL_ERROR(0x15) // 0x81150000
#define E_NOT_WRITER                    KERNEL_ERROR(0x16) // 0x81160000
#define E_NOT_ENOUGH_ADDRESS_SPACE      KERNEL_ERROR(0x17) // 0x81170000
#define E_BAD_FILE_FORMAT               KERNEL_ERROR(0x18) // 0x81180000
#define E_BAD_EXE_FORMAT                KERNEL_ERROR(0x19) // 0x81190000
#define E_BAD_DLL_FORMAT                KERNEL_ERROR(0x1A) // 0x811A0000
#define E_PATH_TOO_LONG                 KERNEL_ERROR(0x1B) // 0x811B0000
#define E_PATH_NOT_FOUND                KERNEL_ERROR(0x1C) // 0x811C0000
#define E_FILE_NOT_FOUND                KERNEL_ERROR(0x1D) // 0x811D0000
#define E_NOT_SUPPORTED                 KERNEL_ERROR(0x1E) // 0x811E0000
#define E_IO                            KERNEL_ERROR(0x1F) // 0x811F0000
#define E_BUFFER_TOO_SMALL              KERNEL_ERROR(0x20) // 0x81200000
#define E_THREAD_ABORTED                KERNEL_ERROR(0x21) // 0x81210000
#define E_SERVICE_NAME_TOO_LONG         KERNEL_ERROR(0x22) // 0x81220000
#define E_READER_LOCKS_TOO_MANY         KERNEL_ERROR(0x23) // 0x81230000
#define E_ACCESS_DENIED                 KERNEL_ERROR(0x24) // 0x81240000
#define E_OUT_OF_MEMORY                 KERNEL_ERROR(0x25) // 0x81250000
#define E_INVALID_ARGUMENT              KERNEL_ERROR(0x26) // 0x81260000
#define E_NOT_CALLBACK_THREAD           KERNEL_ERROR(0x27) // 0x81270000
#define E_VERIFY_FAILED                 KERNEL_ERROR(0x28) // 0x81280000

#define E_CANCLE_BOTH_EVENTS            KERNEL_ERROR(0x2E) // 0x812E0000
#define E_NO_DELEGATE_REGISTERED        KERNEL_ERROR(0x2F) // 0x812F0000
#define E_DELEGATE_ALREADY_REGISTERED   KERNEL_ERROR(0x30) // 0x81300000
#define E_CALLBACK_CANCELED             KERNEL_ERROR(0x31) // 0x81310000
#define E_CALLBACK_IS_BUSY              KERNEL_ERROR(0x32) // 0x81320000

#define S_TIMED_OUT                     KERNEL_SUCCESS(0x01)   // 0x01010000
#define S_INTERRUPTED                   KERNEL_SUCCESS(0x02)   // 0x01020000
#define S_NOT_EXIST                     KERNEL_SUCCESS(0x03)   // 0x01030000
#define S_ALREADY_EXISTS                KERNEL_SUCCESS(0x04)   // 0x01040000
#define S_BUFFER_TOO_SMALL              KERNEL_SUCCESS(0x05)   // 0x01050000
#define S_WITHOUT_LOCK                  KERNEL_SUCCESS(0x06)   // 0x01060000

//==========================================================================
// Driver error codes (Family: 0x1)
// Error codes 0x001 - 0x04f are reserved for system

// The length of a device name exceeds the maximum length allowed.
#define E_DEVICE_NAME_TOO_LONG              DRIVER_ERROR(0x01)

// The specified device already exists.
#define E_DEVICE_EXISTS                     DRIVER_ERROR(0x02)

// The system cannot find the device specified.
#define E_DEVICE_NOT_FOUND                  DRIVER_ERROR(0x03)

// The specified device is busy.
#define E_DRIVER_BUSY                       DRIVER_ERROR(0x04)

// The system cannot find the DMA Controller.
#define E_DMAC_NOT_FOUND                    DRIVER_ERROR(0x05)

// The system cannot find the driver specified.
#define ELADRV_S_DRIVER_NOT_FOUND           DRIVER_SUCCESS(0x01)

// The specified driver cannot be created.
#define ELADRV_S_CREATE_DRIVER_FAILED       DRIVER_SUCCESS(0x02)

// The specified driver cannot be registered.
#define ELADRV_S_REGISTER_DRIVER_FAILED     DRIVER_SUCCESS(0x03)

// The specified driver cannot be unregistered.
#define ELADRV_S_UNREGISTER_DRIVER_FAILED   DRIVER_SUCCESS(0x04)

//==========================================================================
// Macros and constants for FACILITY_CAR error codes
//==========================================================================

// No class information
#define E_NO_CLASS_INFO                     CAR_ERROR(0X01)

// not a export server
#define E_NO_EXPORT_SERVER                  CAR_ERROR(0X02)

// not import server
#define E_NO_IMPORT_SERVER                  CAR_ERROR(0X03)

// marshaling: data transport error
#define E_MARSHAL_DATA_TRANSPORT_ERROR      CAR_ERROR(0X04)

#define E_ERROR_STRING                      CAR_ERROR(0x10)

#define E_ERROR_STRING_A                    CAR_ERROR(0x11)

#define E_ERROR_URL                         CAR_ERROR(0x12)

// IConnectionPoint status codes
//
// there is no connection for this connection id
#define E_CONNECT_NOCONNECTION              CAR_ERROR(0x13)

// this implementation's limit for advisory connections has been reached
#define E_CONNECT_ADVISELIMIT               CAR_ERROR(0x14)

// connection attempt failed
#define E_CONNECT_CANNOTCONNECT             CAR_ERROR(0x15)

//// must use a derived interface to connect
#define E_CONNECT_OVERRIDDEN                CAR_ERROR(0x16)

//==========================================================================
// Refection error codes (Family: 0x10)
#define E_OUT_OF_NUMBER                     REFLECTION_ERROR(0x01)
#define E_DATAINFO_EXIST                    REFLECTION_ERROR(0x02)

//==========================================================================
// Stream error codes (Family: 0x11)
#define E_OUT_OF_STREAM                     STREAM_ERROR(0x01)
#define E_CLOSED_STREAM                     STREAM_ERROR(0x02)

//==========================================================================
// DRM error codes (Family: 0x6)
#define E_DRM_FILE_PARSE                    DRM_ERROR(0X01)
#define E_DRM_NOT_OPEN_RODB                 DRM_ERROR(0X02)
#define E_DRM_NORO                          DRM_ERROR(0X03)
#define E_DRM_FILE_OP                       DRM_ERROR(0X04)
#define E_DRM_NO_COUNT                      DRM_ERROR(0X05)
#define E_DRM_OUT_OF_DATE                   DRM_ERROR(0X06)
#define E_DRM_MEDIA_OBJECT_EXIST            DRM_ERROR(0X07)
#define E_DRM_RO_EXIST                      DRM_ERROR(0X08)
#define E_DRM_PERMISSION_DISABLE            DRM_ERROR(0X09)
#define E_DRM_INVALID_RO                    DRM_ERROR(0X0A)
#define E_DRM_DB_KEY_NOT_EXIST              DRM_ERROR(0X0B)
#define E_DRM_INVALID_XML_FORMAT            DRM_ERROR(0X0C)
#define E_DRM_TIME_UNAVOIDABLE              DRM_ERROR(0X0D)

//==========================================================================
// Database error codes (Family: 0xB)

#define E_DB_SQL_ERROR                  DATABASE_ERROR(0x01)
#define E_DB_INTERNAL_ERROR             DATABASE_ERROR(0x02)
#define E_DB_PERMISSION_DENIED          DATABASE_ERROR(0x03)
#define E_DB_REQUESTED_ABORT            DATABASE_ERROR(0x04)
#define E_DB_FILE_BUSY                  DATABASE_ERROR(0x05)
#define E_DB_TABLE_LOCKED               DATABASE_ERROR(0x06)
#define E_DB_OUT_OF_MEMORY              DATABASE_ERROR(0x07)
#define E_DB_READ_ONLY_DATABASE         DATABASE_ERROR(0x08)
#define E_DB_OPERATION_INTERRUPTED      DATABASE_ERROR(0x09)
#define E_DB_DISK_IO_ERROR              DATABASE_ERROR(0x0A)
#define E_DB_FILE_CORRUPTED             DATABASE_ERROR(0x0B)
#define E_DB_TABLE_NOT_FOUND            DATABASE_ERROR(0x0C)
#define E_DB_DATABASE_FULL              DATABASE_ERROR(0x0D)
#define E_DB_CANT_OPEN_DATABASE         DATABASE_ERROR(0x0E)
#define E_DB_LOCK_PROTOCOL_ERROR        DATABASE_ERROR(0x0F)
#define E_DB_DATABSE_EMPTY              DATABASE_ERROR(0x10)
#define E_DB_SCHEMA_CHANGED             DATABASE_ERROR(0x11)
#define E_DB_DATA_TOO_BIG               DATABASE_ERROR(0x12)
#define E_DB_CONSTRAINT_VIOLATION       DATABASE_ERROR(0x13)
#define E_DB_DATA_TYPE_MISMATCH         DATABASE_ERROR(0x14)
#define E_DB_LIBRARY_MISUSE             DATABASE_ERROR(0x15)
#define E_DB_NO_OS_SUPPORT              DATABASE_ERROR(0x16)
#define E_DB_AUTHORIZATION_DENIED       DATABASE_ERROR(0x17)
#define E_DB_AUXILIARY_DB_FORMAT_ERROR  DATABASE_ERROR(0x18)
#define E_DB_PARAMETER_OUT_OF_RANGE     DATABASE_ERROR(0x19)
#define E_DB_NOT_A_DATABASE_FILE        DATABASE_ERROR(0x1A)
#define E_DB_OPERATE_ON_CLOSED_OBJECT   DATABASE_ERROR(0x30)

#endif // __ELAERROR_H__
