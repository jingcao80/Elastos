
#include "Database.h"
#include "CTableResult.h"
#include "CVm.h"
#include "CBackup.h"
#include "CStmt.h"
#include "CBlob.h"
#include "CFunctionContext.h"
#include "StringUtils.h"
#include "AutoLock.h"
#include <elastos/utility/logging/Logger.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Core::StringUtils;
using Elastos::Sql::SQLite::CFunctionContext;
using Elastos::Sql::SQLite::CVm;
using Elastos::Sql::SQLite::CStmt;
using Elastos::Sql::SQLite::EIID_IDatabase;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_INTERFACE_IMPL(Database, Object, IDatabase)

Database::Database()
    : mHandle(0)
    , mError_code(0)
{
}

Database::~Database()
{
    Finalize();
}

static void Freep(char **strp)
{
    if (strp && *strp) {
        free(*strp);
        *strp = 0;
    }
}

static Int32 Callback(void *udata, int ncol, char **data, char **cols)
{
    handle *h = (handle *) udata;

    if (h->cb) {
    AutoPtr<ArrayOf<String> > arr;
    Int32 i = 0;
    if (h->row1) {
            arr = ArrayOf<String>::Alloc(ncol);
            for (i = 0; i < ncol; i++) {
                if (cols[i]) {
                    (*arr)[i] = String(cols[i]);
                }
            }
            h->row1 = 0;
            h->cb->Columns(arr);

#if HAVE_BOTH_SQLITE
        if (h->is3) {
        mid = (*env)->GetMethodID(env, cls, "types","([Ljava/lang/String;)V");

        if (mid && h->stmt) {
            arr = (*env)->NewObjectArray(env, ncol,
                         C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
            const char *ctype =
                sqlite3_column_decltype(h->stmt, i);

            if (!ctype) {
                switch (sqlite3_column_type(h->stmt, i)) {
                case SQLITE_INTEGER: ctype = "integer"; break;
                case SQLITE_FLOAT:   ctype = "double";  break;
                default:
#if defined(SQLITE_TEXT) && defined(SQLITE3_TEXT) && (SQLITE_TEXT != SQLITE3_TEXT)
                case SQLITE_TEXT:
#else
#ifdef SQLITE3_TEXT
                case SQLITE3_TEXT:
#endif
#endif
                         ctype = "text";    break;
                case SQLITE_BLOB:    ctype = "blob";    break;
                case SQLITE_NULL:    ctype = "null";    break;
                }
            }
            if (ctype) {
                transstr ty;

                trans2utf(env, 1, 0, ctype, &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                              ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
            }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
            (*env)->DeleteLocalRef(env, exc);
            return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
        }
        } else {
        if (h->ver >= 0x020506 && cols[ncol]) {
            mid = (*env)->GetMethodID(env, cls, "types",
                          "([Ljava/lang/String;)V");

            if (mid) {
            arr = (*env)->NewObjectArray(env, ncol,
                             C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
                if (cols[i + ncol]) {
                transstr ty;

                trans2utf(env, h->haveutf, h->enc,
                      cols[i + ncol], &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                                  ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                    (*env)->DeleteLocalRef(env, exc);
                    return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
                }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
            }
        }
        }
#else
#if HAVE_SQLITE2
        if (h->ver >= 0x020506 && cols[ncol]) {
        mid = (*env)->GetMethodID(env, cls, "types",
                      "([Ljava/lang/String;)V");

        if (mid) {
            arr = (*env)->NewObjectArray(env, ncol,
                         C_java_lang_String, 0);
            for (i = 0; i < ncol; i++) {
            if (cols[i + ncol]) {
                transstr ty;

                trans2utf(env, h->haveutf, h->enc,
                      cols[i + ncol], &ty);
                (*env)->SetObjectArrayElement(env, arr, i,
                              ty.jstr);
                exc = (*env)->ExceptionOccurred(env);
                if (exc) {
                (*env)->DeleteLocalRef(env, exc);
                return 1;
                }
                (*env)->DeleteLocalRef(env, ty.jstr);
            }
            }
            (*env)->CallVoidMethod(env, h->cb, mid, arr);
            exc = (*env)->ExceptionOccurred(env);
            if (exc) {
            (*env)->DeleteLocalRef(env, exc);
            return 1;
            }
            (*env)->DeleteLocalRef(env, arr);
        }
        }
#endif
#if HAVE_SQLITE3
        if (h->stmt) {
            arr = ArrayOf<String>::Alloc(ncol);
            for (i = 0; i < ncol; i++) {
                const char *ctype = sqlite3_column_decltype(h->stmt, i);

                if (!ctype) {
                    switch (sqlite3_column_type(h->stmt, i)) {
                        case SQLITE_INTEGER:
                            ctype = "integer";
                            break;
                        case SQLITE_FLOAT:
                            ctype = "double";
                            break;
                        default:
#if defined(SQLITE_TEXT) && defined(SQLITE3_TEXT) && (SQLITE_TEXT != SQLITE3_TEXT)
                        case SQLITE_TEXT:
#else
#ifdef SQLITE3_TEXT
                        case SQLITE3_TEXT:
#endif
#endif
                            ctype = "text";
                            break;
                        case SQLITE_BLOB:
                            ctype = "blob";
                            break;
                        case SQLITE_NULL:
                            ctype = "null";
                            break;
                    }
                }
                if (ctype) {
                    (*arr)[i] = String(ctype);
                }
            }
            h->cb->Types(arr);
        }
#endif
#endif
    }
    if (data) {
        Boolean rc = FALSE;
        arr = ArrayOf<String>::Alloc(ncol);
        for (i = 0; arr && i < ncol; i++) {
            if (data[i]) {
                (*arr)[i] = String(data[i]);
            }
        }
        h->cb->Newrow(arr,&rc);
        return rc != FALSE;
        }
    }
    return 0;
}

#if HAVE_SQLITE3
static Int32 Busyhandler3(void *udata, Int32 count)
{
    handle *h = (handle *) udata;
    Boolean ret = FALSE;

    if (h->bh) {
        h->bh->Busy(String(NULL),count,&ret);
        ret =  ret!= FALSE;
    }
    return ret;
}
#endif

ECode Database::Open(
    /* [in] */ const String& filename,
    /* [in] */ Int32 mode)
{
    if ((mode & 0200) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#else
        mode = IConstants::SQLITE_OPEN_READWRITE |
                IConstants::SQLITE_OPEN_CREATE;
#endif
    } else if ((mode & 0400) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READONLY;
#else
        mode = IConstants::SQLITE_OPEN_READONLY;
#endif
    }
    AutoLock lock(this);
    return _Open4(filename, mode, String(NULL), FALSE);
}

ECode Database::Open(
    /* [in] */ const String& filename,
    /* [in] */ Int32 mode,
    /* [in] */ const String& vfs)
{
    if ((mode & 0200) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#else
        mode = IConstants::SQLITE_OPEN_READWRITE |
                IConstants::SQLITE_OPEN_CREATE;
#endif
    } else if ((mode & 0400) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READONLY;
#else
        mode = IConstants::SQLITE_OPEN_READONLY;
#endif
    }
    AutoLock lock(this);
    return _Open4(filename, mode, vfs, FALSE);
}

ECode Database::Open(
    /* [in] */ const String& filename,
    /* [in] */ Int32 mode,
    /* [in] */ const String& vfs,
    /* [in] */ Boolean ver2)
{
    if ((mode & 0200) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
#else
        mode = IConstants::SQLITE_OPEN_READWRITE |
                IConstants::SQLITE_OPEN_CREATE;
#endif
    } else if ((mode & 0400) != 0) {
#if HAVE_SQLITE3
        mode = SQLITE_OPEN_READONLY;
#else
        mode = IConstants::SQLITE_OPEN_READONLY;
#endif
    }
    AutoLock lock(this);
    return _Open4(filename, mode, vfs, ver2);
}

ECode Database::OpenAuxFile(
    /* [in] */ const String& filename)
{
    AutoLock lock(this);
    return _OpenAuxFile(filename);
}

ECode Database::Finalize()
{
    AutoLock lock(this);
    return _Finalize();
}

ECode Database::Close()
{
    AutoLock lock(this);
    return _Close();
}

ECode Database::Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback* cb)
{
    AutoLock lock(this);
    return _Exec(sql, cb);
}

ECode Database::Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback* cb,
    /* [in] */ ArrayOf<String> * args)
{
    AutoLock lock(this);
    return _Exec(sql, cb, args);
}

ECode Database::LastInsertRowid(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    AutoLock lock(this);
    *id = _LastInsertRowid();
    return NOERROR;
}

ECode Database::Interrupt()
{
    AutoLock lock(this);
    return _Interrupt();
}

ECode Database::Changes(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value);
    AutoLock lock(this);
    *value = _Changes();
    return NOERROR;
}

ECode Database::BusyHandler(
    /* [in] */ IBusyHandler* bh)
{
    AutoLock lock(this);
    return _BusyHandler(bh);
}

ECode Database::BusyTimeout(
    /* [in] */ Int32 ms)
{
    AutoLock lock(this);
    return _BusyTimeout(ms);
}

ECode Database::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ Int32 maxrows,
    /* [out] */ ITableResult ** tableresult)
{
    VALIDATE_NOT_NULL(tableresult);
    *tableresult = NULL;

    AutoPtr<ITableResult> ret ;
    CTableResult::New(maxrows,(ITableResult**)&ret);
    Boolean is3 = FALSE;
    if (Is3(&is3), !is3) {
        ECode ec = Exec(sql, ICallback::Probe(ret));
        if (ec != NOERROR) {
            if (maxrows <= 0 || !((CTableResult *)ret.Get())->mAtMaxRows) {
                return NOERROR;
            }
        }
    } else {
        AutoLock lock(this);
        // /* only one statement !!! */
            AutoPtr<IVm> vm;
            Compile(sql, (IVm**)&vm);
            SetLastError(((CVm *)vm.Get())->mError_code);
            Boolean vmflag = FALSE;
            ((CVm *)vm.Get())->Step(ICallback::Probe(ret),&vmflag);
            if (((CTableResult *)ret.Get())->mMaxRows > 0) {
               while (((CTableResult *)ret.Get())->mNrows < ((CTableResult *)ret.Get())->mMaxRows
                        && vmflag) {
                    SetLastError(((CVm *)vm.Get())->mError_code);
                }
            } else {
                vmflag = FALSE;
                ((CVm *)vm.Get())->Step(ICallback::Probe(ret),&vmflag);
                while (vmflag) {
                    SetLastError(((CVm *)vm.Get())->mError_code);
                }
            }
            ((CVm *)vm.Get())->Finalize();
    }

    *tableresult = ret;
    REFCOUNT_ADD(*tableresult);
    return NOERROR;
}

