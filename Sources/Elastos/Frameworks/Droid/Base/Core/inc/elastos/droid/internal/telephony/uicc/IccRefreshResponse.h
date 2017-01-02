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

#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"


namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {
namespace Uicc {

/**
  * See also RIL_SimRefresh in include/telephony/ril.h
  *
  * {@hide}
  */
class IccRefreshResponse
    : public Object
    , public IIccRefreshResponse
{
public:
    CAR_INTERFACE_DECL();
    /* null terminated string, e.g.,
                                                   from 0xA0, 0x00 -> 0x41,
                                                   0x30, 0x30, 0x30 */
    /* Example: a0000000871002f310ffff89080000ff */
    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

public:
    Int32 mRefreshResult;
    /* Sim Refresh result */
    Int32 mEfId;
    /* EFID */
    String mAid;
};

} // namespace Uicc
} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_UICC_ICCREFRESHRESPONSE_H__

