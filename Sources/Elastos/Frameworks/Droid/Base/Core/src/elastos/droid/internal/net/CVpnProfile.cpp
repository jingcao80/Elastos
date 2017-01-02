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

#include <Elastos.CoreLibrary.Net.h>
#include "elastos/droid/internal/net/CVpnProfile.h"
#include "elastos/droid/text/TextUtils.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Text::TextUtils;
using Elastos::Core::EIID_ICloneable;
using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddress;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Net {

const String CVpnProfile::TAG("VpnProfile");

CAR_INTERFACE_IMPL_3(CVpnProfile, Object, IVpnProfile, ICloneable, IParcelable)

CAR_OBJECT_IMPL(CVpnProfile)

CVpnProfile::CVpnProfile()
    : mName("")
    , mType(IVpnProfile::TYPE_PPTP)
    , mServer("")
    , mUsername("")
    , mPassword("")
    , mDnsServers("")
    , mSearchDomains("")
    , mRoutes("")
    , mMppe(TRUE)
    , mL2tpSecret("")
    , mIpsecIdentifier("")
    , mIpsecSecret("")
    , mIpsecUserCert("")
    , mIpsecCaCert("")
    , mIpsecServerCert("")
    , mSaveLogin(FALSE)
{}

ECode CVpnProfile::constructor()
{
    return NOERROR;
}

ECode CVpnProfile::constructor(
    /* [in] */ const String& key)
{
    mKey = key;
    return NOERROR;
}

ECode CVpnProfile::Decode(
    /* [in] */ const String& key,
    /* [in] */ ArrayOf<Byte>* value,
    /* [out] */ IVpnProfile** result)
{
    VALIDATE_NOT_NULL(result);
    *result = NULL;
    VALIDATE_NOT_NULL(value)

    // try {
    if (key.IsNull()) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<String> > values;
    //TODO: String[] values = new String(value, StandardCharsets.UTF_8).split("\0", -1);
    StringUtils::Split(String(*value), "\0", -1, (ArrayOf<String>**)&values);
    // There can be 14 or 15 values in ICS MR1.
    if (values->GetLength() < 14 || values->GetLength() > 15) {
        return NOERROR;
    }

    AutoPtr<CVpnProfile> profile;
    CVpnProfile::NewByFriend(key, (CVpnProfile**)&profile);
    profile->mName = (*values)[0];
    profile->mType = StringUtils::ParseInt32((*values)[1]);
    if (profile->mType < 0 || profile->mType > TYPE_MAX) {
        return NOERROR;
    }
    profile->mServer = (*values)[2];
    profile->mUsername = (*values)[3];
    profile->mPassword = (*values)[4];
    profile->mDnsServers = (*values)[5];
    profile->mSearchDomains = (*values)[6];
    profile->mRoutes = (*values)[7];
    profile->mMppe = (*values)[8].EqualsIgnoreCase("TRUE");
    profile->mL2tpSecret = (*values)[9];
    profile->mIpsecIdentifier = (*values)[10];
    profile->mIpsecSecret = (*values)[11];
    profile->mIpsecUserCert = (*values)[12];
    profile->mIpsecCaCert = (*values)[13];
    profile->mIpsecServerCert = (values->GetLength() > 14) ? (*values)[14] : "";

    profile->mSaveLogin = !profile->mUsername.IsEmpty() || !profile->mPassword.IsEmpty();
    *result = profile;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CVpnProfile::Encode(
    /* [out, callee] */ ArrayOf<Byte>** value)
{
    VALIDATE_NOT_NULL(value)
    StringBuilder builder(mName);
    builder.Append("\0");
    builder.Append(mType);
    builder.Append("\0");
    builder.Append(mServer);
    builder.Append("\0");
    builder.Append(mSaveLogin ? mUsername : String(""));
    builder.Append("\0");
    builder.Append(mSaveLogin ? mPassword : String(""));
    builder.Append("\0");
    builder.Append(mDnsServers);
    builder.Append("\0");
    builder.Append(mSearchDomains);
    builder.Append("\0");
    builder.Append(mRoutes);
    builder.Append("\0");
    builder.Append(mMppe);
    builder.Append("\0");
    builder.Append(mL2tpSecret);
    builder.Append("\0");
    builder.Append(mIpsecIdentifier);
    builder.Append("\0");
    builder.Append(mIpsecSecret);
    builder.Append("\0");
    builder.Append(mIpsecUserCert);
    builder.Append("\0");
    builder.Append(mIpsecCaCert);
    builder.Append("\0");
    builder.Append(mIpsecServerCert);
    // TODO:
    // builder.toString().getBytes(StandardCharsets.UTF_8);
    AutoPtr<ArrayOf<Byte> > temp = builder.ToString().GetBytes();
    *value = temp;
    REFCOUNT_ADD(*value)
    return NOERROR;
}

ECode CVpnProfile::IsValidLockdownProfile(
    /* [out] */ Boolean* isValid)
{
    VALIDATE_NOT_NULL(isValid);
    *isValid = FALSE;

    AutoPtr<IInetAddressHelper> helper;
    CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&helper);
    AutoPtr<IInetAddress> address;
    AutoPtr<ArrayOf<String> > dnsArray;
    if (FAILED(helper->ParseNumericAddress(mServer, (IInetAddress**)&address)))
        goto _EXIT_;

    StringUtils::Split(mDnsServers, String(" +"), (ArrayOf<String>**)&dnsArray);
    for (Int32 i = 0; i < dnsArray->GetLength(); i++) {
        String dnsServer = (*dnsArray)[i];
        address = NULL;
        if (FAILED(helper->ParseNumericAddress(mDnsServers, (IInetAddress**)&address)))
            goto _EXIT_;
    }

    if (TextUtils::IsEmpty(mDnsServers)) {
        Logger::W(TAG, "DNS required");
        return NOERROR;
    }

    // Everything checked out above
    *isValid = TRUE;
    return NOERROR;

_EXIT_:
    Logger::W(TAG, "Invalid address");
    return NOERROR;
}

ECode CVpnProfile::GetKey(
    /* [out] */ String* key)
{
    VALIDATE_NOT_NULL(key);
    *key = mKey;
    return NOERROR;
}

ECode CVpnProfile::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    *name = mName;
    return NOERROR;
}

