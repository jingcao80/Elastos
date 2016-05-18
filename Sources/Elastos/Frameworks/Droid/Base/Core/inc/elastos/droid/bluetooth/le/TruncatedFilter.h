#ifndef __ELASTOS_DROID_BLUETOOTH_LE_TRUNCATEDFILTER_H__
#define __ELASTOS_DROID_BLUETOOTH_LE_TRUNCATEDFILTER_H__

#include "Elastos.Droid.Bluetooth.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Bluetooth {
namespace LE {

/**
  * A special scan filter that lets the client decide how the scan record should be stored.
  *
  * @hide
  */
// @SystemApi
class TruncatedFilter
    : public Object
    , public ITruncatedFilter
{
public:
    CAR_INTERFACE_DECL()

    TruncatedFilter();

    /**
      * Constructor for {@link TruncatedFilter}.
      *
      * @param filter Scan filter of the truncated filter.
      * @param storageDescriptors Describes how the scan should be stored.
      */
    constructor(
        /* [in] */ IScanFilter* filter,
        /* [in] */ IList* storageDescriptors);// ResultStorageDescriptor

    /**
      * Returns the scan filter.
      */
    CARAPI GetFilter(
        /* [out] */ IScanFilter** result);

    /**
      * Returns a list of descriptor for scan result storage.
      */
    CARAPI GetStorageDescriptors(
        /* [out] */ IList** result); // ResultStorageDescriptor

private:
    AutoPtr<IScanFilter> mFilter;
    AutoPtr<IList> mStorageDescriptors; // ResultStorageDescriptor
};

} // namespace LE
} // namespace Bluetooth
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_BLUETOOTH_LE_TRUNCATEDFILTER_H__

