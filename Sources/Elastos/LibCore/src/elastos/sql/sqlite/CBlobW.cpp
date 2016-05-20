#include "CBlobW.h"

namespace Elastos {
namespace Sql {
namespace SQLite {

CAR_OBJECT_IMPL(CBlobW)

CAR_INTERFACE_IMPL(CBlobW, OutputStream, IBlobW)

CBlobW::CBlobW()
    : mPos(0)
{
}

ECode CBlobW::constructor(
    /* [in] */ Elastos::Sql::SQLite::IBlob* blob)
{
    mBlob = (CBlob *)blob;
    return NOERROR;
}

ECode CBlobW::Write(
    /* [in] */ Int32 oneByte)
{
    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(1);
    (*b)[0] = (Byte) oneByte;
    Int32 n;
    FAIL_RETURN(mBlob->Write(b, 0, mPos, 1, &n))
    mPos += n;
    return NOERROR;
}

ECode CBlobW::WriteBytes(
    /* [in] */ ArrayOf<Byte>* buffer)
{
    if (buffer && buffer->GetLength() > 0) {
        Int32 n;
        FAIL_RETURN(mBlob->Write(buffer, 0, mPos, buffer->GetLength(), &n))
        mPos += n;
    }
    return NOERROR;
}

ECode CBlobW::WriteBytes(
    /* [in] */ ArrayOf<Byte> * b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len)
{
    VALIDATE_NOT_NULL(b)

    if (off + len > b->GetLength()) {
        len = b->GetLength() - off;
    }
    if (len <= 0) {
       return NOERROR;
    }

    Int32 n;
    FAIL_RETURN(mBlob->Write(b, off, mPos, len, &n))
    mPos += n;

    return NOERROR;
}

ECode CBlobW::Close()
{
    mBlob->Close();
    mBlob = NULL;
    mPos = 0;
    return NOERROR;
}

ECode CBlobW::Flush()
{
    return NOERROR;
}

} // namespace SQLite
} // namespace Sql
} // namespace Elastos
