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

#include "elastos/droid/provider/CTelephonyThreads.h"
#include "elastos/droid/provider/Telephony.h"
#include "Elastos.Droid.Net.h"

namespace Elastos {
namespace Droid {
namespace Provider {

CAR_INTERFACE_IMPL_2(CTelephonyThreads, Singleton, ITelephonyThreads, ITelephonyThreadsColumns);
CAR_SINGLETON_IMPL(CTelephonyThreads);

ECode CTelephonyThreads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ const String& recipient,
    /* [out] */ Int64* threadId)
{
    return Telephony::Threads::GetOrCreateThreadId(context, recipient, threadId);
}

ECode CTelephonyThreads::GetOrCreateThreadId(
    /* [in] */ IContext* context,
    /* [in] */ ISet* recipients,//String
    /* [out] */ Int64* threadId)
{
    return Telephony::Threads::GetOrCreateThreadId(context, recipients, threadId);
}

ECode CTelephonyThreads::GetOBSOLETE_THREADS_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Threads::OBSOLETE_THREADS_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

ECode CTelephonyThreads::GetCONTENT_URI(
    /* [out] */ IUri** uri)
{
    VALIDATE_NOT_NULL(uri);
    *uri = Telephony::Threads::CONTENT_URI;
    REFCOUNT_ADD(*uri);
    return NOERROR;
}

} // namespace Provider
} // namespace Droid
} // namespace Elastos
