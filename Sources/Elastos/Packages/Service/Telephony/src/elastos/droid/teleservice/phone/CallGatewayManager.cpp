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

#include "elastos/droid/teleservice/phone/CallGatewayManager.h"
#include "elastos/droid/text/TextUtils.h"
#include "Elastos.CoreLibrary.Utility.Concurrent.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Telecom.h"
#include "Elastos.Droid.Telephony.h"
#include <elastos/core/AutoLock.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Net::IUriHelper;
using Elastos::Droid::Net::CUriHelper;
using Elastos::Droid::Telecom::IPhoneAccount;
using Elastos::Droid::Telephony::CPhoneNumberUtils;
using Elastos::Droid::Telephony::IPhoneNumberUtils;
using Elastos::Droid::Text::TextUtils;
using Elastos::Utility::Concurrent::CConcurrentHashMap;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

String CallGatewayManager::RawGatewayInfo::GetFormattedGatewayNumber()
{
    return CallGatewayManager::FormatProviderUri(mGatewayUri);
}

Boolean CallGatewayManager::RawGatewayInfo::IsEmpty()
{
    return TextUtils::IsEmpty(mPackageName) || mGatewayUri == NULL;
}

const String CallGatewayManager::EXTRA_GATEWAY_PROVIDER_PACKAGE("com.android.phone.extra.GATEWAY_PROVIDER_PACKAGE");

const String CallGatewayManager::EXTRA_GATEWAY_URI("com.android.phone.extra.GATEWAY_URI");

AutoPtr<CallGatewayManager::RawGatewayInfo> CallGatewayManager::EMPTY_INFO = new RawGatewayInfo(String(NULL), NULL, String(NULL));

const String CallGatewayManager::TAG("CallGatewayManager");// = CallGatewayManager.class.getSimpleName();

AutoPtr<CallGatewayManager> CallGatewayManager::sSingleton;
Object CallGatewayManager::THIS;

AutoPtr<CallGatewayManager> CallGatewayManager::GetInstance()
{
    {
        AutoLock syncLock(THIS);
        if (sSingleton == NULL) {
            sSingleton = new CallGatewayManager();
        }
    }
    return sSingleton;
}

CallGatewayManager::CallGatewayManager()
{
    CConcurrentHashMap::New(4, 0.9f, 1, (IConcurrentHashMap**)&mMap);
}

AutoPtr<CallGatewayManager::RawGatewayInfo> CallGatewayManager::GetRawGatewayInfo(
    /* [in] */ IIntent* intent,
    /* [in] */ const String& number)
{
    if (HasPhoneProviderExtras(intent)) {
        String extra;
        intent->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &extra);
        AutoPtr<IUri> uri = GetProviderGatewayUri(intent);
        AutoPtr<RawGatewayInfo> info = new RawGatewayInfo(extra, uri, number);
        return info;
    }
    return EMPTY_INFO;
}

ECode CallGatewayManager::SetGatewayInfoForConnection(
    /* [in] */ IConnection* connection,
    /* [in] */ RawGatewayInfo* gatewayInfo)
{
    if (!gatewayInfo->IsEmpty()) {
        mMap->Put(TO_IINTERFACE(connection), TO_IINTERFACE(gatewayInfo));
    }
    else {
        mMap->Remove(TO_IINTERFACE(connection));
    }
    return NOERROR;
}

ECode CallGatewayManager::ClearGatewayData(
    /* [in] */ IConnection* connection)
{
    SetGatewayInfoForConnection(connection, EMPTY_INFO);
    return NOERROR;
}

AutoPtr<CallGatewayManager::RawGatewayInfo> CallGatewayManager::GetGatewayInfo(
    /* [in] */ IConnection* connection)
{
    AutoPtr<IInterface> obj;
    mMap->Get(TO_IINTERFACE(connection), (IInterface**)&obj);
    AutoPtr<RawGatewayInfo> info = (RawGatewayInfo*)IObject::Probe(obj);
    if (info != NULL) {
        return info;
    }

    return EMPTY_INFO;
}

Boolean CallGatewayManager::HasPhoneProviderExtras(
    /* [in] */ IIntent* intent)
{
    if (NULL == intent) {
        return FALSE;
    }
    String name;
    intent->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &name);
    String gatewayUri;
    intent->GetStringExtra(EXTRA_GATEWAY_URI, &gatewayUri);

    return !TextUtils::IsEmpty(name) && !TextUtils::IsEmpty(gatewayUri);
}

ECode CallGatewayManager::CheckAndCopyPhoneProviderExtras(
    /* [in] */ IIntent* src,
    /* [in] */ IIntent* dst)
{
    if (!HasPhoneProviderExtras(src)) {
        Logger::D(TAG, "checkAndCopyPhoneProviderExtras: some or all extras are missing.");
        return NOERROR;
    }

    String extra;
    src->GetStringExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, &extra);
    dst->PutExtra(EXTRA_GATEWAY_PROVIDER_PACKAGE, extra);

    String extra2;
    src->GetStringExtra(EXTRA_GATEWAY_URI, &extra2);
    return dst->PutExtra(EXTRA_GATEWAY_URI, extra2);
}

AutoPtr<IUri> CallGatewayManager::GetProviderGatewayUri(
    /* [in] */ IIntent* intent)
{
    String uri;
    intent->GetStringExtra(EXTRA_GATEWAY_URI, &uri);

    if(TextUtils::IsEmpty(uri)){
        return NULL;
    }
    else {
        AutoPtr<IUri> _uri;
        AutoPtr<IUriHelper> helper;
        CUriHelper::AcquireSingleton((IUriHelper**)&helper);
        helper->Parse(uri, (IUri**)&_uri);
        return _uri;
    }
    return NULL;
}

String CallGatewayManager::FormatProviderUri(
    /* [in] */ IUri* uri)
{
    if (uri != NULL) {
        String schem;
        uri->GetScheme(&schem);
        if (IPhoneAccount::SCHEME_TEL.Equals(schem)) {
            String schemeSpecific;
            uri->GetSchemeSpecificPart(&schemeSpecific);
            AutoPtr<IPhoneNumberUtils> helper;
            CPhoneNumberUtils::AcquireSingleton((IPhoneNumberUtils**)&helper);
            String v;
            helper->FormatNumber(schemeSpecific, &v);
            return v;
        }
        else {
            String str;
            IObject::Probe(uri)->ToString(&str);
            return str;
        }
    }
    return String(NULL);
}

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos