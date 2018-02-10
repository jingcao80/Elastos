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

#include "elastos/droid/wifi/p2p/CWifiP2pConfig.h"
#include "elastos/droid/wifi/p2p/CWifiP2pGroup.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
#include "elastos/droid/wifi/CWpsInfo.h"

using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Wifi::CWpsInfo;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

CAR_INTERFACE_IMPL_2(CWifiP2pConfig, Object, IWifiP2pConfig, IParcelable)

CAR_OBJECT_IMPL(CWifiP2pConfig)

CWifiP2pConfig::CWifiP2pConfig()
    : mDeviceAddress("")
    , mGroupOwnerIntent(-1)
    , mNetId(IWifiP2pGroup::PERSISTENT_NET_ID)
{
}

ECode CWifiP2pConfig::constructor()
{
    //set defaults
    FAIL_RETURN(CWpsInfo::New((IWpsInfo**)&mWps));
    FAIL_RETURN(mWps->SetSetup(IWpsInfo::PBC));

    return NOERROR;
}

ECode CWifiP2pConfig::constructor(
    /* [in] */ const String& supplicantEvent)
{
    AutoPtr<ArrayOf<String> > tokens;
    FAIL_RETURN(StringUtils::Split(supplicantEvent, String(" "), (ArrayOf<String>**)&tokens));

    if (tokens == NULL || tokens->GetLength() < 2) {
        Slogger::E("WifiP2pConfig", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed supplicant event %s",
            supplicantEvent.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (!(*tokens)[0].Equals(String("P2P-GO-NEG-REQUEST"))) {
        Slogger::E("WifiP2pConfig", "E_ILLEGAL_ARGUMENT_EXCEPTION: Malformed supplicant event %s",
            supplicantEvent.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mDeviceAddress = (*tokens)[1];

    FAIL_RETURN(CWpsInfo::New((IWpsInfo**)&mWps));

    if (tokens->GetLength() > 2) {
        AutoPtr<ArrayOf<String> > nameVal;
        FAIL_RETURN(StringUtils::Split((*tokens)[2], String("="), (ArrayOf<String>**)&nameVal));
        if (nameVal == NULL || nameVal->GetLength() < 1) {
            Slogger::E("WifiP2pConfig", "E_ILLEGAL_ARGUMENT_EXCEPTION: %s", supplicantEvent.string());
            return E_ILLEGAL_ARGUMENT_EXCEPTION;
        }

        Int32 devPasswdId = 0;
        ECode ec = StringUtils::Parse((*nameVal)[1], 10, &devPasswdId);
        if (FAILED(ec)) {
            devPasswdId = 0;
        }

        //Based on definitions in wps/wps_defs.h
        switch (devPasswdId) {
            //DEV_PW_USER_SPECIFIED = 0x0001,
            case 0x01:
                FAIL_RETURN(mWps->SetSetup(IWpsInfo::DISPLAY));
                break;
            //DEV_PW_PUSHBUTTON = 0x0004,
            case 0x04:
                FAIL_RETURN(mWps->SetSetup(IWpsInfo::PBC));
                break;
            //DEV_PW_REGISTRAR_SPECIFIED = 0x0005
            case 0x05:
                FAIL_RETURN(mWps->SetSetup(IWpsInfo::KEYPAD));
                break;
            default:
                FAIL_RETURN(mWps->SetSetup(IWpsInfo::PBC));
                break;
        }
    }

    return NOERROR;
}

ECode CWifiP2pConfig::constructor(
    /* [in] */ Elastos::Droid::Wifi::P2p::IWifiP2pConfig* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetDeviceAddress(&mDeviceAddress));
        AutoPtr<IWpsInfo> temp;
        FAIL_RETURN(source->GetWps((IWpsInfo**)&temp));
        FAIL_RETURN(CWpsInfo::New(temp, (IWpsInfo**)&mWps));
        FAIL_RETURN(source->GetGroupOwnerIntent(&mGroupOwnerIntent));
        FAIL_RETURN(source->GetNetId(&mNetId));
    }

    return NOERROR;
}

ECode CWifiP2pConfig::GetDeviceAddress(
    /* [out] */ String* deviceAddress)
{
    VALIDATE_NOT_NULL(deviceAddress);

    *deviceAddress = mDeviceAddress;

    return NOERROR;
}

ECode CWifiP2pConfig::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    mDeviceAddress = deviceAddress;

    return NOERROR;
}

ECode CWifiP2pConfig::GetWps(
    /* [out] */ IWpsInfo** wpsInfo)
{
    VALIDATE_NOT_NULL(wpsInfo);

    *wpsInfo = mWps;
    REFCOUNT_ADD(*wpsInfo);

    return NOERROR;
}

ECode CWifiP2pConfig::SetWps(
    /* [in] */ IWpsInfo* wpsInfo)
{
    mWps = wpsInfo;

    return NOERROR;
}

ECode CWifiP2pConfig::GetGroupOwnerIntent(
    /* [out] */ Int32* intent)
{
    VALIDATE_NOT_NULL(intent);

    *intent = mGroupOwnerIntent;

    return NOERROR;
}

ECode CWifiP2pConfig::SetGroupOwnerIntent(
    /* [in] */ Int32 intent)
{
    mGroupOwnerIntent = intent;

    return NOERROR;
}

ECode CWifiP2pConfig::GetNetId(
    /* [out] */ Int32* netId)
{
    VALIDATE_NOT_NULL(netId);

    *netId = mNetId;

    return NOERROR;
}

ECode CWifiP2pConfig::SetNetId(
    /* [in] */ Int32 netId)
{
    mNetId = netId;

    return NOERROR;
}

ECode CWifiP2pConfig::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;
    sb += "\n address: ";
    sb += mDeviceAddress;
    sb += "\n wps: ";
    String temp;
    if (mWps) {
        assert(0);
        // TODO
        // mWps->ToString(&temp);
    }
    sb += temp;

    sb += "\n groupOwnerIntent: ";
    sb += mGroupOwnerIntent;
    sb += "\n persist: ";
    sb += mNetId;

    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pConfig::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    mWps = NULL;
    FAIL_RETURN(source->ReadString(&mDeviceAddress));
    FAIL_RETURN(source->ReadInterfacePtr((HANDLE*)&mWps));
    FAIL_RETURN(source->ReadInt32(&mGroupOwnerIntent));
    FAIL_RETURN(source->ReadInt32(&mNetId));
    return NOERROR;
}

ECode CWifiP2pConfig::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mDeviceAddress));
    FAIL_RETURN(dest->WriteInterfacePtr(mWps.Get()));
    FAIL_RETURN(dest->WriteInt32(mGroupOwnerIntent));
    FAIL_RETURN(dest->WriteInt32(mNetId));

    return NOERROR;
}

ECode CWifiP2pConfig::Invalidate()
{
    mDeviceAddress = "";
    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
