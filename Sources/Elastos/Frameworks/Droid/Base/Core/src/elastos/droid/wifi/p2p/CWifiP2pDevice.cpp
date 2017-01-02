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

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include "elastos/droid/wifi/p2p/CWifiP2pWfdInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuffer.h>
#include <elastos/utility/logging/Slogger.h>
#include <Elastos.CoreLibrary.h>
#include <unistd.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuffer;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPatternHelper;
using Elastos::Utility::Regex::CPatternHelper;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

static Boolean DBG = FALSE;

// These definitions match the ones in wpa_supplicant
/* WPS config methods supported */
const Int32 CWifiP2pDevice::WPS_CONFIG_DISPLAY;
const Int32 CWifiP2pDevice::WPS_CONFIG_PUSHBUTTON;
const Int32 CWifiP2pDevice::WPS_CONFIG_KEYPAD;

/* Device Capability bitmap */
const Int32 CWifiP2pDevice::DEVICE_CAPAB_SERVICE_DISCOVERY;
const Int32 CWifiP2pDevice::DEVICE_CAPAB_CLIENT_DISCOVERABILITY;
const Int32 CWifiP2pDevice::DEVICE_CAPAB_CONCURRENT_OPER;
const Int32 CWifiP2pDevice::DEVICE_CAPAB_INFRA_MANAGED;
const Int32 CWifiP2pDevice::DEVICE_CAPAB_DEVICE_LIMIT;
const Int32 CWifiP2pDevice::DEVICE_CAPAB_INVITATION_PROCEDURE;

/* Group Capability bitmap */
const Int32 CWifiP2pDevice::GROUP_CAPAB_GROUP_OWNER;
const Int32 CWifiP2pDevice::GROUP_CAPAB_PERSISTENT_GROUP;
const Int32 CWifiP2pDevice::GROUP_CAPAB_GROUP_LIMIT;
const Int32 CWifiP2pDevice::GROUP_CAPAB_INTRA_BSS_DIST;
const Int32 CWifiP2pDevice::GROUP_CAPAB_CROSS_CONN;
const Int32 CWifiP2pDevice::GROUP_CAPAB_PERSISTENT_RECONN;
const Int32 CWifiP2pDevice::GROUP_CAPAB_GROUP_FORMATION;

static AutoPtr<IPattern> InitPattern(
    /* [in] */ const String& p)
{
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    helper->Compile(p, (IPattern**)&pattern);
    return pattern;
}

AutoPtr<IPattern> CWifiP2pDevice::mDetailedDevicePattern
    = InitPattern(String("((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) (\\d+ )?p2p_dev_addr=((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) pri_dev_type=(\\d+-[0-9a-fA-F]+-\\d+) name='(.*)' config_methods=(0x[0-9a-fA-F]+) dev_capab=(0x[0-9a-fA-F]+) group_capab=(0x[0-9a-fA-F]+)( wfd_dev_info=0x([0-9a-fA-F]{12}))?"));
AutoPtr<IPattern> CWifiP2pDevice::mTwoTokenPattern
    = InitPattern(String("(p2p_dev_addr=)?((?:[0-9a-f]{2}:){5}[0-9a-f]{2})"));
AutoPtr<IPattern> CWifiP2pDevice::mThreeTokenPattern
    = InitPattern(String("(?:[0-9a-f]{2}:){5}[0-9a-f]{2} p2p_dev_addr=((?:[0-9a-f]{2}:){5}[0-9a-f]{2})"));
AutoPtr<IPattern> CWifiP2pDevice::mFourTokenPattern
    = InitPattern(String("(?:[0-9a-f]{2}:){5}[0-9a-f]{2} p2p_dev_addr=((?:[0-9a-f]{2}:){5}[0-9a-f]{2}) wfd_dev_info=0x000006([0-9a-fA-F]{12})"));

CAR_INTERFACE_IMPL_2(CWifiP2pDevice, Object, IWifiP2pDevice, IParcelable)

CAR_OBJECT_IMPL(CWifiP2pDevice)

CWifiP2pDevice::CWifiP2pDevice()
    : mDeviceName("")
    , mDeviceAddress("")
    , mWpsConfigMethodsSupported(0)
    , mDeviceCapability(0)
    , mGroupCapability(0)
    , mStatus(0)
{
}

