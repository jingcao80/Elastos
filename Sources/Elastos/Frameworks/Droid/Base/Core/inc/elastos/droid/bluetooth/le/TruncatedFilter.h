//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
    CARAPI constructor(
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