ECode CVpnProfile::SetName(
    /* [in] */ const String& name)
{
    mName = name;
    return NOERROR;
}

ECode CVpnProfile::GetType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mType;
    return NOERROR;
}

ECode CVpnProfile::SetType(
    /* [in] */ Int32 type)
{
    mType = type;
    return NOERROR;
}

ECode CVpnProfile::GetServer(
    /* [out] */ String* server)
{
    VALIDATE_NOT_NULL(server);
    *server = mServer;
    return NOERROR;
}

ECode CVpnProfile::SetServer(
    /* [in] */ const String& server)
{
    mServer = server;
    return NOERROR;
}

ECode CVpnProfile::GetUsername(
    /* [out] */ String* username)
{
    VALIDATE_NOT_NULL(username);
    *username = mUsername;
    return NOERROR;
}

ECode CVpnProfile::SetUsername(
    /* [in] */ const String& username)
{
    mUsername = username;
    return NOERROR;
}

ECode CVpnProfile::GetPassword(
    /* [out] */ String* password)
{
    VALIDATE_NOT_NULL(password);
    *password = mPassword;
    return NOERROR;
}

ECode CVpnProfile::SetPassword(
    /* [in] */ const String& password)
{
    mPassword = password;
    return NOERROR;
}

ECode CVpnProfile::GetDnsServers(
    /* [out] */ String* dnsServers)
{
    VALIDATE_NOT_NULL(dnsServers);
    *dnsServers = mDnsServers;
    return NOERROR;
}

ECode CVpnProfile::SetDnsServers(
    /* [in] */ const String& dnsServers)
{
    mDnsServers = dnsServers;
    return NOERROR;
}

ECode CVpnProfile::GetSearchDomains(
    /* [out] */ String* searchDomains)
{
    VALIDATE_NOT_NULL(searchDomains);
    *searchDomains = mSearchDomains;
    return NOERROR;
}

ECode CVpnProfile::SetSearchDomains(
    /* [in] */ const String& searchDomains)
{
    mSearchDomains = searchDomains;
    return NOERROR;
}

ECode CVpnProfile::GetRoutes(
    /* [out] */ String* routes)
{
    VALIDATE_NOT_NULL(routes);
    *routes = mRoutes;
    return NOERROR;
}

ECode CVpnProfile::SetRoutes(
    /* [in] */ const String& routes)
{
    mRoutes = routes;
    return NOERROR;
}

ECode CVpnProfile::GetMppe(
    /* [out] */ Boolean* mppe)
{
    VALIDATE_NOT_NULL(mppe);
    *mppe = mMppe;
    return NOERROR;
}

ECode CVpnProfile::SetMppe(
    /* [in] */ Boolean mppe)
{
    mMppe = mppe;
    return NOERROR;
}

ECode CVpnProfile::GetL2tpSecret(
    /* [out] */ String* l2tpSecret)
{
    VALIDATE_NOT_NULL(l2tpSecret);
    *l2tpSecret = mL2tpSecret;
    return NOERROR;
}

ECode CVpnProfile::SetL2tpSecret(
    /* [in] */ const String& l2tpSecret)
{
    mL2tpSecret = l2tpSecret;
    return NOERROR;
}

ECode CVpnProfile::GetIpsecIdentifier(
    /* [out] */ String* ipsecIdentifier)
{
    VALIDATE_NOT_NULL(ipsecIdentifier);
    *ipsecIdentifier = mIpsecIdentifier;
    return NOERROR;
}

ECode CVpnProfile::SetIpsecIdentifier(
    /* [in] */ const String& ipsecIdentifier)
{
    mIpsecIdentifier = ipsecIdentifier;
    return NOERROR;
}