ECode Database::GetTable(
    /* [in] */ const String& sql,
    /* [out] */ ITableResult ** tableresult)
{
    return GetTable(sql, 0, tableresult);
}

ECode Database::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ Int32 maxrows,
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ ITableResult ** tableresult)
{
    VALIDATE_NOT_NULL(tableresult);
    *tableresult = NULL;

    AutoPtr<ITableResult> ret ;
    CTableResult::New(maxrows,(ITableResult**)&ret);
    Boolean is3 = FALSE;
    if (Is3(&is3), !is3) {
        ECode ec = Exec(sql, ICallback::Probe(ret), args);
        if (ec != NOERROR) {
            if (maxrows <= 0 || !((CTableResult *)ret.Get())->mAtMaxRows) {
                return NOERROR;
            }
        }
    } else {
        AutoLock lock(this);
        // /* only one statement !!! */
        AutoPtr<IVm> vm;
        Compile(sql, args, (IVm**)&vm);
        SetLastError(((CVm *)vm.Get())->mError_code);
        Boolean vmflag = FALSE;
        if (((CTableResult *)ret.Get())->mMaxRows > 0) {
            ((CVm *)vm.Get())->Step(ICallback::Probe(ret),&vmflag);
            while (((CTableResult *)ret.Get())->mNrows < ((CTableResult *)ret.Get())->mMaxRows
                    && vmflag) {
                SetLastError(((CVm *)vm.Get())->mError_code);
            }
        } else {
            vmflag = FALSE;
            ((CVm *)vm.Get())->Step(ICallback::Probe(ret),&vmflag);
            while (vmflag) {
            SetLastError(((CVm *)vm.Get())->mError_code);
            }
        }
        ((CVm *)vm.Get())->Finalize();
    }

    *tableresult = ret;
    REFCOUNT_ADD(*tableresult);
    return NOERROR;
}

ECode Database::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ ITableResult ** tableresult)
{
    return GetTable(sql, 0, args, tableresult);
}

ECode Database::GetTable(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String> * args,
    /* [in] */ ITableResult* tbl)
{
    ((CTableResult* )tbl)->Clear();
    Boolean is3 = FALSE;
    if (Is3(&is3), !is3) {
        ECode ec = Exec(sql, ICallback::Probe(tbl), args);
        if (ec != NOERROR) {
            if (((CTableResult *)tbl)->mMaxRows <= 0 || !((CTableResult *)tbl)->mAtMaxRows) {
                return E_SQL_EXCEPTION;
            }
        }
    } else {
        AutoLock lock(this);
        /* only one statement !!! */
        AutoPtr<IVm> vm;
        Compile(sql, args, (IVm**)&vm);
        Boolean vmflag = FALSE;
        if (((CTableResult* )tbl)->mMaxRows > 0) {
            ((CVm *)vm.Get())->Step((ICallback *)tbl,&vmflag);
            while (((CTableResult* )tbl)->mNrows < ((CTableResult* )tbl)->mMaxRows
                        && vmflag) {
                SetLastError(((CVm *)vm.Get())->mError_code);
            }
        } else {
            vmflag = FALSE;
            ((CVm *)vm.Get())->Step((ICallback *)tbl,&vmflag);
            while (vmflag) {
                SetLastError(((CVm *)vm.Get())->mError_code);
            }
        }
        ((CVm *)vm.Get())->Finalize();
    }

    return NOERROR;
}

String Database::Version()
{
        /* CHECK THIS */
#if HAVE_BOTH_SQLITE
    return (*env)->NewStringUTF(env, sqlite_libversion());
#else
#if HAVE_SQLITE2
    return (*env)->NewStringUTF(env, sqlite_libversion());
#else
    return String(sqlite3_libversion());
#endif
#endif
}

ECode Database::Dbversion(
    /* [out] */ String * ver)
{
    VALIDATE_NOT_NULL(ver);
    handle *h = (handle *)mHandle;

     if (h && h->sqlite) {
 #if HAVE_BOTH_SQLITE
     Boolean isflag = FALSE;
     Is3(&isflag);
     if (isflag) {
         *ver = sqlite3_libversion();
         return NOERROR;
     } else {
         *ver = sqlite_libversion();
         return NOERROR;
     }
 #else
 #if HAVE_SQLITE2
    *ver = sqlite_libversion();
    return NOERROR;
 #else
    *ver = String(sqlite3_libversion());
    return NOERROR;
 #endif
 #endif
     }
    *ver = String("unknown");
    return NOERROR;
}

ECode Database::CreateFunction(
    /* [in] */ const String& name,
    /* [in] */ Int32 nargs,
    /* [in] */ IFunction* f)
{
    AutoLock lock(this);
    return _CreateFunction(name, nargs, f);
}

ECode Database::CreateAggregate(
    /* [in] */ const String& name,
    /* [in] */ Int32 nargs,
    /* [in] */ IFunction* f)
{
    AutoLock lock(this);
    return _CreateAggregate(name, nargs, f);
}

ECode Database::FunctionType(
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    AutoLock lock(this);
    return _FunctionType(name, type);
}

ECode Database::LastError(
    /* [out] */ Int32 * err)
{
    VALIDATE_NOT_NULL(err);
    *err = mError_code;
    return NOERROR;
}

ECode Database::SetLastError(
    /* [in] */ Int32 mError_code)
{
    mError_code = mError_code;
    return NOERROR;
}

String Database::ErrorString(Int32 mError_code)
{
#if HAVE_SQLITE2
    return (*env)->NewStringUTF(env, sqlite_error_string((int) err));
#else
    return String("unkown error");
#endif
}

ECode Database::ErrorMessage(
    /* [out] */ String * str)
{
    VALIDATE_NOT_NULL(str);
    {    AutoLock syncLock(this);
        *str = _Errmsg();
    }

    return NOERROR;
}

ECode Database::SetEncoding(
    /* [in] */ const String& enc)
{
    AutoLock lock(this);
    return _SetEncoding(enc);
}

ECode Database::SetAuthorizer(
    /* [in] */ IAuthorizer* auth)
{
    AutoLock lock(this);
    return _SetAuthorizer(auth);
}

ECode Database::Trace(
    /* [in] */ ITrace* tr)
{
    AutoLock lock(this);
    return _Trace(tr);
}

ECode Database::Backup(
    /* [in] */ IDatabase* dest,
    /* [in] */ const String& destName,
    /* [in] */ const String& srcName,
    /* [out] */ IBackup ** backup)
{
    VALIDATE_NOT_NULL(backup);
    {    AutoLock syncLock(this);
        AutoPtr<IBackup> b;
        CBackup::New((IBackup**)&b);
        _Backup(b, dest, destName, (IDatabase *)(this->Probe(EIID_IDatabase)), srcName);
        *backup = b;
        REFCOUNT_ADD(*backup);
    }

    return NOERROR;
}

ECode Database::Profile(
    /* [in] */ IProfile* r)
{
    ECode ec = NOERROR;
    {    AutoLock syncLock(this);
        ec = _Profile(r);
    }
    return ec;
}

ECode Database::DbStatus(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32> * info,
    /* [in] */ Boolean flag,
    /* [out] */ Int32 * status)
{
    VALIDATE_NOT_NULL(status);
    {    AutoLock syncLock(this);
        *status = _DbStatus(op, info, flag);
    }
    return NOERROR;
}

ECode Database::Compile(
    /* [in] */ const String& sql,
    /* [out] */ IVm ** ivm)
{
    VALIDATE_NOT_NULL(ivm);
    {    AutoLock syncLock(this);
        AutoPtr<IVm> vm;
        CVm::New((IVm**)&vm);
        Vm_compile(sql, vm);
        *ivm = vm;
        REFCOUNT_ADD(*ivm);
    }

    return NOERROR;
}

ECode Database::Compile(
    /* [in] */ const String& sql,
    /* [in] */ ArrayOf<String> * args,
    /* [out] */ IVm ** ivm)
{
    VALIDATE_NOT_NULL(ivm);
    {    AutoLock syncLock(this);
        AutoPtr<IVm> vm;
        CVm::New((IVm**)&vm);
        Vm_compile_args(sql, vm, args);
        *ivm = vm;
        REFCOUNT_ADD(*ivm);
    }
    return NOERROR;
}

ECode Database::Prepare(
    /* [in] */ const String& sql,
    /* [out] */ IStmt ** st)
{
    VALIDATE_NOT_NULL(st);

    {    AutoLock syncLock(this);
        AutoPtr<IStmt> stmt;
        CStmt::New((IStmt**)&stmt);
        FAIL_RETURN(Stmt_prepare(sql, stmt));
        *st = stmt;
        REFCOUNT_ADD(*st);
    }

    return NOERROR;
}

