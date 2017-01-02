//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Elastos.CoreLibrary.h"

using Elastos::Sql::SQLite::ICallback;
using Elastos::Sql::SQLite::IBusyHandler;
using Elastos::Sql::SQLite::IAuthorizer;
using Elastos::Sql::SQLite::ITrace;
using Elastos::Sql::SQLite::IProfile;
using Elastos::Sql::SQLite::IProgressHandler;
using Elastos::Sql::SQLite::IFunctionContext;
using Elastos::Sql::SQLite::IFunction;
using Elastos::Sql::SQLite::IDatabase;


#define HAVE_SQLITE2 0
#define HAVE_SQLITE3 1
#define HAVE_SQLITE_FUNCTION_TYPE 0
#define HAVE_SQLITE_OPEN_AUX_FILE 0
#define HAVE_SQLITE_SET_AUTHORIZER 0
#define HAVE_SQLITE_TRACE 0
#define HAVE_SQLITE_COMPILE 0
#define HAVE_SQLITE_PROGRESS_HANDLER 0
#define HAVE_SQLITE3_MALLOC 1
#define HAVE_SQLITE3_PREPARE_V2 0
#define HAVE_SQLITE3_PREPARE16_V2 0
#define HAVE_SQLITE3_BIND_ZEROBLOB 0
#define HAVE_SQLITE3_CLEAR_BINDINGS 0
#define HAVE_SQLITE3_COLUMN_TABLE_NAME16 0
#define HAVE_SQLITE3_COLUMN_DATABASE_NAME16 0
#define HAVE_SQLITE3_COLUMN_ORIGIN_NAME16 0
#define HAVE_SQLITE3_BIND_PARAMETER_COUNT 1
#define HAVE_SQLITE3_BIND_PARAMETER_NAME 1
#define HAVE_SQLITE3_BIND_PARAMETER_INDEX 1
#define HAVE_SQLITE3_RESULT_ZEROBLOB 0
#define HAVE_SQLITE3_INCRBLOBIO 0

#define CANT_PASS_VALIST_AS_CHARPTR

#if HAVE_SQLITE2
#include <sqlite.h>
#endif

#if HAVE_SQLITE3
#include <sqlite3.h>
#undef  HAVE_SQLITE_COMPILE
#define HAVE_SQLITE_COMPILE 1
#undef  HAVE_SQLITE_PROGRESS_HANDLER
#define HAVE_SQLITE_PROGRESS_HANDLER 1
#undef  HAVE_SQLITE_TRACE
#define HAVE_SQLITE_TRACE 1
#if !HAVE_SQLITE3_MALLOC
#define sqlite3_malloc malloc
#define sqlite3_free   free
#endif
#if !HAVE_SQLITE3_BIND_PARAMETER_COUNT
#define sqlite3_bind_parameter_count(dummy) (1000)
#endif
#endif

#if HAVE_SQLITE2 && HAVE_SQLITE3
#define HAVE_BOTH_SQLITE 1
#endif

#ifndef HAVE_SQLITE3_SHARED_CACHE
#define HAVE_SQLITE3_SHARED_CACHE 0
#endif

#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
#define MAX_PARAMS 256
#else
#define MAX_PARAMS 32
#endif

/* free memory proc */

typedef void (freemem)(void *);

/* internal handle for SQLite database */

typedef struct {
    void *sqlite;           /* SQLite handle */
#if HAVE_BOTH_SQLITE
    Int32 is3;              /* True for SQLITE3 handle */
#endif
    Int32 ver;              /* version code */
    IBusyHandler * bh;      /* BusyHandler object */
    ICallback * cb;         /* Callback object */
    IAuthorizer * ai;       /* Authorizer object */
    ITrace * tr;            /* Trace object */
    IProfile * pr;          /* Profile object */
    IProgressHandler * ph;  /* ProgressHandler object */
    Int32 row1;             /* true while processing first row */
    Int32 haveutf;          /* true for SQLite UTF-8 support */
    String * enc;             /* encoding or 0 */
    struct hfunc *funcs;    /* SQLite user defined function handles */
#if HAVE_SQLITE_COMPILE
    struct hvm *vms;        /* Compiled SQLite VMs */
#endif
#if HAVE_SQLITE3
    sqlite3_stmt *stmt;     /* For callback() */
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    struct hbl *blobs;      /* SQLite3 blob handles */
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
  struct hbk *backups;      /* SQLite3 backup handles */
#endif
} handle;

/* internal handle for SQLite user defined function */

typedef struct hfunc {
    struct hfunc *next;     /* next function */
#if HAVE_BOTH_SQLITE
    Int32 is3;              /* True for SQLITE3 handle */
#endif
    IFunctionContext * fc;  /* FunctionContext object */
    IFunction * fi;         /* Function object */
    IDatabase * db;         /* Database object */
    handle *h;              /* SQLite database handle */
    void *sf;               /* SQLite function handle */
} hfunc;

#if HAVE_SQLITE_COMPILE
/* internal handle for SQLite VM (sqlite_compile()) */

typedef struct hvm {
    struct hvm *next;       /* next vm handle */
#if HAVE_BOTH_SQLITE
    Int32 is3;              /* True for SQLITE3 handle */
#endif
    void *vm;               /* SQLite 2/3 VM/statement */
    char *tail;             /* tail SQL string */
    Int32 tail_len;         /* only for SQLite3/prepare */
    handle *h;              /* SQLite database handle */
    handle hh;              /* fake SQLite database handle */
} hvm;
#endif

#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
/* internal handle for sqlite3_blob */

typedef struct hbl {
    struct hbl *next;       /* next blob handle */
    sqlite3_blob *blob;     /* SQLite3 blob */
    handle *h;              /* SQLite database handle */
} hbl;
#endif

#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
/* internal handle for sqlite3_backup */

typedef struct hbk {
    struct hbk *next;       /* next blob handle */
    sqlite3_backup *bkup;   /* SQLite3 backup handle */
    handle *h;              /* SQLite database handle (source) */
} hbk;
#endif

/* ISO to/from UTF-8 translation */

typedef struct {
    char *result;          /* translated C string result */
    char *tofree;          /* memory to be free'd, or 0 */
    String jstr;           /* resulting Java string or 0 */
} transstr;

static const char xdigits[] = "0123456789ABCDEF";
