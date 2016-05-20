
#include "CBlob.h"
#include "CBlobR.h"
#include "CBlobW.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CBlob)

CAR_INTERFACE_IMPL(CBlob, Object, IBlob)

CBlob::CBlob()
    : mSize(0)
    , mHandle(0)
{
}

ECode CBlob::GetInputStream(
    /* [out] */ IInputStream** instream)
{
    AutoPtr<IBlobR> tmp;
    FAIL_RETURN(CBlobR::New(this, (IBlobR** )&tmp));
    *instream = IInputStream::Probe(tmp);
    REFCOUNT_ADD(*instream);
    return NOERROR;
}

ECode CBlob::GetOutputStream(
    /* [out] */ IOutputStream ** outstream)
{
    AutoPtr<IBlobW> tmp;
    FAIL_RETURN(CBlobW::New(this, (IBlobW** )&tmp));
    *outstream = IOutputStream::Probe(tmp);
    REFCOUNT_ADD(*outstream);
    return NOERROR;
}

ECode CBlob::Close()
{
#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    hbl *bl = mHandle;

    if (bl) {
    if (bl->h) {
        handle *h = bl->h;
        hbl *blc, **blp;

        blp = &h->blobs;
        blc = *blp;
        while (blc) {
        if (blc == bl) {
            *blp = blc->next;
            break;
        }
        blp = &blc->next;
        blc = *blp;
        }
    }
    if (bl->blob) {
        sqlite3_blob_close(bl->blob);
    }
    bl->blob = 0;
    free(bl);
    mHandle = 0;
    mSize = 0;
    }
#endif
    return NOERROR;
}

ECode CBlob::Write(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    hbl *bl = mHandle;

    if (bl && bl->h && bl->blob) {
        Int32 ret;

        if (len <= 0) {
            return NOERROR;
        }

        ret = sqlite3_blob_write(bl->blob, b, len, pos);
        if (ret != SQLITE_OK) {
            return NOERROR;
        }

        *count = len;
        return NOERROR;
    }
#else
   return E_SQL_SQLITE_THROWEX_EXCEPTION;
#endif
}

ECode CBlob::Read(
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 pos,
    /* [in] */ Int32 len,
    /* [out] */ Int32 * count)
{
    VALIDATE_NOT_NULL(count)
    *count = 0;

#if HAVE_SQLITE3 && HAVE_SQLITE3_INCRBLOBIO
    hbl *bl = mHandle;

    if (bl && bl->h && bl->blob) {
        Int32 ret;

        if (len <= 0) {
            return NOERROR;
        }
        ret = sqlite3_blob_read(bl->blob, b, len, pos);
        if (ret != SQLITE_OK) {
            return NOERROR;
        }
        *count = len;
        return NOERROR;
    }
    return E_SQL_SQLITE_THROWEX_EXCEPTION;
#else
    return E_SQL_SQLITE_THROWEX_EXCEPTION;
#endif
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