ECode CVpnProfile::GetIpsecSecret(
    /* [out] */ String* ipsecSecret)
{
    VALIDATE_NOT_NULL(ipsecSecret);
    *ipsecSecret = mIpsecSecret;
    return NOERROR;
}

ECode CVpnProfile::SetIpsecSecret(
    /* [in] */ const String& ipsecSecret)
{
    mIpsecSecret = ipsecSecret;
    return NOERROR;
}

ECode CVpnProfile::GetIpsecUserCert(
    /* [out] */ String* ipsecUserCert)
{
    VALIDATE_NOT_NULL(ipsecUserCert);
    *ipsecUserCert = mIpsecUserCert;
    return NOERROR;
}

ECode CVpnProfile::SetIpsecUserCert(
    /* [in] */ const String& ipsecUserCert)
{
    mIpsecUserCert = ipsecUserCert;
    return NOERROR;
}

ECode CVpnProfile::GetIpsecCaCert(
    /* [out] */ String* ipsecCaCert)
{
    VALIDATE_NOT_NULL(ipsecCaCert);
    *ipsecCaCert = mIpsecCaCert;
    return NOERROR;
}

ECode CVpnProfile::SetIpsecCaCert(
    /* [in] */ const String& ipsecCaCert)
{
    mIpsecCaCert = ipsecCaCert;
    return NOERROR;
}

ECode CVpnProfile::GetIpsecServerCert(
    /* [out] */ String* ipsecServerCert)
{
    VALIDATE_NOT_NULL(ipsecServerCert);
    *ipsecServerCert = mIpsecServerCert;
    return NOERROR;
}

ECode CVpnProfile::SetIpsecServerCert(
    /* [in] */ const String& ipsecServerCert)
{
    mIpsecServerCert = ipsecServerCert;
    return NOERROR;
}

ECode CVpnProfile::GetSaveLogin(
    /* [out] */ Boolean* saveLogin)
{
    VALIDATE_NOT_NULL(saveLogin);
    *saveLogin = mSaveLogin;
    return NOERROR;
}

ECode CVpnProfile::SetSaveLogin(
    /* [in] */ Boolean saveLogin)
{
    mSaveLogin = saveLogin;
    return NOERROR;
}

ECode CVpnProfile::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);

    source->ReadString(&mKey);
    source->ReadString(&mName);
    source->ReadInt32(&mType);
    source->ReadString(&mServer);
    source->ReadString(&mUsername);
    source->ReadString(&mPassword);
    source->ReadString(&mDnsServers);
    source->ReadString(&mSearchDomains);
    source->ReadString(&mRoutes);
    source->ReadBoolean(&mMppe);
    source->ReadString(&mL2tpSecret);
    source->ReadString(&mIpsecIdentifier);
    source->ReadString(&mIpsecSecret);
    source->ReadString(&mIpsecUserCert);
    source->ReadString(&mIpsecCaCert);
    source->ReadString(&mIpsecServerCert);
    source->ReadBoolean(&mSaveLogin);
    return NOERROR;
}

ECode CVpnProfile::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);

    dest->WriteString(mKey);
    dest->WriteString(mName);
    dest->WriteInt32(mType);
    dest->WriteString(mServer);
    dest->WriteString(mUsername);
    dest->WriteString(mPassword);
    dest->WriteString(mDnsServers);
    dest->WriteString(mSearchDomains);
    dest->WriteString(mRoutes);
    dest->WriteBoolean(mMppe);
    dest->WriteString(mL2tpSecret);
    dest->WriteString(mIpsecIdentifier);
    dest->WriteString(mIpsecSecret);
    dest->WriteString(mIpsecUserCert);
    dest->WriteString(mIpsecCaCert);
    dest->WriteString(mIpsecServerCert);
    dest->WriteBoolean(mSaveLogin);
    return NOERROR;
}

ECode CVpnProfile::Clone(
    /* [out] */ IInterface** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<CVpnProfile> profile;
    CVpnProfile::NewByFriend((CVpnProfile**)&profile);
    profile->mKey = mKey;
    profile->mName = mName;
    profile->mType = mType;
    profile->mServer = mServer;
    profile->mUsername = mUsername;
    profile->mPassword = mPassword;
    profile->mDnsServers = mDnsServers;
    profile->mSearchDomains = mSearchDomains;
    profile->mRoutes = mRoutes;
    profile->mMppe = mMppe;
    profile->mL2tpSecret = mL2tpSecret;
    profile->mIpsecIdentifier = mIpsecIdentifier;
    profile->mIpsecSecret = mIpsecSecret;
    profile->mIpsecUserCert = mIpsecUserCert;
    profile->mIpsecCaCert = mIpsecCaCert;
    profile->mIpsecServerCert = mIpsecServerCert;
    profile->mSaveLogin = mSaveLogin;
    *object = TO_IINTERFACE(profile);
    REFCOUNT_ADD(*object)
    return NOERROR;
}

} // namespace Net
} // namespace Internal
} // namespace Droid
} // namespace Elastos