ECode Database::OpenBlob(
    /* [in] */ const String& db,
    /* [in] */ const String& table,
    /* [in] */ const String& column,
    /* [in] */ Int64 row,
    /* [in] */ Boolean rw,
    /* [out] */ IBlob ** blob)
{
    VALIDATE_NOT_NULL(blob);

    {    AutoLock syncLock(this);
        AutoPtr<IBlob> bl;
        CBlob::New((IBlob**)&bl);
        _OpenBlob(db, table, column, row, rw, bl);
        *blob = bl;
        REFCOUNT_ADD(*blob);
    }

    return NOERROR;
}

ECode Database::Is3(
    /* [out] */ Boolean * value)
{
    VALIDATE_NOT_NULL(value);
#if HAVE_BOTH_SQLITE
    handle *h = (handle *)mHandle);

    if (h) {
        *value = h->is3 ? TRUE : FALSE;
        return NOERROR;
    }
    else
    {
        *value = FALSE;
        return NOERROR;
    }
#else
#if HAVE_SQLITE2
    *value = FALSE;
    return NOERROR;
#endif
#if HAVE_SQLITE3
    *value = TRUE;
    return NOERROR;
#endif
#endif

    *value = FALSE;
    return NOERROR;
}

ECode Database::ProgressHandler(
    /* [in] */ Int32 n,
    /* [in] */ IProgressHandler* p)
{
    AutoLock lock(this);
    return _ProgressHandler(n, p);
}

ECode Database::Key(
    /* [in] */ ArrayOf<Byte> * ekey)
{
    AutoLock lock(this);
    return _Key(ekey);
}

ECode Database::Key(
    /* [in] */ const String& skey)
{
    AutoLock lock(this);
    AutoPtr<ArrayOf<Byte> > ekey;
    AutoPtr<ArrayOf<Char32> > charArray = skey.GetChars();
    Char32 c;
    if (charArray->GetLength() > 0) {
        ekey = ArrayOf<Byte>::Alloc(charArray->GetLength());
        for (Int32 i = 0; i< charArray->GetLength(); i++) {
            c = (*charArray)[i];
            (*ekey)[i] = (Byte) ((c & 0xff) ^ (c >> 8));
        }
    }
    return _Key(ekey);
}
ECode Database::_Open(
    /* [in] */ const String& filename,
    /* [in] */ Int32 mode)
{
    return _Open4(filename,mode,String(NULL),0);
}

ECode Database::_Open4(
    /* [in] */ const String& filename,
    /* [in] */ Int32 mode,
    /* [in] */ const String& vfs,
    /* [in] */ Boolean ver2)
{
    handle *h = (handle *)mHandle;
    String file(filename);
    Int32 maj = 0, min = 0, lev = 0;
#if HAVE_SQLITE3_OPEN_V2
    transstr vfsname;

    vfsname.result = 0;
    vfsname.tofree = 0;
    vfsname.jstr = 0;
#endif
    if (h) {
    if (h->sqlite) {
#if HAVE_BOTH_SQLITE
        if (h->is3) {
        sqlite3_close((sqlite3 *) h->sqlite);
        } else {
        sqlite_close((sqlite *) h->sqlite);
        }
        h->is3 = 0;
#else
#if HAVE_SQLITE2
        sqlite_close((sqlite *) h->sqlite);
#endif
#if HAVE_SQLITE3
        sqlite3_close((sqlite3 *) h->sqlite);
#endif
#endif
        h->sqlite = 0;
    }
    } else {
    h = (handle *)malloc(sizeof(handle));
    if (h == NULL) {
        return E_NULL_POINTER_EXCEPTION;
    }
    h->sqlite = NULL;
    h->bh = NULL;
    h->cb = NULL;
    h->ai = NULL;
    h->tr = NULL;
    h->pr = NULL;
    h->ph = NULL;
    /* CHECK THIS */
#if HAVE_BOTH_SQLITE
    h->is3 = 0;
    h->stmt = 0;
    h->haveutf = 1;
#else
#if HAVE_SQLITE2
    h->haveutf = strcmp(sqlite_libencoding(), "UTF-8") == 0;
#endif
#if HAVE_SQLITE3
    h->stmt = NULL;
    h->haveutf = 1;
#endif
#endif
    h->enc = NULL;
    h->funcs = NULL;
    h->ver = 0;
#if HAVE_SQLITE_COMPILE
    h->vms = 0;
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    h->blobs = 0;
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
    h->backups = 0;
#endif
    }
    if (file.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
#if HAVE_SQLITE3_OPEN_V2
    if (vfs) {
    trans2iso(1, (String)h->enc, vfs, &vfsname);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        transfree(&file);
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    }
#endif
#if HAVE_BOTH_SQLITE
    {
    FILE *f = fopen(file.result, "rb");
    int c_0 = EOF;

    if (f) {
        c_0 = fgetc(f);
        fclose(f);
    }
    if (c_0 != '*' && ver2 == JNI_FALSE) {
#if HAVE_SQLITE3_OPEN_V2
        int rc = sqlite3_open_v2(file.result, (sqlite3 **) &h->sqlite,
                     (int) mode, vfsname.result);
#else
        int rc = sqlite3_open(file.result, (sqlite3 **) &h->sqlite);
#endif
        if (rc == SQLITE_OK) {
        h->is3 = 1;
        } else if (h->sqlite) {
        sqlite3_close((sqlite3 *) h->sqlite);
        h->sqlite = 0;
        }
    } else {
        h->sqlite = (void *) sqlite_open(file.result,
                         (int) mode, &err);
    }
    }
#else
#if HAVE_SQLITE2
    h->sqlite = (void *) sqlite_open(file.result, (int) mode, &err);
#endif
#if HAVE_SQLITE3
#if HAVE_SQLITE3_OPEN_V2
    if (sqlite3_open_v2(file.result, (sqlite3 **) &h->sqlite,
            (int) mode, vfsname.result) != SQLITE_OK)
#else
    if (sqlite3_open(file.string(), (sqlite3 **) &h->sqlite) != SQLITE_OK)
#endif
    {
        if (h->sqlite) {
            sqlite3_close((sqlite3 *) h->sqlite);
            h->sqlite = 0;
        }
    }
#endif
#endif
    file = String(NULL);
#if HAVE_SQLITE3_OPEN_V2
    transfree(&vfsname);
#endif
    //exc = (*env)->ExceptionOccurred(env);
//     if (exc) {
//     (*env)->DeleteLocalRef(env, exc);
// #if HAVE_SQLITE2
//     if (err) {
//         sqlite_freemem(err);
//     }
// #endif
//     if (h->sqlite) {
// #if HAVE_BOTH_SQLITE
//         if (h->is3) {
//         sqlite3_close((sqlite3 *) h->sqlite);
//         h->is3 = 0;
//         } else {
//         sqlite_close((sqlite *) h->sqlite);
//         }
// #else
// #if HAVE_SQLITE2
//         sqlite_close((sqlite *) h->sqlite);
// #endif
// #if HAVE_SQLITE3
//         sqlite3_close((sqlite3 *) h->sqlite);
// #endif
// #endif
//     }
//     h->sqlite = 0;
//     return;
//     }
    if (h->sqlite) {
    mHandle = (Int64)h;
#if HAVE_SQLITE2
    if (err) {
        sqlite_freemem(err);
    }
#endif
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sscanf(sqlite3_libversion(), "%d.%d.%d", &maj, &min, &lev);
#if HAVE_SQLITE3_LOAD_EXTENSION
        sqlite3_enable_load_extension((sqlite3 *) h->sqlite, 1);
#endif
    } else {
        sscanf(sqlite_libversion(), "%d.%d.%d", &maj, &min, &lev);
    }
#else
#if HAVE_SQLITE2
    sscanf(sqlite_libversion(), "%d.%d.%d", &maj, &min, &lev);
#endif
#if HAVE_SQLITE3
    sscanf(sqlite3_libversion(), "%d.%d.%d", &maj, &min, &lev);
#if HAVE_SQLITE3_LOAD_EXTENSION
    sqlite3_enable_load_extension((sqlite3 *) h->sqlite, 1);
#endif
#endif
#endif
    h->ver = ((maj & 0xFF) << 16) | ((min & 0xFF) << 8) | (lev & 0xFF);
    return NOERROR;
    }
    //throwex(env, err ? err : "unknown error in open");
#if HAVE_SQLITE2
    if (err) {
    sqlite_freemem(err);
    }
#endif
    return NOERROR;
}

ECode Database::_OpenAuxFile(
    /* [in] */ const String& filename)
{
    handle *h = (handle *)mHandle;

#if HAVE_SQLITE_OPEN_AUX_FILE
    jthrowable exc;
    char *err = 0;
    Int32 ret = 0;
#endif

    if (h && h->sqlite) {
#if HAVE_SQLITE_OPEN_AUX_FILE
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        //throwex(env, "unsupported");
    }
#endif
    trans2iso(env, h->haveutf, h->enc, file, &filename);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    ret = sqlite_open_aux_file((sqlite *) h->sqlite,
                   filename.result, &err);
    transfree(&filename);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        if (err) {
        sqlite_freemem(err);
        }
        return;
    }
    if (ret != SQLITE_OK) {
        //throwex(env, err ? err : sqlite_error_string(ret));
    }
    if (err) {
        sqlite_freemem(err);
    }
    return NOERROR;
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    }

    return NOERROR;
}

