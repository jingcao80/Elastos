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

#include "elastos/droid/hardware/CCarrierFrequencyRange.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Hardware {

CAR_INTERFACE_IMPL(CCarrierFrequencyRange, Object, ICarrierFrequencyRange)

CAR_OBJECT_IMPL(CCarrierFrequencyRange)

ECode CCarrierFrequencyRange::constructor(
    /* [in] */ Int32 min,
    /* [in] */ Int32 max)
{
    mMinFrequency = min;
    mMaxFrequency = max;
    return NOERROR;
}

ECode CCarrierFrequencyRange::GetMinFrequency(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mMinFrequency;
    return NOERROR;
}

ECode CCarrierFrequencyRange::GetMaxFrequency(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);

    *value = mMaxFrequency;
    return NOERROR;
}

} // namespace Hardware
} // namespace Droid
} // namespace Elastos
