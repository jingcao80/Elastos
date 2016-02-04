
#include "elastos/io/NIOAccess.h"
#include "elastos/io/Buffer.h"

namespace Elastos {
namespace IO {

ECode NIOAccess::GetBasePointer(
    /* [in] */ IBuffer* b,
    /* [out] */ Int64* result)
{
    assert(b != NULL);
    Buffer* buf = (Buffer*)b;
    if (buf == NULL || buf->mEffectiveDirectAddress == 0)
    {
        return 0L;
    }
    return buf->mEffectiveDirectAddress + (buf->mPosition << buf->mElementSizeShift);
}

ECode NIOAccess::GetBaseArray(
    /* [in] */ IBuffer* b,
    /* [out] */ IInterface** result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return b.hasArray() ? b.array() : null;

#endif
}

ECode NIOAccess::GetRemainingBytes(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* remaining)
{
    assert(b);
    Buffer* buf = (Buffer*)b;
    if (buf != NULL) {
        return (buf->mLimit - buf->mPosition) << buf->mElementSizeShift;
    }
    else {
        return -1;
    }
}

ECode NIOAccess::GetBaseArrayOffset(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* result)
{
    assert(b);
    Boolean hasArray;
    b->HasArray(&hasArray);
    Buffer* buf = (Buffer*)b;
    if (hasArray && buf != NULL) {
        Int32 offset, position;
        b->GetArrayOffset(&offset);
        b->GetPosition(&position);
        return (offset + position)<< buf->mElementSizeShift;
    }
    else {
        return 0;
    }
}

} // namespace IO
} // namespace Elastos