ECode CWifiP2pDevice::GetDeviceName(
    /* [out] */ String* deviceName)
{
    VALIDATE_NOT_NULL(deviceName);

    *deviceName = mDeviceName;

    return NOERROR;
}

ECode CWifiP2pDevice::SetDeviceName(
    /* [in] */ const String& deviceName)
{
    mDeviceName = deviceName;

    return NOERROR;
}

ECode CWifiP2pDevice::GetDeviceAddress(
    /* [out] */ String* deviceAddress)
{
    VALIDATE_NOT_NULL(deviceAddress);

    *deviceAddress = mDeviceAddress;

    return NOERROR;
}

ECode CWifiP2pDevice::SetDeviceAddress(
    /* [in] */ const String& deviceAddress)
{
    mDeviceAddress = deviceAddress;

    return NOERROR;
}

ECode CWifiP2pDevice::GetPrimaryDeviceType(
    /* [out] */ String* primaryDeviceType)
{
    VALIDATE_NOT_NULL(primaryDeviceType);

    *primaryDeviceType = mPrimaryDeviceType;

    return NOERROR;
}

ECode CWifiP2pDevice::SetPrimaryDeviceType(
    /* [in] */ const String& primaryDeviceType)
{
    mPrimaryDeviceType = primaryDeviceType;

    return NOERROR;
}

ECode CWifiP2pDevice::GetSecondaryDeviceType(
    /* [out] */ String* secondaryDeviceType)
{
    VALIDATE_NOT_NULL(secondaryDeviceType);

    *secondaryDeviceType = mSecondaryDeviceType;

    return NOERROR;
}

ECode CWifiP2pDevice::SetSecondaryDeviceType(
    /* [in] */ const String& secondaryDeviceType)
{
    mSecondaryDeviceType = secondaryDeviceType;

    return NOERROR;
}

ECode CWifiP2pDevice::GetWpsConfigMethodsSupported(
    /* [out] */ Int32* wpsConfigMethodsSupported)
{
    VALIDATE_NOT_NULL(wpsConfigMethodsSupported);

    *wpsConfigMethodsSupported = mWpsConfigMethodsSupported;

    return NOERROR;
}

ECode CWifiP2pDevice::SetWpsConfigMethodsSupported(
    /* [in] */ Int32 wpsConfigMethodsSupported)
{
    mWpsConfigMethodsSupported = wpsConfigMethodsSupported;
    if (DBG) {
        Slogger::D("WifiP2pDevice", "SetWpsConfigMethodsSupported : %d", mWpsConfigMethodsSupported);
    }
    return NOERROR;
}

ECode CWifiP2pDevice::GetDeviceCapability(
    /* [out] */ Int32* deviceCapability)
{
    VALIDATE_NOT_NULL(deviceCapability);

    *deviceCapability = mDeviceCapability;

    return NOERROR;
}

ECode CWifiP2pDevice::SetDeviceCapability(
    /* [in] */ Int32 deviceCapability)
{
    mDeviceCapability = deviceCapability;
    if (DBG) {
        Slogger::D("WifiP2pDevice", "SetDeviceCapability : %d", deviceCapability);
    }
    return NOERROR;
}

ECode CWifiP2pDevice::GetGroupCapability(
    /* [out] */ Int32* groupCapability)
{
    VALIDATE_NOT_NULL(groupCapability);

    *groupCapability = mGroupCapability;

    return NOERROR;
}

ECode CWifiP2pDevice::SetGroupCapability(
    /* [in] */ Int32 groupCapability)
{
    mGroupCapability = groupCapability;

    return NOERROR;
}

ECode CWifiP2pDevice::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = mStatus;

    return NOERROR;
}

ECode CWifiP2pDevice::SetStatus(
    /* [in] */ Int32 status)
{
    mStatus = status;

    return NOERROR;
}

ECode CWifiP2pDevice::GetWfdInfo(
    /* [out] */ IWifiP2pWfdInfo** wfdInfo)
{
    VALIDATE_NOT_NULL(wfdInfo);

    *wfdInfo = mWfdInfo;
    REFCOUNT_ADD(*wfdInfo);

    return NOERROR;
}

ECode CWifiP2pDevice::SetWfdInfo(
    /* [in] */ IWifiP2pWfdInfo* wfdInfo)
{
    mWfdInfo = wfdInfo;

    return NOERROR;
}