ECode Database::doclose(
    /* [in] */ Int32 final)
{
    handle *h = (handle *)mHandle;
    if (h) {
    hfunc *f;
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    hbl *bl;
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
    hbk *bk;
#endif
#if HAVE_SQLITE_COMPILE
    hvm *v;

    while ((v = h->vms)) {
        h->vms = v->next;
        v->next = 0;
        v->h = 0;
        if (v->vm) {
#if HAVE_BOTH_SQLITE
        if (h->is3) {
            sqlite3_finalize((sqlite3_stmt *) v->vm);
        } else {
            sqlite_finalize((sqlite_vm *) v->vm, 0);
        }
#else
#if HAVE_SQLITE2
        sqlite_finalize((sqlite_vm *) v->vm, 0);
#endif
#if HAVE_SQLITE3
        sqlite3_finalize((sqlite3_stmt *) v->vm);
#endif
#endif
        v->vm = 0;
        }
    }
#endif
    if (h->sqlite) {
#if HAVE_BOTH_SQLITE
        if (h->is3) {
        sqlite3_close((sqlite3 *) h->sqlite);
        } else {
        sqlite_close((sqlite *) h->sqlite);
        }
#else
#if HAVE_SQLITE2
        sqlite_close((sqlite *) h->sqlite);
#endif
#if HAVE_SQLITE3
        sqlite3_close((sqlite3 *) h->sqlite);
#endif
#endif
        h->sqlite = 0;
    }
    while ( h->funcs != NULL) {
        f = h->funcs;
        h->funcs = f->next;
        f->h = 0;
        f->sf = 0;
        if (f->fc) {
            mHandle = (Int64)0;
        }
        free(f);
        f = NULL;
    }
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    while ((bl = h->blobs)) {
        h->blobs = bl->next;
        bl->next = 0;
        bl->h = 0;
        if (bl->blob) {
        sqlite3_blob_close(bl->blob);
        }
        bl->blob = 0;
    }
#endif
#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
    while ((bk = h->backups)) {
        h->backups = bk->next;
        bk->next = 0;
        bk->h = 0;
        if (bk->bkup) {
        sqlite3_backup_finish(bk->bkup);
        }
        bk->bkup = 0;
    }
#endif
    free(h);
    h = NULL;
    mHandle = (Int64)0;
    return NOERROR;
    }
    if (!final) {
    //throwclosed(env);
    }
    return NOERROR;
}

Boolean Database::Complete(
    /* [in] */ const String& sql)
{
    return _Complete(sql);
}

ECode Database::_Finalize()
{
    return doclose(1);
}

ECode Database::_Close()
{
    return doclose(0);
}

ECode Database::_Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback* cb)
{
    handle *h = (handle *)mHandle;
    freemem *freeproc;

    if (sql.IsNull()) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (h) {
    if (h->sqlite) {
        Int32 rc = 0;
        char *err = NULL;
        AutoPtr<ICallback> oldcb = h->cb;
        h->cb = cb;
        h->row1 = 1;
        String sqlstr(sql);
#if HAVE_BOTH_SQLITE
        if (h->is3) {
        rc = sqlite3_exec((sqlite3 *) h->sqlite, sqlstr.result,
                  callback, h, &err);
        freeproc = (freemem *) sqlite3_free;
        } else {
        rc = sqlite_exec((sqlite *) h->sqlite, sqlstr.result,
                 callback, h, &err);
        freeproc = (freemem *) sqlite_freemem;
        }
#else
#if HAVE_SQLITE2
        rc = sqlite_exec((sqlite *) h->sqlite, sqlstr.result,
                 callback, h, &err);
        freeproc = (freemem *) sqlite_freemem;
#endif
#if HAVE_SQLITE3
        rc = sqlite3_exec((sqlite3 *) h->sqlite, sqlstr.string(),
                  Callback, h, &err);
        freeproc = (freemem *) sqlite3_free;

#endif
#endif
        h->cb = oldcb;
        if (rc != SQLITE_OK) {
            char msg[128];

            mError_code = rc;
            if (!err) {
                snprintf(msg, 128, "error %d in sqlite*_exec", rc);
            }
            return E_SQL_EXCEPTION;
        }
        if (err) {
            freeproc(err);
        }
        return NOERROR;
    }
    }

    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_Exec(
    /* [in] */ const String& sql,
    /* [in] */ ICallback* cb,
    /* [in] */ ArrayOf<String> * argsstr)
{
    handle *h = (handle *)mHandle;
    freemem *freeproc = 0;
    if (!sql) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (h) {
    if (h->sqlite) {
        Int32 rc = SQLITE_ERROR, nargs, i = 0;
        char *err = NULL, *p = NULL;
        const char *str = sql.string();
        String sqlstr;
        struct args {
            char *arg;
            String obj;
            String trans;
        } *argv = 0;
        char **cargv = 0;
        AutoPtr<ICallback> oldcb = h->cb;

        h->cb = cb;
        p = (char *) str;
        nargs = 0;
        while (*p) {
        if (*p == '%') {
            ++p;
            if (*p == 'q' || *p == 's') {
            nargs++;
            if (nargs > MAX_PARAMS) {
                str = NULL;
                h->cb = oldcb;
                // throwex(env, "too much SQL parameters");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            } else if (h->ver >= 0x020500 && *p == 'Q') {
            nargs++;
            if (nargs > MAX_PARAMS) {
                str = NULL;
                h->cb = oldcb;
                // throwex(env, "too much SQL parameters");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            } else if (*p != '%') {
            str = NULL;
            h->cb = oldcb;
            // throwex(env, "bad % specification in query");
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        }
        ++p;
        }
        cargv = (char ** )malloc((sizeof (*argv) + sizeof (char *)) * MAX_PARAMS);
        if (!cargv) {
            str = NULL;
            h->cb = oldcb;
            // throwoom(env, "unable to allocate arg vector");
            return E_NULL_POINTER_EXCEPTION;
        }
        memset(cargv, 0, (sizeof (*argv) + sizeof (char *)) * MAX_PARAMS);
        argv = (struct args *) (cargv + MAX_PARAMS);
        for (i = 0; i < MAX_PARAMS; i++) {
            cargv[i] = 0;
            argv[i].arg = 0;
            argv[i].obj = 0;
            argv[i].trans = 0;
        }

        if (argsstr) {
            for (i = 0; i < nargs; i++) {
                if (i < argsstr->GetLength()) {
                    argv[i].obj = (*argsstr)[i];
                    argv[i].arg = cargv[i] = const_cast<char *>((*argsstr)[i].string());
                }
            }
        }
        // if (exc) {
        // for (i = 0; i < nargs; i++) {
        //     if (argv[i].obj) {
        //     //transfree(&argv[i].trans);
        //     }
        // }
        // freep((char **) &cargv);
        // (*env)->ReleaseStringUTFChars(env, sql, str);
        // delglobrefp(env, &h->cb);
        // h->cb = oldcb;
        // return;
        // }
        h->row1 = 1;
        sqlstr = sql;
#if HAVE_BOTH_SQLITE
        if (h->is3) {
#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
            char *s = sqlite3_vmprintf(sqlstr.result, (char *) cargv);
#else
            char *s = sqlite3_mprintf(sqlstr.result,
                          cargv[0], cargv[1],
                          cargv[2], cargv[3],
                          cargv[4], cargv[5],
                          cargv[6], cargv[7],
                          cargv[8], cargv[9],
                          cargv[10], cargv[11],
                          cargv[12], cargv[13],
                          cargv[14], cargv[15],
                          cargv[16], cargv[17],
                          cargv[18], cargv[19],
                          cargv[20], cargv[21],
                          cargv[22], cargv[23],
                          cargv[24], cargv[25],
                          cargv[26], cargv[27],
                          cargv[28], cargv[29],
                          cargv[30], cargv[31]);
#endif

            if (s) {
            rc = sqlite3_exec((sqlite3 *) h->sqlite, s, callback,
                      h, &err);
            sqlite3_free(s);
            } else {
            rc = SQLITE_NOMEM;
            }
            freeproc = (freemem *) sqlite3_free;
        } else {
#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
            rc = sqlite_exec_vprintf((sqlite *) h->sqlite,
                         sqlstr.result, callback, h, &err,
                         (char *) cargv);
#else
            rc = sqlite_exec_printf((sqlite *) h->sqlite,
                        sqlstr.result, callback,
                        h, &err,
                        cargv[0], cargv[1],
                        cargv[2], cargv[3],
                        cargv[4], cargv[5],
                        cargv[6], cargv[7],
                        cargv[8], cargv[9],
                        cargv[10], cargv[11],
                        cargv[12], cargv[13],
                        cargv[14], cargv[15],
                        cargv[16], cargv[17],
                        cargv[18], cargv[19],
                        cargv[20], cargv[21],
                        cargv[22], cargv[23],
                        cargv[24], cargv[25],
                        cargv[26], cargv[27],
                        cargv[28], cargv[29],
                        cargv[30], cargv[31]);
#endif
            freeproc = (freemem *) sqlite_freemem;
        }
#else
#if HAVE_SQLITE2
#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
        rc = sqlite_exec_vprintf((sqlite *) h->sqlite, sqlstr.result,
                     callback, h, &err, (char *) cargv);
#else
        rc = sqlite_exec_printf((sqlite *) h->sqlite, sqlstr.result,
                    callback, h, &err,
                    cargv[0], cargv[1],
                    cargv[2], cargv[3],
                    cargv[4], cargv[5],
                    cargv[6], cargv[7],
                    cargv[8], cargv[9],
                    cargv[10], cargv[11],
                    cargv[12], cargv[13],
                    cargv[14], cargv[15],
                    cargv[16], cargv[17],
                    cargv[18], cargv[19],
                    cargv[20], cargv[21],
                    cargv[22], cargv[23],
                    cargv[24], cargv[25],
                    cargv[26], cargv[27],
                    cargv[28], cargv[29],
                    cargv[30], cargv[31]);
#endif
        freeproc = (freemem *) sqlite_freemem;
#endif
#if HAVE_SQLITE3
#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
        char *s = sqlite3_vmprintf(sqlstr.result, (char *) cargv);
#else
        char *s = sqlite3_mprintf(sqlstr.string(),
                      cargv[0], cargv[1],
                      cargv[2], cargv[3],
                      cargv[4], cargv[5],
                      cargv[6], cargv[7],
                      cargv[8], cargv[9],
                      cargv[10], cargv[11],
                      cargv[12], cargv[13],
                      cargv[14], cargv[15],
                      cargv[16], cargv[17],
                      cargv[18], cargv[19],
                      cargv[20], cargv[21],
                      cargv[22], cargv[23],
                      cargv[24], cargv[25],
                      cargv[26], cargv[27],
                      cargv[28], cargv[29],
                      cargv[30], cargv[31]);
#endif
        if (s) {
            rc = sqlite3_exec((sqlite3 *) h->sqlite, s, Callback,
                      h, &err);
            sqlite3_free(s);
        } else {
            rc = SQLITE_NOMEM;
        }
        freeproc = (freemem *) sqlite3_free;
#endif
#endif
        for (i = 0; i < nargs; i++) {
            if (argv[i].obj) {
                argv[i].trans = String(NULL);
            }
        }
        sqlstr = String(NULL);
        Freep(cargv);
        h->cb = oldcb;
        if (rc != SQLITE_OK) {
            char msg[128];
            if (!err) {
                snprintf(msg, 128, "error %d in sqlite*_exec", rc);
            }
            return E_SQL_EXCEPTION;
        }
        if (err && freeproc) {
        free(err);
        }
        return NOERROR;
    }
    }
    //throwclosed(env);
    return E_NULL_POINTER_EXCEPTION;
}

Int64 Database::_LastInsertRowid()
{
    handle *h = (handle*)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        return (Int64) sqlite3_last_insert_rowid((sqlite3 *) h->sqlite);
    } else {
        return (Int64) sqlite_last_insert_rowid((sqlite *) h->sqlite);
    }
#else
#if HAVE_SQLITE2
    return (Int64) sqlite_last_insert_rowid((sqlite *) h->sqlite);
#endif
#if HAVE_SQLITE3
    return (Int64) sqlite3_last_insert_rowid((sqlite3 *) h->sqlite);
#endif
#endif
    }
    return (Int64)0;
}

ECode Database::_Interrupt()
{
    handle *h = (handle*)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_interrupt((sqlite3 *) h->sqlite);
    } else {
        sqlite_interrupt((sqlite *) h->sqlite);
    }
#else
#if HAVE_SQLITE2
    sqlite_interrupt((sqlite *) h->sqlite);
#endif
#if HAVE_SQLITE3
    sqlite3_interrupt((sqlite3 *) h->sqlite);
#endif
#endif
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

Int64 Database::_Changes()
{
    handle *h = (handle*)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        return (Int64) sqlite3_changes((sqlite3 *) h->sqlite);
    } else {
        return (Int64) sqlite_changes((sqlite *) h->sqlite);
    }
#else
#if HAVE_SQLITE2
    return (Int64) sqlite_changes((sqlite *) h->sqlite);
#endif
#if HAVE_SQLITE3
    return (Int64) sqlite3_changes((sqlite3 *) h->sqlite);
#endif
#endif
    }
//     throwclosed(env);

    return (Int64)0;
}

