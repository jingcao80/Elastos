
#include "elastos/io/NIOAccess.h"

namespace Elastos {
namespace IO {

ECode NIOAccess::GetBasePointer(
    /* [in] */ IBuffer* b,
    /* [out] */ Int64* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        long address = b.effectiveDirectAddress;
        if (address == 0L) {
            return 0L;
        }
        return address + (b.position << b._elementSizeShift);

#endif
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

ECode NIOAccess::GetBaseArrayOffset(
    /* [in] */ IBuffer* b,
    /* [out] */ Int32* result)
{
    return E_NOT_IMPLEMENTED;
#if 0 // TODO: Translate codes below
        return b.hasArray() ? ((b.arrayOffset() + b.position) << b._elementSizeShift) : 0;

#endif
}

} // namespace IO
} // namespace Elastos
