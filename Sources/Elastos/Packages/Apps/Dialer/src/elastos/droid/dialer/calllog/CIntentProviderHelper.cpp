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

#include "elastos/droid/dialer/calllog/CIntentProviderHelper.h"
#include "elastos/droid/dialer/calllog/IntentProvider.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace CallLog {

CAR_INTERFACE_IMPL(CIntentProviderHelper, Singleton, IIntentProviderHelper);

CAR_SINGLETON_IMPL(CIntentProviderHelper);

ECode CIntentProviderHelper::GetReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnCallIntentProvider(number);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnCallIntentProvider(
            number, accountHandle);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnVideoCallIntentProvider(number);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetReturnVideoCallIntentProvider(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetReturnVideoCallIntentProvider(
            number, accountHandle);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetPlayVoicemailIntentProvider(
    /* [in] */ Int64 rowId,
    /* [in] */ const String& voicemailUri,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetPlayVoicemailIntentProvider(
            rowId, voicemailUri);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

ECode CIntentProviderHelper::GetCallDetailIntentProvider(
    /* [in] */ Int64 id,
    /* [in] */ ArrayOf<Int64>* extraIds,
    /* [in] */ const String& voicemailUri,
    /* [out] */ IIntentProvider** provider)
{
    VALIDATE_NOT_NULL(provider);
    AutoPtr<IIntentProvider> result = IntentProvider::GetCallDetailIntentProvider(
            id, extraIds, voicemailUri);
    *provider = result;
    REFCOUNT_ADD(*provider);
    return NOERROR;
}

} // CallLog
} // Dialer
} // Droid
} // Elastos