ECode Database::_BusyHandler(
    /* [in] */ IBusyHandler* bh)
{
    handle *h = (handle*)mHandle;

    if (h && h->sqlite) {
    h->bh = bh;
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_busy_handler((sqlite3 *) h->sqlite, busyhandler3, h);
    } else {
        sqlite_busy_handler((sqlite *) h->sqlite, busyhandler, h);
    }
#else
#if HAVE_SQLITE2
    sqlite_busy_handler((sqlite *) h->sqlite, busyhandler, h);
#endif
#if HAVE_SQLITE3
    sqlite3_busy_handler((sqlite3 *) h->sqlite, Busyhandler3, h);
#endif
#endif
    return NOERROR;
    }
//     throwclosed(env);
    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_BusyTimeout(
    /* [in] */ Int32 ms)
{
    handle *h = (handle*)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_busy_timeout((sqlite3 * ) h->sqlite, ms);
    } else {
        sqlite_busy_timeout((sqlite *) h->sqlite, ms);
    }
#else
#if HAVE_SQLITE2
    sqlite_busy_timeout((sqlite *) h->sqlite, ms);
#endif
#if HAVE_SQLITE3
    sqlite3_busy_timeout((sqlite3 * ) h->sqlite, ms);
#endif
#endif
    return NOERROR;
    }
//     throwclosed(env);
    return E_NULL_POINTER_EXCEPTION;
}

Boolean Database::_Complete(
    /* [in] */ const String& sql)
{
    String sqlstr;
    Boolean result = FALSE;

    if (!sql) {
    return FALSE;
    }
#if HAVE_BOTH_SQLITE || HAVE_SQLITE3
    /* CHECK THIS */
    sqlstr = sql;
    result = sqlite3_complete(sqlstr.string()) ? TRUE : FALSE;
#else
    trans2iso(env, strcmp(sqlite_libencoding(), "UTF-8") == 0, 0, sql, &sqlstr);
    result = sqlite_complete(sqlstr.result) ? TRUE : FALSE;
#endif
    return result;
}

#if HAVE_SQLITE3
static void Call3_common(sqlite3_context *sf, Int32 isstep, Int32 nargs, sqlite3_value **args)
{
    hfunc *f = (hfunc *) sqlite3_user_data(sf);
    if (f && f->fi) {
        AutoPtr<ArrayOf<String> > arr = ArrayOf<String>::Alloc(nargs);
        Int32 i = 0;

        for (i = 0; i < nargs; i++) {
            if (args[i]) {
                (*arr)[i] = String((char *) sqlite3_value_text(args[i]));
            }
        }
        f->sf = sf;
        if (isstep)
        {
            f->fi->Step(f->fc,*arr);
        } else {
            f->fi->Function(f->fc,*arr);
        }

    }
}

static void Call3_func(sqlite3_context *sf, Int32 nargs, sqlite3_value **args)
{
    Call3_common(sf, 0, nargs, args);
}

static void Call3_step(sqlite3_context *sf, int nargs, sqlite3_value **args)
{
    Call3_common(sf, 1, nargs, args);
}

static void Call3_final(sqlite3_context *sf)
{
    hfunc *f = (hfunc *) sqlite3_user_data(sf);
    if (f && f->fi) {
        f->sf = sf;
        f->fi->LastStep(f->fc);
    }
}
#endif

static ECode Mkfunc_common(handle * ihandle,AutoPtr<IDatabase> obj,Int32 isagg, String name, Int32 nargs, AutoPtr<IFunction> fi)
{
    handle *h = ihandle;
    if (h && h->sqlite) {
    hfunc *f;
    Int32 ret = 0;
    AutoPtr<IFunctionContext> fc;
    CFunctionContext::New((IFunctionContext**)&fc);
    if (!fi) {
        return E_NULL_POINTER_EXCEPTION;
    }
    f = (hfunc *)malloc(sizeof (hfunc));
    if (!f) {
        return E_NULL_POINTER_EXCEPTION;
    }
    f->fc = fc;
    f->fi = fi;
    f->db = obj;
    REFCOUNT_ADD(f->fc);
    REFCOUNT_ADD(f->fi);
    REFCOUNT_ADD(f->db);
    f->h = h;
    f->next = h->funcs;
    h->funcs = f;
    f->sf = 0;
    ((CFunctionContext *)f->fc)->mHandle = (Int64)f;
#if HAVE_BOTH_SQLITE
    f->is3 = h->is3;
    if (h->is3) {
        ret = sqlite3_create_function((sqlite3 *) h->sqlite,
                      namestr.result,
                      (int) nargs,
                      SQLITE_UTF8, f,
                      isagg ? NULL : call3_func,
                      isagg ? call3_step : NULL,
                      isagg ? call3_final : NULL);

    } else {
        if (isagg) {
        ret = sqlite_create_aggregate((sqlite *) h->sqlite,
                          namestr.result,
                          (int) nargs,
                          call_step, call_final, f);
        } else {
        ret = sqlite_create_function((sqlite *) h->sqlite,
                         namestr.result,
                         (int) nargs,
                         call_func, f);
        }
    }
#else
#if HAVE_SQLITE2
    if (isagg) {
        ret = sqlite_create_aggregate((sqlite *) h->sqlite, namestr.result,
                      (int) nargs,
                      call_step, call_final, f);
    } else {
        ret = sqlite_create_function((sqlite *) h->sqlite, namestr.result,
                     (int) nargs,
                     call_func, f);
    }
#endif
#if HAVE_SQLITE3
    ret = sqlite3_create_function((sqlite3 *) h->sqlite,
                      name.string(),
                      (int) nargs,
                      SQLITE_UTF8, f,
                      isagg ? NULL : Call3_func,
                      isagg ? Call3_step : NULL,
                      isagg ? Call3_final : NULL);
#endif
#endif
    if (ret != SQLITE_OK) {
        PRINT_FILE_LINE_EX("sqlite3_create_function/aggregate error:%p \n",ret)
        return E_SQL_EXCEPTION;
    }
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_CreateFunction(
    /* [in] */ const String& name,
    /* [in] */ Int32 nargs,
    /* [in] */ IFunction* f)
{
    return Mkfunc_common((handle *)mHandle,(IDatabase *)(this->Probe(EIID_IDatabase)), 0, name, nargs, f);
}

ECode Database::_CreateAggregate(
    /* [in] */ const String& name,
    /* [in] */ Int32 nargs,
    /* [in] */ IFunction* f)
{
    return Mkfunc_common((handle *)mHandle,(IDatabase *)(this->Probe(EIID_IDatabase)), 1, name, nargs, f);
}

ECode Database::_FunctionType(
    /* [in] */ const String& name,
    /* [in] */ Int32 type)
{
    handle *h = (handle *)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        return;
    }
#endif
#if HAVE_SQLITE2
#if HAVE_SQLITE_FUNCTION_TYPE
    {
        int ret;
        transstr namestr;
        jthrowable exc;

        trans2iso(env, h->haveutf, h->enc, name, &namestr);
        exc = (*env)->ExceptionOccurred(env);
        if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
        }
        ret = sqlite_function_type(h->sqlite, namestr.result, (int) type);
        transfree(&namestr);
        if (ret != SQLITE_OK) {
        throwex(env, sqlite_error_string(ret));
        }
    }
#endif
#endif
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

String Database::_Errmsg()
{
#if HAVE_SQLITE3
    handle *h = (handle *)mHandle;

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (!h->is3) {
        return 0;
    }
#endif
    return String(sqlite3_errmsg((sqlite3 *) h->sqlite));
    }
#endif

    return String(NULL);
}

