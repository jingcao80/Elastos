#include "elastos/droid/database/CharArrayBuffer.h"

namespace Elastos {
namespace Droid {
namespace Database {

CAR_INTERFACE_IMPL(CharArrayBuffer, Object, ICharArrayBuffer)

CharArrayBuffer::CharArrayBuffer()
    : mSizeCopied(0)
{}

ECode CharArrayBuffer::constructor(
    /* [in] */ Int32 size)
{
    mData = ArrayOf<Char32>::Alloc(size);
    return NOERROR;
}

ECode CharArrayBuffer::constructor(
    /* [in] */ ArrayOf<Char32>* buf)
{
    mData = buf;
    return NOERROR;
}

ECode CharArrayBuffer::GetData(
    /* [out, callee] */ ArrayOf<Char32>** data)
{
    VALIDATE_NOT_NULL(data)
    *data = mData;
    REFCOUNT_ADD(*data);
    return NOERROR;
}

ECode CharArrayBuffer::SetData(
    /* [in] */ ArrayOf<Char32>* data)
{
    mData = data;
    return NOERROR;
}

ECode CharArrayBuffer::GetSizeCopied(
    /* [out] */ Int32* sizeCopied)
{
    VALIDATE_NOT_NULL(sizeCopied)
    *sizeCopied = mSizeCopied;
    return NOERROR;
}

ECode CharArrayBuffer::SetSizeCopied(
    /* [in] */ Int32 sizeCopied)
{
    mSizeCopied = sizeCopied;
    return NOERROR;
}

} //Database
} //Droid
} //Elastos
