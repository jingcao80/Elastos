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