ECode Database::_SetEncoding(
    /* [in] */ const String& enc)
{
    handle *h = (handle *)mHandle;

    if (h && !h->haveutf) {
#if HAVE_BOTH_SQLITE
    if (!h->is3) {
        delglobrefp(env, &h->enc);
        h->enc = enc;
        globrefset(env, enc, &h->enc);
    }
#else
#if HAVE_SQLITE2
    delglobrefp(env, &h->enc);
    h->enc = enc;
    globrefset(env, enc, &h->enc);
#endif
#endif
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_SetAuthorizer(
    /* [in] */ IAuthorizer* auth)
{
    handle *h = (handle *)mHandle;

    if (h && h->sqlite) {
        h->ai = auth;
#if HAVE_SQLITE_SET_AUTHORIZER
    h->env = env;
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_set_authorizer((sqlite3 *) h->sqlite,
                   h->ai ? doauth : 0, h);
    } else {
        sqlite_set_authorizer((sqlite *) h->sqlite,
                  h->ai ? doauth : 0, h);
    }
#else
#if HAVE_SQLITE2
    sqlite_set_authorizer((sqlite *) h->sqlite, h->ai ? doauth : 0, h);
#endif
#if HAVE_SQLITE3
    sqlite3_set_authorizer((sqlite3 *) h->sqlite, h->ai ? doauth : 0, h);
#endif
#endif
#endif
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

#if HAVE_SQLITE_TRACE
static void Dotrace(void *arg, const char *msg)
{
    handle *h = (handle *) arg;

    if (h->tr && msg) {
        String str(msg);
        h->tr->Trace(str);
    }
    return;
}
#endif

ECode Database::_Trace(
    /* [in] */ ITrace* tr)
{
    handle *h = (handle *)mHandle;

    if (h && h->sqlite) {
        h->tr = tr;
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_trace((sqlite3 *) h->sqlite, h->tr ? dotrace : 0, h);
    } else {
#if HAVE_SQLITE_TRACE
        sqlite_trace((sqlite *) h->sqlite, h->tr ? dotrace : 0, h);
#endif
    }
#else
#if HAVE_SQLITE2
#if HAVE_SQLITE_TRACE
    sqlite_trace((sqlite *) h->sqlite, h->tr ? dotrace : 0, h);
#endif
#endif
#if HAVE_SQLITE3
    sqlite3_trace((sqlite3 *) h->sqlite, h->tr ? Dotrace : 0, h);
#endif
#endif
    return NOERROR;
    }
    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_Backup(
    /* [in] */ IBackup* b,
    /* [in] */ IDatabase* dest,
    /* [in] */ const String& destName,
    /* [in] */ IDatabase* src,
    /* [in] */ const String& srcName)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_BACKUPAPI
    handle *hsrc = gethandle(env, src);
    handle *hdest = gethandle(env, dest);
    hbk *bk;
    jthrowable exc;
    transstr dbns, dbnd;
    sqlite3_backup *bkup;
    jvalue vv;

    if (!bkupj) {
    throwex(env, "null backup");
    return;
    }
    if (!hsrc) {
    throwex(env, "no source database");
    return;
    }
    if (!hdest) {
    throwex(env, "no destination database");
    return;
    }
#if HAVE_BOTH_SQLITE
    if (!hsrc->is3 || !hdest->is3) {
    throwex(env, "not a SQLite 3 database");
    return;
    }
#endif
    if (!hsrc->sqlite) {
    throwex(env, "source database not open");
    return;
    }
    if (!hdest->sqlite) {
    throwex(env, "destination database not open");
    return;
    }
    trans2iso(env, hdest->haveutf, hdest->enc, destName, &dbnd);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
    (*env)->DeleteLocalRef(env, exc);
    return;
    }
    trans2iso(env, hsrc->haveutf, hsrc->enc, srcName, &dbns);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
    transfree(&dbnd);
    (*env)->DeleteLocalRef(env, exc);
    return;
    }
    bkup = sqlite3_backup_init((sqlite3 *) hdest->sqlite, dbnd.result,
                   (sqlite3 *) hsrc->sqlite, dbns.result);
    transfree(&dbnd);
    transfree(&dbns);
    if (!bkup) {
    const char *err = sqlite3_errmsg((sqlite3 *) hdest->sqlite);

    seterr(env, src, sqlite3_errcode((sqlite3 *) hdest->sqlite));
    throwex(env, err ? err : "error in backup init");
    return;
    }
    bk = malloc(sizeof (hbk));
    if (!bk) {
    sqlite3_backup_finish(bkup);
    throwoom(env, "unable to get SQLite backup handle");
    return;
    }
    bk->next = hsrc->backups;
    hsrc->backups = bk;
    bk->bkup = bkup;
    bk->h = hsrc;
    vv.j = 0;
    vv.l = (jobject) bk;
    (*env)->SetLongField(env, bkupj, F_SQLite_Backup_handle, vv.j);
    return;
#else
    // throwex(env, "unsupported");
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

#if HAVE_SQLITE3_PROFILE
static void Doprofile(void *arg, const char *msg, sqlite_uint64 est)
{
    handle *h = (handle *) arg;
    JNIEnv *env = h->env;

    if (env && h->pr && msg) {
    jthrowable exc;
    jclass cls = (*env)->GetObjectClass(env, h->pr);
    jmethodID mid;

    mid = (*env)->GetMethodID(env, cls, "profile",
                  "(Ljava/lang/String;J)V");
    if (mid) {
        transstr tr;
#if _MSC_VER && (_MSC_VER < 1300)
        jlong ms = est / (3600i64 * 24i64 * 1000i64);
#else
        jlong ms = est / (3600LL * 24LL * 1000LL);
#endif

        trans2utf(env, h->haveutf, h->enc, msg, &tr);
        exc = (*env)->ExceptionOccurred(env);
        if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        (*env)->ExceptionClear(env);
        return;
        }
        (*env)->CallVoidMethod(env, h->pr, mid, tr.jstr, ms);
        (*env)->ExceptionClear(env);
        (*env)->DeleteLocalRef(env, tr.jstr);
        return;
    }
    }
    return;
}
#endif

ECode Database::_Profile(
    /* [in] */ IProfile* pr)
{
#if HAVE_SQLITE3_PROFILE
    handle *h = gethandle(env, obj);

    if (h && h->sqlite) {
    delglobrefp(env, &h->pr);
    globrefset(env, tr, &h->pr);
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_profile((sqlite3 *) h->sqlite, h->pr ? doprofile : 0, h);
    }
#else
#if HAVE_SQLITE3
    sqlite3_profile((sqlite3 *) h->sqlite, h->pr ? doprofile : 0, h);
#endif
#endif
    }
#endif
    return NOERROR;
}

Int32 Database::_Status(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32>* info,
    /* [in] */ Boolean flag)
{
    Int32 ret = SQLITE_ERROR;
#if HAVE_SQLITE3_STATUS
    Int32 data[2] = { 0, 0 };
    AutoPtr<ArrayOf<Int32> > jdata = ArrayOf<Int32>::Alloc(2);
#if HAVE_SQLITE3
    ret = sqlite3_status(op, &data[0], &data[2], flag);
    if (ret == SQLITE_OK) {
        (*jdata)[0] = data[0];
        (*jdata)[1] = data[1];
        info = jdata;
        REFCOUNT_ADD(info);
    }
#endif
#endif
    return ret;
}

Int32 Database::_DbStatus(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32> * info,
    /* [in] */ Boolean flag)
{
    Int32 ret = SQLITE_ERROR;
#if HAVE_SQLITE3_DB_STATUS
    handle *h = (handle *)mHandle;
    Int32 data[2] = { 0, 0 };
    AutoPtr<ArrayOf<Int32> > jdata = ArrayOf<Int32>::Alloc(2);

    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        ret = sqlite3_db_status((sqlite3 *) h->sqlite, op, &data[0],
                    &data[1], flag);
    }
#else
#if HAVE_SQLITE3
    ret = sqlite3_db_status((sqlite3 *) h->sqlite, op, &data[0],
                &data[2], flag);
#endif
#endif
    if (ret == SQLITE_OK) {
        (*jdata)[0] = data[0];
        (*jdata)[1] = data[1];
        info = jdata;
    }
    }
#endif
    return ret;
}

ECode Database::Vm_compile(
    /* [in] */ const String& sql,
    /* [in] */ IVm* vm)
{
#if HAVE_SQLITE_COMPILE
    handle *h = (handle *)mHandle;
    void *svm = NULL;
    hvm *v;
    char *err = NULL;
#if HAVE_SQLITE2
    char *errfr = NULL;
#endif
    const char *tail = NULL;
    Int32 ret = 0;

    if (!h) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!vm) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!sql) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
