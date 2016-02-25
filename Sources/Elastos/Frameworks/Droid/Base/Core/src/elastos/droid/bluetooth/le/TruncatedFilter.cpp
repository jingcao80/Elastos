
#include "elastos/droid/bluetooth/le/TruncatedFilter.h"

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

//=====================================================================
//                           TruncatedFilter
//=====================================================================
CAR_INTERFACE_IMPL(TruncatedFilter, Object, ITruncatedFilter);

TruncatedFilter::TruncatedFilter()
{
}

ECode TruncatedFilter::constructor(
    /* [in] */ IScanFilter* filter,
    /* [in] */ IList* storageDescriptors)
{
    mFilter = filter;
    mStorageDescriptors = storageDescriptors;
    return NOERROR;
}

ECode TruncatedFilter::GetFilter(
    /* [out] */ IScanFilter** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mFilter;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode TruncatedFilter::GetStorageDescriptors(
    /* [out] */ IList** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mStorageDescriptors;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos
