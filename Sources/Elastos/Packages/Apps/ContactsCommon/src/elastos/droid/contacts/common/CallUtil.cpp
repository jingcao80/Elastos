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

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/contacts/common/CallUtil.h"
#include "elastos/droid/contacts/common/util/PhoneNumberHelper.h"
#include <elastos/droid/net/Uri.h>

using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telecom::ITelecomManager;
using Elastos::Droid::Telecom::IVideoProfileVideoState;

using Elastos::Droid::Contacts::Common::Util::PhoneNumberHelper;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number)
{
    return GetCallIntent(number, String(NULL), NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri)
{
    return GetCallIntent(uri, String(NULL), NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin)
{
    return GetCallIntent(GetCallUri(number), callOrigin, NULL);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(number, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(uri, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(GetCallUri(number), callOrigin, accountHandle);

}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(uri, callOrigin, accountHandle,
            IVideoProfileVideoState::AUDIO_ONLY);

}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin)
{
    return GetCallIntent(GetCallUri(number), callOrigin, NULL,
                IVideoProfileVideoState::BIDIRECTIONAL);
}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetCallIntent(GetCallUri(number), callOrigin, accountHandle,
                IVideoProfileVideoState::BIDIRECTIONAL);
}

AutoPtr<IIntent> CallUtil::GetVideoCallIntent(
    /* [in] */ const String& number,
    /* [in] */ IPhoneAccountHandle* accountHandle)
{
    return GetVideoCallIntent(number, String(NULL), accountHandle);
}

AutoPtr<IIntent> CallUtil::GetCallIntent(
    /* [in] */ IUri* uri,
    /* [in] */ const String& callOrigin,
    /* [in] */ IPhoneAccountHandle* accountHandle,
    /* [in] */ Int32 videoState)
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_CALL_PRIVILEGED, uri, (IIntent**)&intent);
    intent->PutExtra(ITelecomManager::EXTRA_START_CALL_WITH_VIDEO_STATE, videoState);
    if (!callOrigin.IsNull()) {
        assert(0 && "TODO");
        // intent->PutExtra(IPhoneConstants::EXTRA_CALL_ORIGIN, callOrigin);
    }
    if (accountHandle != NULL) {
        intent->PutExtra(ITelecomManager::EXTRA_PHONE_ACCOUNT_HANDLE, IParcelable::Probe(accountHandle));
    }

    return intent;
}

AutoPtr<IUri> CallUtil::GetCallUri(
    /* [in] */ const String& number)
{
    AutoPtr<IUri> uri;
    if (PhoneNumberHelper::IsUriNumber(number)) {
        Uri::FromParts(IPhoneAccount::SCHEME_SIP, number, String(NULL), (IUri**)&uri);
        return uri;
    }
    Uri::FromParts(IPhoneAccount::SCHEME_TEL, number, String(NULL), (IUri**)&uri);
    return uri;
}

Boolean CallUtil::IsVideoEnabled(
    /* [in] */ IContext* context)
{
    AutoPtr<IInterface> service;
    context->GetSystemService(IContext::TELECOM_SERVICE, (IInterface**)&service);
    ITelecomManager* telecommMgr = ITelecomManager::Probe(service);
    if (telecommMgr == NULL) {
        return FALSE;
    }

    // TODO: Check telecommManager for value instead.
    // return telecommMgr.isVideoEnabled();
    return FALSE;
}

} // Common
} // Contacts
} // Droid
} // Elastos
