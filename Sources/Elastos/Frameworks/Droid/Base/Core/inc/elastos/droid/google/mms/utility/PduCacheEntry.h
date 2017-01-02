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

#ifndef __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__
#define __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"

using Elastos::Droid::Google::Mms::Pdu::IGenericPdu;

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

class PduCacheEntry
    : public Object
    , public IPduCacheEntry
{
public:
    CAR_INTERFACE_DECL();

    PduCacheEntry();

    CARAPI constructor(
        /* [in] */ IGenericPdu* pdu,
        /* [in] */ Int32 msgBox,
        /* [in] */ Int64 threadId);

    virtual CARAPI GetPdu(
        /* [out] */ IGenericPdu** result);

    virtual CARAPI GetMessageBox(
        /* [out] */ Int32* result);

    virtual CARAPI GetThreadId(
        /* [out] */ Int64* result);

private:
    /*const*/ AutoPtr<IGenericPdu> mPdu;
    /*const*/ Int32 mMessageBox;
    /*const*/ Int64 mThreadId;
};

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GOOGLE_MMS_UTILITY_PDUCACHEENTRY_H__
