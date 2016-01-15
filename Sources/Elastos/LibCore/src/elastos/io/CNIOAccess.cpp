#include "elastos/core/Object.h"
#include "elastos/io/CNIOAccess.h"
#include "elastos/io/NIOAccess.h"

namespace Elastos {
namespace IO {

CAR_INTERFACE_IMPL(CNIOAccess, Singleton, INIOAccess)

CAR_SINGLETON_IMPL(CNIOAccess)

ECode CNIOAccess::GetBasePointer(
    /* [in] */ IBuffer* b,
    /* [out] */ Int64* pointer)
{
    return NIOAccess::GetBasePointer(b, pointer);
}

ECode CNIOAccess::GetBaseArray(
        /* [in] */ IBuffer* b,
        /* [out] */ IInterface** result)
{
    return NIOAccess::GetBaseArray(b, result);
}


ECode CNIOAccess::GetBaseArrayOffset(
        /* [in] */ IBuffer* b,
        /* [out] */ Int32* result)
{
    return NIOAccess::GetBaseArrayOffset(b, result);
}

} // namespace IO
} // namespace Elastos