ECode CWifiP2pDevice::WpsPbcSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = ((mWpsConfigMethodsSupported & WPS_CONFIG_PUSHBUTTON) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::WpsKeypadSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = ((mWpsConfigMethodsSupported & WPS_CONFIG_KEYPAD) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::WpsDisplaySupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = ((mWpsConfigMethodsSupported & WPS_CONFIG_DISPLAY) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::IsServiceDiscoveryCapable(
    /* [out] */ Boolean* capable)
{
    VALIDATE_NOT_NULL(capable);

    *capable = ((mDeviceCapability & DEVICE_CAPAB_SERVICE_DISCOVERY) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::IsInvitationCapable(
    /* [out] */ Boolean* capable)
{
    VALIDATE_NOT_NULL(capable);

    *capable = ((mDeviceCapability & DEVICE_CAPAB_INVITATION_PROCEDURE) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::IsDeviceLimit(
    /* [out] */ Boolean* isDeviceLimit)
{
    VALIDATE_NOT_NULL(isDeviceLimit);

    *isDeviceLimit = ((mDeviceCapability & DEVICE_CAPAB_DEVICE_LIMIT) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::IsGroupOwner(
    /* [out] */ Boolean* isGroupOwner)
{
    VALIDATE_NOT_NULL(isGroupOwner);

    *isGroupOwner = ((mGroupCapability & GROUP_CAPAB_GROUP_OWNER) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::IsGroupLimit(
    /* [out] */ Boolean* isGroupLimit)
{
    VALIDATE_NOT_NULL(isGroupLimit);

    *isGroupLimit = ((mGroupCapability & GROUP_CAPAB_GROUP_LIMIT) != 0);

    return NOERROR;
}

ECode CWifiP2pDevice::Update(
    /* [in] */ IWifiP2pDevice* device)
{
    UpdateSupplicantDetails(device);
    return device->GetStatus(&mStatus);
}

ECode CWifiP2pDevice::UpdateSupplicantDetails(
    /* [in] */ IWifiP2pDevice* device)
{
    if (device == NULL) {
        // throw new IllegalArgumentException("device is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    String deviceAddress;
    device->GetDeviceAddress(&deviceAddress);
    if (deviceAddress == NULL) {
        // throw new IllegalArgumentException("deviceAddress is null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!mDeviceAddress.Equals(deviceAddress)) {
        // throw new IllegalArgumentException("deviceAddress does not match");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    FAIL_RETURN(device->GetDeviceName(&mDeviceName));
    FAIL_RETURN(device->GetPrimaryDeviceType(&mPrimaryDeviceType));
    FAIL_RETURN(device->GetSecondaryDeviceType(&mSecondaryDeviceType));
    FAIL_RETURN(device->GetWpsConfigMethodsSupported(&mWpsConfigMethodsSupported));
    FAIL_RETURN(device->GetDeviceCapability(&mDeviceCapability));
    FAIL_RETURN(device->GetGroupCapability(&mGroupCapability));

    AutoPtr<IWifiP2pWfdInfo> wfdInfo;
    FAIL_RETURN(device->GetWfdInfo((IWifiP2pWfdInfo**)&wfdInfo));
    mWfdInfo = wfdInfo;

    if (DBG) {
        String temp;
        ToString(&temp);
        Slogger::D("WifiP2pDevice", "Update : %s", temp.string());
    }
    return NOERROR;
}

ECode CWifiP2pDevice::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    if (TO_IINTERFACE(this) == obj) {
        *isEqual = TRUE;
        return NOERROR;
    }

    IWifiP2pDevice* other = IWifiP2pDevice::Probe(obj);
    if (other == NULL) {
        *isEqual = FALSE;
        return NOERROR;
    }

    String oa;
    other->GetDeviceAddress(&oa);
    if (oa.IsNull()) {
        *isEqual = mDeviceAddress.IsNull();
        return NOERROR;
    }

    *isEqual = oa.Equals(mDeviceAddress);
    return NOERROR;
}

ECode CWifiP2pDevice::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuffer sbuf;
    sbuf.Append("Device: ");
    sbuf.Append(mDeviceName);
    sbuf.Append("\n deviceAddress: ");
    sbuf.Append(mDeviceAddress);
    sbuf.Append("\n primary type: ");
    sbuf.Append(mPrimaryDeviceType);
    sbuf.Append("\n secondary type: ");
    sbuf.Append(mSecondaryDeviceType);
    sbuf.Append("\n wps: ");
    sbuf.Append(mWpsConfigMethodsSupported);
    sbuf.Append("\n grpcapab: ");
    sbuf.Append(mGroupCapability);
    sbuf.Append("\n devcapab: ");
    sbuf.Append(mDeviceCapability);
    sbuf.Append("\n status: ");
    sbuf.Append(mStatus);
    sbuf.Append("\n wfdInfo:");
    String temp("NULL");
    if (mWfdInfo != NULL) {
        assert(0);
        // TODO
        // FAIL_RETURN(mWfdInfo->ToString(&temp));
    }
    sbuf.Append(temp);
    *str = sbuf.ToString();
    return NOERROR;
}

ECode CWifiP2pDevice::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    FAIL_RETURN(source->ReadString(&mDeviceName));
    FAIL_RETURN(source->ReadString(&mDeviceAddress));
    FAIL_RETURN(source->ReadString(&mPrimaryDeviceType));
    FAIL_RETURN(source->ReadString(&mSecondaryDeviceType));
    FAIL_RETURN(source->ReadInt32(&mWpsConfigMethodsSupported));
    FAIL_RETURN(source->ReadInt32(&mDeviceCapability));
    FAIL_RETURN(source->ReadInt32(&mGroupCapability));
    FAIL_RETURN(source->ReadInt32(&mStatus));
    Int32 temp;
    FAIL_RETURN(source->ReadInt32(&temp));
    mWfdInfo = NULL;
    if (temp == 1) {
        FAIL_RETURN(source->ReadInterfacePtr((Handle32*)&mWfdInfo));
    }

    return NOERROR;
}

ECode CWifiP2pDevice::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteString(mDeviceName));
    FAIL_RETURN(dest->WriteString(mDeviceAddress));
    FAIL_RETURN(dest->WriteString(mPrimaryDeviceType));
    FAIL_RETURN(dest->WriteString(mSecondaryDeviceType));
    FAIL_RETURN(dest->WriteInt32(mWpsConfigMethodsSupported));
    FAIL_RETURN(dest->WriteInt32(mDeviceCapability));
    FAIL_RETURN(dest->WriteInt32(mGroupCapability));
    FAIL_RETURN(dest->WriteInt32(mStatus));
    if (mWfdInfo != NULL) {
        FAIL_RETURN(dest->WriteInt32(1));
        FAIL_RETURN(dest->WriteInterfacePtr(mWfdInfo.Get()));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(0));
    }

    return NOERROR;
}

ECode CWifiP2pDevice::constructor()
{
    return NOERROR;
}

ECode CWifiP2pDevice::constructor(
    /* [in] */ const String& dataString)
{
    if (DBG) Slogger::D("WifiP2pDevice", "new CWifiP2pDevice: [%s]", dataString.string());
    AutoPtr<IPatternHelper> helper;
    CPatternHelper::AcquireSingleton((IPatternHelper**)&helper);
    AutoPtr<IPattern> pattern;
    FAIL_RETURN(helper->Compile(String("[ \n]"), (IPattern**)&pattern));

    AutoPtr<ArrayOf<String> > tokens;
    FAIL_RETURN(pattern->Split(dataString, (ArrayOf<String>**)&tokens));

    AutoPtr<IMatcher> match;
    Boolean ret = FALSE;

    if ((tokens == NULL) || (tokens->GetLength() < 1)) {
        Slogger::E("WifiP2pDevice", "E_ILLEGAL_ARGUMENT_EXCEPTION:Malformed supplicant event %s", dataString.string());
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (DBG) {
        Slogger::D("WifiP2pDevice", "new CWifiP2pDevice: tokens length: %d", tokens->GetLength());
        for (Int32 i = 0; i < tokens->GetLength(); ++i) {
            Slogger::D("WifiP2pDevice", " > token %d: [%s]", i, (*tokens)[i].string());
        }
    }

    switch (tokens->GetLength()) {
        case 1:
            /* Just a device address */
            mDeviceAddress = dataString;
            return NOERROR;
        case 2:
            FAIL_RETURN(mTwoTokenPattern->Matcher(dataString, (IMatcher**)&match));
            FAIL_RETURN(match->Find(&ret));
            if (!ret) {
                Slogger::E("WifiP2pDevice", "E_ILLEGAL_ARGUMENT_EXCEPTION:Malformed supplicant event %s", dataString.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(2, &mDeviceAddress));
            return NOERROR;
        case 3:
            FAIL_RETURN(mThreeTokenPattern->Matcher(dataString, (IMatcher**)&match));
            FAIL_RETURN(match->Find(&ret));
            if (!ret) {
                Slogger::E("WifiP2pDevice", "E_ILLEGAL_ARGUMENT_EXCEPTION:Malformed supplicant event %s", dataString.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(1, &mDeviceAddress));
            return NOERROR;
        default:
            FAIL_RETURN(mDetailedDevicePattern->Matcher(dataString, (IMatcher**)&match));
            FAIL_RETURN(match->Find(&ret));
            if (!ret) {
                Slogger::E("WifiP2pDevice", "E_ILLEGAL_ARGUMENT_EXCEPTION:Malformed supplicant event %s", dataString.string());
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(3, &mDeviceAddress));
            // FAIL_RETURN(match->Group(4, &mPrimaryDeviceType));
            // FAIL_RETURN(match->Group(5, &mDeviceName));

            String temp;
            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(6, &temp));
            mWpsConfigMethodsSupported = ParseHex(temp);
            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(7, &temp));
            mDeviceCapability = ParseHex(temp);
            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(8, &temp));
            mGroupCapability = ParseHex(temp);

            assert(0);
            // TODO
            // FAIL_RETURN(match->Group(9, &temp));
            if (!temp.IsNull()) {
                String str;
                assert(0);
                // TODO
                // FAIL_RETURN(match->Group(10, &str));

                String str1 = str.Substring(0, 4);
                String str2 = str.Substring(4, 8);
                String str3 = str.Substring(8, 12);
                Int32 a = ParseHex(str1);
                Int32 b = ParseHex(str2);
                Int32 c = ParseHex(str3);
                FAIL_RETURN(CWifiP2pWfdInfo::New(a, b, c, (IWifiP2pWfdInfo**)&mWfdInfo));
            }
            break;
    }

    if ((*tokens)[0].StartWith("P2P-DEVICE-FOUND")) {
        mStatus = IWifiP2pDevice::AVAILABLE;
    }

    if (DBG) {
        String info;
        ToString(&info);
        Slogger::D("WifiP2pDevice", "new CWifiP2pDevice:\n %s", info.string());
    }
    return NOERROR;
}

ECode CWifiP2pDevice::constructor(
    /* [in] */ IWifiP2pDevice* source)
{
    if (source != NULL) {
        FAIL_RETURN(source->GetDeviceName(&mDeviceName));
        FAIL_RETURN(source->GetDeviceAddress(&mDeviceAddress));
        FAIL_RETURN(source->GetPrimaryDeviceType(&mPrimaryDeviceType));
        FAIL_RETURN(source->GetSecondaryDeviceType(&mSecondaryDeviceType));
        FAIL_RETURN(source->GetWpsConfigMethodsSupported(&mWpsConfigMethodsSupported));
        FAIL_RETURN(source->GetDeviceCapability(&mDeviceCapability));
        FAIL_RETURN(source->GetGroupCapability(&mGroupCapability));
        FAIL_RETURN(source->GetStatus(&mStatus));
        AutoPtr<IWifiP2pWfdInfo> wfdInfo;
        FAIL_RETURN(source->GetWfdInfo((IWifiP2pWfdInfo**)&wfdInfo));
        CWifiP2pWfdInfo::New(wfdInfo, (IWifiP2pWfdInfo**)&mWfdInfo);

        if (DBG) {
            String info;
            ToString(&info);
            Slogger::D("WifiP2pDevice", "new CWifiP2pDevice copy from other device:\n %s", info.string());
        }
    }

    return NOERROR;
}

//supported formats: 0x1abc, 0X1abc, 1abc
Int32 CWifiP2pDevice::ParseHex(
    /* [in] */ const String& str)
{
    Int32 num = 0;
    String hexStr = str;
    if (hexStr.StartWith("0x") || hexStr.StartWith("0X")) {
        hexStr = str.Substring(2);
    }

    // try {
    ECode ec = StringUtils::Parse(hexStr, 16, &num);
    if (FAILED(ec)) {
        Slogger::E("WifiP2pDevice", "Failed to parse hex string %s", str.string());
    }
    // } catch(NumberFormatException e) {
    //     Log.e(TAG, "Failed to parse hex string " + hexString);
    // }

    return num;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
