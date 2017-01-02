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

#include "elastos/droid/hardware/display/WifiDisplaySessionInfo.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Display {

CAR_INTERFACE_IMPL_2(WifiDisplaySessionInfo, Object, IWifiDisplaySessionInfo, IParcelable)

WifiDisplaySessionInfo::WifiDisplaySessionInfo()
    : mClient(FALSE)
    , mSessionId(0)
    , mGroupId(NULL)
    , mPassphrase(NULL)
    , mIP(NULL)
{
}

ECode WifiDisplaySessionInfo::constructor()
{
    return constructor(TRUE, 0, String(""), String(""), String(""));
}

ECode WifiDisplaySessionInfo::constructor(
    /* [in] */ Boolean client,
    /* [in] */ Int32 session,
    /* [in] */ const String& group,
    /* [in] */ const String& pp,
    /* [in] */ const String& ip)
{
    mClient = client;
    mSessionId = session;
    mGroupId = group;
    mPassphrase = pp;
    mIP = ip;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::IsClient(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mClient;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::GetSessionId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mSessionId;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::GetGroupId(
    /* [out] */ String* gId)
{
    VALIDATE_NOT_NULL(gId);

    *gId = mGroupId;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::GetPassphrase(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mPassphrase;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::GetIP(
    /* [out] */ String* ip)
{
    VALIDATE_NOT_NULL(ip);

    *ip = mIP;
    return NOERROR;
}

ECode WifiDisplaySessionInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadBoolean(&mClient));
    FAIL_RETURN(source->ReadInt32(&mSessionId));
    FAIL_RETURN(source->ReadString(&mGroupId));
    FAIL_RETURN(source->ReadString(&mPassphrase));
    FAIL_RETURN(source->ReadString(&mIP));
    return NOERROR;
}

ECode WifiDisplaySessionInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteBoolean(mClient))
    FAIL_RETURN(dest->WriteInt32(mSessionId))
    FAIL_RETURN(dest->WriteString(mGroupId))
    FAIL_RETURN(dest->WriteString(mPassphrase))
    FAIL_RETURN(dest->WriteString(mIP))
    return NOERROR;
}

ECode WifiDisplaySessionInfo::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);

    StringBuilder sb;
    String tmp;
    sb += "WifiDisplaySessionInfo:";
    sb += "\n    Client/Owner: ";
    if (mClient) {
        sb += "Client";
    }
    else {
        sb += "Owner";
    }
    sb += "\n    GroupId: ";
    sb += mGroupId;
    sb += "\n    Passphrase: ";
    sb += mPassphrase;
    sb += "\n    SessionId: ";
    sb += StringUtils::ToString(mSessionId);
    sb += "\n    IP Address: ";
    sb += mIP;

    return sb.ToString(result);
}

} // namespace Display
} // namespace Hardware
} // namepsace Droid
} // namespace Elastos