#if HAVE_BOTH_SQLITE
    if (h->is3) {
#if HAVE_SQLITE3_PREPARE_V2
    ret = sqlite3_prepare_v2((sqlite3 *) h->sqlite, tr.result, -1,
                 (sqlite3_stmt **) &svm, &tail);
#else
    ret = sqlite3_prepare((sqlite3 *) h->sqlite, tr.result, -1,
                  (sqlite3_stmt **) &svm, &tail);
#endif
    if (ret != SQLITE_OK) {
        if (svm) {
        sqlite3_finalize((sqlite3_stmt *) svm);
        svm = 0;
        }
        err = (char *) sqlite3_errmsg((sqlite3 *) h->sqlite);
    }
    } else {
    ret = sqlite_compile((sqlite *) h->sqlite, tr.result, &tail,
                 (sqlite_vm **) &svm, &errfr);
    if (ret != SQLITE_OK) {
        err = errfr;
        if (svm) {
        sqlite_finalize((sqlite_vm *) svm, 0);
        }
    }
    }
#else
#if HAVE_SQLITE2
    ret = sqlite_compile((sqlite *) h->sqlite, tr.result, &tail,
             (sqlite_vm **) &svm, &errfr);
    if (ret != SQLITE_OK) {
    err = errfr;
    if (svm) {
        sqlite_finalize((sqlite_vm *) svm, 0);
        svm = 0;
    }
    }
#endif
#if HAVE_SQLITE3
#if HAVE_SQLITE3_PREPARE_V2
    ret = sqlite3_prepare_v2((sqlite3 *) h->sqlite, tr.result, -1,
                 (sqlite3_stmt **) &svm, &tail);
#else
    ret = sqlite3_prepare((sqlite3 *) h->sqlite, sql.string(), -1,
              (sqlite3_stmt **) &svm, &tail);
#endif
    if (ret != SQLITE_OK) {
    if (svm) {
        sqlite3_finalize((sqlite3_stmt *) svm);
        svm = 0;
    }
    err = (char *) sqlite3_errmsg((sqlite3 *) h->sqlite);
    }
#endif
#endif
    if (ret != SQLITE_OK) {
        Logger::E("Database", err);
#if HAVE_SQLITE2
    if (errfr) {
        sqlite_freemem(errfr);
    }
#endif
    return E_SQL_EXCEPTION;
    }
#if HAVE_SQLITE2
    if (errfr) {
    sqlite_freemem(errfr);
    }
#endif
    if (!svm) {
    return E_NULL_POINTER_EXCEPTION;
    }
    v = (hvm*)malloc(sizeof (hvm) + strlen(tail) + 1);
    if (!v) {
#if HAVE_BOTH_SQLITE
    if (h->is3) {
        sqlite3_finalize((sqlite3_stmt *) svm);
    } else {
        sqlite_finalize((sqlite_vm *) svm, 0);
    }
#else
#if HAVE_SQLITE2
    sqlite_finalize((sqlite_vm *) svm, 0);
#endif
#if HAVE_SQLITE3
    sqlite3_finalize((sqlite3_stmt *) svm);
#endif
#endif
    return E_NULL_POINTER_EXCEPTION;
    }
    v->next = h->vms;
    h->vms = v;
    v->vm = svm;
    v->h = h;
    v->tail = (char *) (v + 1);
#if HAVE_BOTH_SQLITE
    v->is3 = v->hh.is3 = h->is3;
#endif
    strcpy(v->tail, tail);
    v->hh.sqlite = 0;
    v->hh.haveutf = h->haveutf;
    v->hh.ver = h->ver;
    v->hh.bh = 0;
    v->hh.cb = 0;
    v->hh.ai = 0;
    v->hh.tr = 0;
    v->hh.ph = 0;
    v->hh.row1 = 1;
    v->hh.enc = h->enc;
    v->hh.funcs = 0;
    v->hh.vms = 0;
    ((CVm *)vm)->mHandle = (Int64 )v;
#else
    throwex(env, "unsupported");
#endif
    return NOERROR;
}

ECode Database::Vm_compile_args(
    /* [in] */ const String& sql,
    /* [in] */ IVm* vm,
    /* [in] */ ArrayOf<String>* argsstr)
{
#if HAVE_SQLITE_COMPILE
#if HAVE_SQLITE3
    handle *h = (handle *)mHandle;
#endif

#if HAVE_BOTH_SQLITE
    if (h && !h->is3) {
    throwex(env, "unsupported");
    return;
    }
#else
#if HAVE_SQLITE2
    throwex(env, "unsupported");
#endif
#endif
#if HAVE_SQLITE3
    if (!h || !h->sqlite) {
    return E_NULL_POINTER_EXCEPTION;
    }
    if (!vm) {
    return E_NULL_POINTER_EXCEPTION;
    }
    if (sql.IsNull()) {
    return E_ILLEGAL_ARGUMENT_EXCEPTION;
    } else {
    void *svm = 0;
    hvm *v;
    Int32 rc = SQLITE_ERROR, nargs = 0, i = 0;
    char *p;
    const char *str = sql.string();
    const char *tail;
    char **cargv = 0;

    p = (char *) str;
    nargs = 0;
    while (*p) {
        if (*p == '%') {
        ++p;
        if (*p == 'q' || *p == 'Q' || *p == 's') {
            nargs++;
            if (nargs > MAX_PARAMS) {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
        } else if (*p != '%') {
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }
        }
        ++p;
    }

    cargv = (char **)malloc((sizeof (char *)) * MAX_PARAMS);
    if (!cargv) {
        return E_NULL_POINTER_EXCEPTION;
    }
    for (i = 0; i < MAX_PARAMS; i++) {
        cargv[i] = NULL;
    }
    if (argsstr) {
        for (i = 0; i < nargs; i++) {
            if (i < argsstr->GetLength()) {
                cargv[i] = const_cast<char *>((*argsstr)[i].string());
            }

    }
    }
    h->row1 = 1;
#if defined(_WIN32) || !defined(CANT_PASS_VALIST_AS_CHARPTR)
        char *s = sqlite3_vmprintf(sqlstr.result, (char *) cargv);
#else
        char *s = sqlite3_mprintf(sql.string(),
                      cargv[0], cargv[1],
                      cargv[2], cargv[3],
                      cargv[4], cargv[5],
                      cargv[6], cargv[7],
                      cargv[8], cargv[9],
                      cargv[10], cargv[11],
                      cargv[12], cargv[13],
                      cargv[14], cargv[15],
                      cargv[16], cargv[17],
                      cargv[18], cargv[19],
                      cargv[20], cargv[21],
                      cargv[22], cargv[23],
                      cargv[24], cargv[25],
                      cargv[26], cargv[27],
                      cargv[28], cargv[29],
                      cargv[30], cargv[31]);
#endif
        if (!s) {
            rc = SQLITE_NOMEM;
        } else {
#if HAVE_SQLITE3_PREPARE_V2
        rc = sqlite3_prepare_v2((sqlite3 *) h->sqlite, s, -1,
                    (sqlite3_stmt **) &svm, &tail);
#else
        rc = sqlite3_prepare((sqlite3 *) h->sqlite, s, -1,
                      (sqlite3_stmt **) &svm, &tail);
#endif
        if (rc != SQLITE_OK) {
            if (svm) {
            sqlite3_finalize((sqlite3_stmt *) svm);
            svm = 0;
            }
        }
        }
        if (rc != SQLITE_OK) {
            sqlite3_free(s);
            Freep((char **) &cargv);
            ((CVm *)vm)->mError_code = rc;
            return E_SQL_EXCEPTION;
        }
        v = (hvm*)malloc(sizeof (hvm) + strlen(tail) + 1);
        if (!v) {
            sqlite3_free(s);
            Freep((char **) &cargv);
            sqlite3_finalize((sqlite3_stmt *) svm);
            ((CVm *)vm)->mError_code = SQLITE_NOMEM;
            return E_NULL_POINTER_EXCEPTION;
        }
        v->next = h->vms;
        h->vms = v;
        v->vm = svm;
        v->h = h;
        v->tail = (char *) (v + 1);
#if HAVE_BOTH_SQLITE
        v->is3 = v->hh.is3 = h->is3;
#endif
        strcpy(v->tail, tail);
        sqlite3_free(s);
        v->hh.sqlite = 0;
        v->hh.haveutf = h->haveutf;
        v->hh.ver = h->ver;
        v->hh.bh = 0;
        v->hh.cb = 0;
        v->hh.ai = 0;
        v->hh.tr = 0;
        v->hh.ph = 0;
        v->hh.row1 = 1;
        v->hh.enc = h->enc;
        v->hh.funcs = 0;
        v->hh.vms = 0;
        ((CVm *)vm)->mHandle = (Int64 )v;
        Freep((char **) &cargv);
        return NOERROR;
    }
#endif
#else
    throwex(env, "unsupported");
#endif
}

ECode Database::Stmt_prepare(
    /* [in] */ const String& sql,
    /* [in] */ IStmt* stmt)
{
#if HAVE_SQLITE3
    handle *h = (handle *)mHandle;
    void *svm = 0;
    hvm *v;
    Int32 len16 = 0;
    const char *sql16 = 0, *tail = 0;
    Int32 ret = 0;

    if (!h) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!stmt) {
        return E_NULL_POINTER_EXCEPTION;
    }
    if (!sql) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
#ifdef HAVE_BOTH_SQLITE
    if (!h->is3) {
    throwex(env, "only on SQLite3 database");
    return;
    }
#endif
    len16 = sql.GetByteLength();
    if (len16 < 1) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    sql16 = sql.string();
#if HAVE_SQLITE3_PREPARE16_V2
    ret = sqlite3_prepare16_v2((sqlite3 *) h->sqlite, sql16, len16,
                   (sqlite3_stmt **) &svm, (const void **) &tail);
#else
    ret = sqlite3_prepare((sqlite3 *) h->sqlite, sql16, len16,
                (sqlite3_stmt **) &svm, (const char **) &tail);
#endif
    if (ret != SQLITE_OK) {
        if (svm) {
            sqlite3_finalize((sqlite3_stmt *) svm);
            svm = 0;
        }
    }
    if (ret != SQLITE_OK) {
        const char *err = sqlite3_errmsg((sqlite3*)h->sqlite);
        Logger::E("Database", err);
        ((CStmt *)stmt)->mError_code = ret;
        return E_SQL_EXCEPTION;
    }
    if (!svm) {
        return E_NULL_POINTER_EXCEPTION;
    }
    len16 = len16 + sizeof(char) - ((char *) tail - (char *) sql16);
    if (len16 < (Int32)sizeof(char)) {
        len16 = sizeof(char);
    }
    v = (hvm*)malloc(sizeof (hvm) + len16);
    if (!v) {
        sqlite3_finalize((sqlite3_stmt *) svm);
        return E_OUT_OF_MEMORY;
    }
    v->next = h->vms;
    h->vms = v;
    v->vm = svm;
    v->h = h;
    v->tail = (char *) (v + 1);
#if HAVE_BOTH_SQLITE
    v->is3 = v->hh.is3 = 1;
#endif
    memcpy(v->tail, tail, len16);
    len16 /= sizeof (char);
    ((char *) v->tail)[len16 - 1] = 0;
    v->hh.sqlite = 0;
    v->hh.haveutf = h->haveutf;
    v->hh.ver = h->ver;
    v->hh.bh = 0;
    v->hh.cb = 0;
    v->hh.ai = 0;
    v->hh.tr = 0;
    v->hh.ph = 0;
    v->hh.row1 = 1;
    v->hh.enc = h->enc;
    v->hh.funcs = 0;
    v->hh.vms = 0;
    ((CStmt *)stmt)->mHandle = (Int64)v;
    return NOERROR;
#else
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
}

