
#include "CFunctionContext.h"
#include "sqlitejni.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CFunctionContext)
CAR_INTERFACE_IMPL(CFunctionContext, Object, IFunctionContext)

CFunctionContext::CFunctionContext()
    : mHandle(0)
{
}

ECode CFunctionContext::SetResult(
    /* [in] */ const String& ret)
{
    hfunc *f = (hfunc *)mHandle;
    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (!f->is3) {
        String retstr;

        trans2iso(env, f->h->haveutf, f->h->enc, ret, &retstr);
        exc = (*env)->ExceptionOccurred(env);
        if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
        }
        sqlite_set_result_string((sqlite_func *) f->sf,
                     retstr.result, -1);
        transfree(&retstr);
    } else if (ret) {
        jsize len = (*env)->GetStringLength(env, ret) * sizeof (jchar);
        const jchar *str = (*env)->GetStringChars(env, ret, 0);

        sqlite3_result_text((sqlite3_context *) f->sf, str, len,
                  SQLITE_TRANSIENT);
        (*env)->ReleaseStringChars(env, ret, str);
    } else {
        sqlite3_result_null((sqlite3_context *) f->sf);
    }
#else
#if HAVE_SQLITE2
    transstr retstr;
    jthrowable exc;

    trans2iso(env, f->h->haveutf, f->h->enc, ret, &retstr);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    sqlite_set_result_string((sqlite_func *) f->sf, retstr.result, -1);
    transfree(&retstr);
#endif
#if HAVE_SQLITE3
    if (!ret.IsNull()) {
        Int32 len = ret.GetByteLength();
        sqlite3_result_text((sqlite3_context *) f->sf, ret.string(), len,
                  SQLITE_TRANSIENT);
    } else {
        sqlite3_result_null((sqlite3_context *) f->sf);
    }
#endif
#endif
    }
    return NOERROR;
}

ECode CFunctionContext::SetResult(
    /* [in] */ Int32 i)
{
    hfunc *f = (hfunc *)mHandle;

    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (f->is3) {
        sqlite3_result_int((sqlite3_context *) f->sf, (int) i);
    } else {
        sqlite_set_result_int((sqlite_func *) f->sf, (int) i);
    }
#else
#if HAVE_SQLITE2
    sqlite_set_result_int((sqlite_func *) f->sf, (int) i);
#endif
#if HAVE_SQLITE3
    sqlite3_result_int((sqlite3_context *) f->sf, (Int32) i);
#endif
#endif
    }

    return NOERROR;
}

ECode CFunctionContext::SetResult(
    /* [in] */ Double d)
{
    hfunc *f = (hfunc *)mHandle;
    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (f->is3) {
        sqlite3_result_double((sqlite3_context *) f->sf, (double) d);
    } else {
        sqlite_set_result_double((sqlite_func *) f->sf, (double) d);
    }
#else
#if HAVE_SQLITE2
    sqlite_set_result_double((sqlite_func *) f->sf, (double) d);
#endif
#if HAVE_SQLITE3
    sqlite3_result_double((sqlite3_context *) f->sf, (Double) d);
#endif
#endif
    }
    return NOERROR;
}

ECode CFunctionContext::SetError(
    /* [in] */ const String& err)
{
    hfunc *f = (hfunc *)mHandle;

    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (!f->is3) {
        transstr errstr;
        jthrowable exc;

        trans2iso(env, f->h->haveutf, f->h->enc, err, &errstr);
        exc = (*env)->ExceptionOccurred(env);
        if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
        }
        sqlite_set_result_error((sqlite_func *) f->sf,
                    errstr.result, -1);
        transfree(&errstr);
    } else if (err) {
        jsize len = (*env)->GetStringLength(env, err) * sizeof (jchar);
        const jchar *str = (*env)->GetStringChars(env, err, 0);

        sqlite3_result_error16((sqlite3_context *) f->sf, str, len);
        (*env)->ReleaseStringChars(env, err, str);
    } else {
        sqlite3_result_error((sqlite3_context *) f->sf,
                 "null error text", -1);
    }
#else
#if HAVE_SQLITE2
    transstr errstr;
    jthrowable exc;

    trans2iso(env, f->h->haveutf, f->h->enc, err, &errstr);
    exc = (*env)->ExceptionOccurred(env);
    if (exc) {
        (*env)->DeleteLocalRef(env, exc);
        return;
    }
    sqlite_set_result_error((sqlite_func *) f->sf, errstr.result, -1);
    transfree(&errstr);
#endif
#if HAVE_SQLITE3
    if (err) {
        Int32 len = err.GetByteLength();
        sqlite3_result_error16((sqlite3_context *) f->sf, err.string(), len);
    } else {
        sqlite3_result_error((sqlite3_context *) f->sf,
                 "null error text", -1);
    }
#endif
#endif
    }
    return NOERROR;
}

ECode CFunctionContext::SetResult(
    /* [in] */ const ArrayOf<Byte>& b)
{
#if HAVE_SQLITE3
    hfunc *f = (hfunc *)mHandle;

    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (!f->is3) {
        /* silently ignored */
        return NOERROR;
    }
#endif
    if (b.GetPayload()) {
        Int32 len = 0;
        len = b.GetLength();
        sqlite3_result_blob((sqlite3_context *) f->sf,
                b.GetPayload(), len, SQLITE_TRANSIENT);
    } else {
        sqlite3_result_null((sqlite3_context *) f->sf);
    }
    }
#endif
    return NOERROR;
}

ECode CFunctionContext::SetResultZeroblob(
    /* [in] */ Int32 n)
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_RESULT_ZEROBLOB
    hfunc *f = (hfunc *)mHandle;

    if (f && f->sf) {
#if HAVE_BOTH_SQLITE
    if (!f->is3) {
        /* silently ignored */
        return NOERROR;
    }
#endif
    sqlite3_result_zeroblob((sqlite3_context *) f->sf, n);
    }
#endif
    return NOERROR;
}

ECode CFunctionContext::Count(
    /* [out] */ Int32* count)
{
    hfunc *f = (hfunc *)mHandle;
    Int32 r = 0;

    if (f && f->sf) {
#if HAVE_SQLITE_BOTH
    if (f->is3) {
        r = (jint) sqlite3_aggregate_count((sqlite3_context *) f->sf);
    } else {
        r = (jint) sqlite_aggregate_count((sqlite_func *) f->sf);
    }
#else
#if HAVE_SQLITE2
    r = (jint) sqlite_aggregate_count((sqlite_func *) f->sf);
#endif
#if HAVE_SQLITE3
    r = (Int32) sqlite3_aggregate_count((sqlite3_context *) f->sf);
#endif
#endif
    }
    *count = r;
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
