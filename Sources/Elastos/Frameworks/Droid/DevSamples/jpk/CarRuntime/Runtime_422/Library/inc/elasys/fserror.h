//==========================================================================
// Copyright (c) 2000-2008,  Elastos, Inc.  All Rights Reserved.
//==========================================================================

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef __FSERROR_H__
#define __FSERROR_H__

#define E_FS_NO_PERMISSION      FILESYS_ERROR(0x01)    /* Not sufficient
                                                        * permissions */
#define E_FS_NO_SUCH_FILE       FILESYS_ERROR(0x03)    /* No such file */
#define E_FS_IO_ERROR           FILESYS_ERROR(0x05)    /* I/O error */
#define E_FS_TRY_AGAIN          FILESYS_ERROR(0x0b)    /* Try again later */
#define E_FS_FILE_BUSY          FILESYS_ERROR(0x10)    /* File busy */
#define E_FS_FILE_EXISTS        FILESYS_ERROR(0x11)    /* File exists */
#define E_FS_NOT_DIRECTORY      FILESYS_ERROR(0x14)    /* Not a directory */
#define E_FS_IS_DIRECTORY       FILESYS_ERROR(0x15)    /* Is a directory */
#define E_FS_NO_SPACE           FILESYS_ERROR(0x1c)    /* No space left
                                                        * on device */
#define E_FS_READ_ONLY          FILESYS_ERROR(0x1e)    /* Read-only file
                                                        * system */
#define E_FS_INVALID_HANDLE     FILESYS_ERROR(0x1f)    /* invalid handle */
#define E_FS_NO_ROOT            FILESYS_ERROR(0x21)    /* No root fs*/
#define E_FS_NO_ENTRY           FILESYS_ERROR(0x23)    /* No directory entry */

#endif // __FSERROR_H__