ECode Database::_OpenBlob(
    /* [in] */ const String& db,
    /* [in] */ const String& table,
    /* [in] */ const String& column,
    /* [in] */ Int64 row,
    /* [in] */ Boolean rw,
    /* [in] */ IBlob* blob)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    handle *h = (handle *)mHandle;
    hbl *bl;
    jthrowable exc;
    transstr dbn, tbl, col;
    sqlite3_blob *blob;
    jvalue vv;
    int ret;

    if (!blobj) {
    throwex(env, "null blob");
    return;
    }
#if HAVE_BOTH_SQLITE
    if (!h->is3) {
    throwex(env, "not a SQLite 3 database");
    return;
    }
#endif
    if (h && h->sqlite) {
    trans2iso(env, h->haveutf, h->enc, dbname, &dbn);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    trans2iso(env, h->haveutf, h->enc, table, &tbl);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        transfree(&dbn);
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    trans2iso(env, h->haveutf, h->enc, column, &col);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        transfree(&tbl);
        transfree(&dbn);
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    ret = sqlite3_blob_open(h->sqlite,
                dbn.result, tbl.result, col.result,
                row, rw, &blob);
    transfree(&col);
    transfree(&tbl);
    transfree(&dbn);
    if (ret != SQLITE_OK) {
        const char *err = sqlite3_errmsg(h->sqlite);

        seterr(env, obj, ret);
        throwex(env, err ? err : "error in blob open");
        return;
    }
    bl = malloc(sizeof (hbl));
    if (!bl) {
        sqlite3_blob_close(blob);
        throwoom(env, "unable to get SQLite blob handle");
        return;
    }
    bl->next = h->blobs;
    h->blobs = bl;
    bl->blob = blob;
    bl->h = h;
    vv.j = 0;
    vv.l = (jobject) bl;
    (*env)->SetLongField(env, blobj, F_SQLite_Blob_handle, vv.j);
    (*env)->SetIntField(env, blobj, F_SQLite_Blob_size,
                sqlite3_blob_bytes(blob));
    return;
    }
    throwex(env, "not an open database");
#else
    // throwex(env, "unsupported");
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
}

static Int32 Progresshandlervoid(void *udata)
{
    handle *h = (handle *) udata;
    Int32 ret = 0;
    if (h->ph) {
        Boolean phflag = FALSE;
        h->ph->Progress(&phflag);
        ret = phflag != TRUE;
    }
    return ret;
}

ECode Database::_ProgressHandler(
    /* [in] */ Int32 n,
    /* [in] */ IProgressHandler* ph)
{
    handle *h = (handle *)mHandle;

    if (h && h->sqlite) {
    /* CHECK THIS */
#if HAVE_SQLITE_PROGRESS_HANDLER

#if HAVE_BOTH_SQLITE
    if (h->is3) {
        if (ph) {
        globrefset(env, ph, &h->ph);
        h->ph = ph;
        sqlite3_progress_handler((sqlite3 *) h->sqlite,
                     n, progresshandler, h);
        } else {
        sqlite3_progress_handler((sqlite3 *) h->sqlite,
                     0, 0, 0);
        }
    } else {
        if (ph) {
        globrefset(env, ph, &h->ph);
        sqlite_progress_handler((sqlite *) h->sqlite,
                    n, progresshandler, h);
        } else {
        sqlite_progress_handler((sqlite *) h->sqlite,
                    0, 0, 0);
        }
    }
#else
#if HAVE_SQLITE2
    if (ph) {
        globrefset(env, ph, &h->ph);
        sqlite_progress_handler((sqlite *) h->sqlite,
                    n, progresshandler, h);
    } else {
        sqlite_progress_handler((sqlite *) h->sqlite,
                    0, 0, 0);
    }
#endif
#if HAVE_SQLITE3
    if (ph) {
        h->ph = ph;
        sqlite3_progress_handler((sqlite3 *) h->sqlite,
                     n, Progresshandlervoid, h);
    } else {
        sqlite3_progress_handler((sqlite3 *) h->sqlite,
                     0, 0, 0);
    }
#endif
#endif
    return NOERROR;
#else
    // throwex(env, "unsupported");
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    }
    return E_NULL_POINTER_EXCEPTION;
}

ECode Database::_Key(
    /* [in] */ ArrayOf<Byte> * ekey)
{
    Int32 len(0);
    AutoPtr<ArrayOf<Byte> > data;
#if HAVE_SQLITE3_KEY
    handle *h = gethandle(env, obj);
#endif

    if (ekey) {
        len = ekey->GetLength();
    }
    if (len != 0) {
        data = ekey;
    }

    if (!data) {
        len = 0;
    }
#if HAVE_SQLITE3_KEY
    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (!h->is3) {
        if (data) {
        memset(data, 0, len);
        }
        throwex(env, "unsupported");
    }
#endif
    sqlite3_key((sqlite3 *) h->sqlite, data, len);
    if (data) {
        memset(data, 0, len);
    }
    } else {
    if (data) {
        memset(data, 0, len);
    }
    throwclosed(env);
    }
#else
    if (data) {
    memset(data, 0, len);
    }
    /* no error */
#endif
    return NOERROR;
}

ECode Database::_Rekey(
    /* [in] */ ArrayOf<Byte> * ekey)
{
    Int32 len(0);
    AutoPtr<ArrayOf<Byte> > data;
#if HAVE_SQLITE3_KEY
    handle *h = (handle *)mHandle;
#endif
    len = ekey->GetLength();
    data = ekey;
    if (len == 0) {
        data = 0;
    }
    if (!data) {
        len = 0;
    }
#if HAVE_SQLITE3_KEY
    if (h && h->sqlite) {
#if HAVE_BOTH_SQLITE
    if (!h->is3) {
        if (data) {
        memset(data, 0, len);
        }
        throwex(env, "unsupported");
    }
#endif
    sqlite3_rekey((sqlite3 *) h->sqlite, data, len);
    if (data) {
        memset(data, 0, len);
    }
    } else {
    if (data) {
        memset(data, 0, len);
    }
    throwclosed(env);
    }
#else
    if (data) {
        memset(data, 0, len);
    }
    // throwex(env, "unsupported");
    return E_SQL_FEATURE_NOT_SUPPORTED_EXCEPTION;
#endif
    return NOERROR;
}

void Database::Internal_init()
{
// #if defined(DONT_USE_JNI_ONLOAD) || !defined(JNI_VERSION_1_2)
//     while (C_java_lang_String == 0) {
//     jclass jls = (*env)->FindClass(env, "java/lang/String");

//     C_java_lang_String = (*env)->NewGlobalRef(env, jls);
//     }
// #endif
//     F_SQLite_Database_handle =
//     (*env)->GetFieldID(env, cls, "handle", "J");
//     F_SQLite_Database_error_code =
//     (*env)->GetFieldID(env, cls, "error_code", "I");
//     M_java_lang_String_getBytes =
//     (*env)->GetMethodID(env, C_java_lang_String, "getBytes", "()[B");
//     M_java_lang_String_getBytes2 =
//     (*env)->GetMethodID(env, C_java_lang_String, "getBytes",
//                 "(Ljava/lang/String;)[B");
//     M_java_lang_String_initBytes =
//     (*env)->GetMethodID(env, C_java_lang_String, "<init>", "([B)V");
//     M_java_lang_String_initBytes2 =
//     (*env)->GetMethodID(env, C_java_lang_String, "<init>",
//                 "([BLjava/lang/String;)V");
}

Int32 Database::Status(
    /* [in] */ Int32 op,
    /* [in] */ ArrayOf<Int32>*
    /* [in] */ info,
    /* [in] */ Boolean flag)
{
    return _Status(op, info, flag);
}

Boolean Database::_Enable_shared_cache(
    /* [in] */ Boolean onoff)
{
    return FALSE;
}

Int64 Database::LongFromJulian(
    /* [in] */ Double d)
{
    d -= 2440587.5;
    d *= 86400000.0;
    return (Int64)d;
}

Int64 Database::LongFromJulian(
    /* [in] */ const String& s)
{

    Double d = StringUtils::ParseDouble(s); // android-changed: performance
    return LongFromJulian(d);
}

Double Database::JulianFromLong(
    /* [in] */ Int64 ms)
{
    Double adj = (ms < 0) ? 0 : 0.5;
    Double d = (ms + adj) / 86400000.0 + 2440587.5;
    return d;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
