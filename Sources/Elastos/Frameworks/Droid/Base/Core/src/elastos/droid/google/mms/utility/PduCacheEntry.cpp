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

#include "Elastos.Droid.Google.h"
#include "elastos/droid/google/mms/utility/PduCacheEntry.h"

namespace Elastos {
namespace Droid {
namespace Google {
namespace Mms {
namespace Utility {

//=====================================================================
//                            PduCacheEntry
//=====================================================================
CAR_INTERFACE_IMPL(PduCacheEntry, Object, IPduCacheEntry);

PduCacheEntry::PduCacheEntry()
{
}

ECode PduCacheEntry::constructor(
    /* [in] */ IGenericPdu* pdu,
    /* [in] */ Int32 msgBox,
    /* [in] */ Int64 threadId)
{
    mPdu = pdu;
    mMessageBox = msgBox;
    mThreadId = threadId;
    return NOERROR;
}

ECode PduCacheEntry::GetPdu(
    /* [out] */ IGenericPdu** result)
{
    VALIDATE_NOT_NULL(result);
    *result = mPdu;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode PduCacheEntry::GetMessageBox(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mMessageBox;
    return NOERROR;
}

ECode PduCacheEntry::GetThreadId(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mThreadId;
    return NOERROR;
}

} // namespace Utility
} // namespace Mms
} // namespace Google
} // namespace Droid
} // namespace Elastos
