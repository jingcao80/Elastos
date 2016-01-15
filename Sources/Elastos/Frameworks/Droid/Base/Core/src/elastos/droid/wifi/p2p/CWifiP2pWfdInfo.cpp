
#include "elastos/droid/wifi/p2p/CWifiP2pWfdInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {

const  String CWifiP2pWfdInfo::TAG("WifiP2pWfdInfo");

/* Device information bitmap */
/** One of {@link #WFD_SOURCE}, {@link #PRIMARY_SINK}, {@link #SECONDARY_SINK}
 * or {@link #SOURCE_OR_PRIMARY_SINK}
 */
const  Int32 CWifiP2pWfdInfo::DEVICE_TYPE;
const  Int32 CWifiP2pWfdInfo::COUPLED_SINK_SUPPORT_AT_SOURCE;
const  Int32 CWifiP2pWfdInfo::COUPLED_SINK_SUPPORT_AT_SINK;
const  Int32 CWifiP2pWfdInfo::SESSION_AVAILABLE;
const  Int32 CWifiP2pWfdInfo::SESSION_AVAILABLE_BIT1;
const  Int32 CWifiP2pWfdInfo::SESSION_AVAILABLE_BIT2;

CAR_INTERFACE_IMPL_2(CWifiP2pWfdInfo, Object, IWifiP2pWfdInfo, IParcelable)

CAR_OBJECT_IMPL(CWifiP2pWfdInfo)

CWifiP2pWfdInfo::CWifiP2pWfdInfo()
    : mDeviceInfo(0)
    , mCtrlPort(0)
    , mMaxThroughput(0)
    , mWfdEnabled(FALSE)
{
}

ECode CWifiP2pWfdInfo::constructor()
{
    return NOERROR;
}

ECode CWifiP2pWfdInfo::constructor(
    /* [in] */ Int32 devInfo,
    /* [in] */ Int32 ctrlPort,
    /* [in] */ Int32 maxTput)
{
    mWfdEnabled = TRUE;
    mDeviceInfo = devInfo;
    mCtrlPort = ctrlPort;
    mMaxThroughput = maxTput;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::constructor(
    /* [in] */ IWifiP2pWfdInfo* source)
{
    if (source != NULL) {
        CWifiP2pWfdInfo* obj = (CWifiP2pWfdInfo*)source;
        mWfdEnabled = obj->mWfdEnabled;
        mDeviceInfo = obj->mDeviceInfo;
        mCtrlPort = obj->mCtrlPort;
        mMaxThroughput = obj->mMaxThroughput;
    }

    return NOERROR;
}

ECode CWifiP2pWfdInfo::IsWfdEnabled(
    /* [out] */ Boolean* isWfdEnabled)
{
    VALIDATE_NOT_NULL(isWfdEnabled);

    *isWfdEnabled = mWfdEnabled;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetWfdEnabled(
    /* [in] */ Boolean enabled)
{
    mWfdEnabled = enabled;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::GetDeviceType(
    /* [out] */ Int32* deviceType)
{
    VALIDATE_NOT_NULL(deviceType);

    *deviceType = (mDeviceInfo & DEVICE_TYPE);

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetDeviceType(
    /* [in] */ Int32 deviceType,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    if (deviceType >= WFD_SOURCE && deviceType <= SOURCE_OR_PRIMARY_SINK) {
        mDeviceInfo |= deviceType;
        *result = TRUE;
    }

    return NOERROR;
}

ECode CWifiP2pWfdInfo::IsCoupledSinkSupportedAtSource(
    /* [out] */ Boolean* isCoupledSinkSupportedAtSource)
{
    VALIDATE_NOT_NULL(isCoupledSinkSupportedAtSource);

    *isCoupledSinkSupportedAtSource =
        ((mDeviceInfo & COUPLED_SINK_SUPPORT_AT_SINK) != 0);

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetCoupledSinkSupportAtSource(
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        mDeviceInfo |= COUPLED_SINK_SUPPORT_AT_SINK;
    }
    else {
        mDeviceInfo &= ~COUPLED_SINK_SUPPORT_AT_SINK;
    }

    return NOERROR;
}

ECode CWifiP2pWfdInfo::IsCoupledSinkSupportedAtSink(
    /* [out] */ Boolean* isCoupledSinkSupportedAtSink)
{
    VALIDATE_NOT_NULL(isCoupledSinkSupportedAtSink);

    *isCoupledSinkSupportedAtSink =
        ((mDeviceInfo & COUPLED_SINK_SUPPORT_AT_SINK) != 0);

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetCoupledSinkSupportAtSink(
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        mDeviceInfo |= COUPLED_SINK_SUPPORT_AT_SINK;
    }
    else {
        mDeviceInfo &= ~COUPLED_SINK_SUPPORT_AT_SINK;
    }

    return NOERROR;
}

ECode CWifiP2pWfdInfo::IsSessionAvailable(
    /* [out] */ Boolean* isSessionAvailable)
{
    VALIDATE_NOT_NULL(isSessionAvailable);

    *isSessionAvailable = ((mDeviceInfo & SESSION_AVAILABLE) != 0);

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetSessionAvailable(
    /* [in] */ Boolean enabled)
{
    if (enabled) {
        mDeviceInfo |= SESSION_AVAILABLE_BIT1;
        mDeviceInfo &= ~SESSION_AVAILABLE_BIT2;
    }
    else {
        mDeviceInfo &= ~SESSION_AVAILABLE;
    }

    return NOERROR;
}

ECode CWifiP2pWfdInfo::GetControlPort(
    /* [out] */ Int32* controlPort)
{
    VALIDATE_NOT_NULL(controlPort);

    *controlPort = mCtrlPort;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetControlPort(
    /* [in] */ Int32 port)
{
    mCtrlPort = port;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::SetMaxThroughput(
    /* [in] */ Int32 maxThroughput)
{
    mMaxThroughput = maxThroughput;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::GetMaxThroughput(
    /* [out] */ Int32* maxThroughput)
{
    VALIDATE_NOT_NULL(maxThroughput);

    *maxThroughput = mMaxThroughput;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::GetDeviceInfoHex(
    /* [out] */ String* deviceInfoHex)
{
    VALIDATE_NOT_NULL(deviceInfoHex);

    String s;
    assert(0);
    // TODO
    // FAIL_RETURN(s.AppendFormat(
    //     Locale.US, "%04x%04x%04x%04x", 6, mDeviceInfo, mCtrlPort, mMaxThroughput));
    *deviceInfoHex = s;

    return NOERROR;
}

ECode CWifiP2pWfdInfo::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;
    sb += " {\n   WFD enabled: ";
    sb += mWfdEnabled;
    sb += "\n   WFD DeviceInfo: ";
    sb += mDeviceInfo;
    sb += ", 0X";
    sb += StringUtils::ToHexString(mDeviceInfo);
    sb += "\n   WFD CtrlPort: ";
    sb += mCtrlPort;
    sb += "\n   WFD MaxThroughput: ";
    sb += mMaxThroughput;
    sb += "\n }";
    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pWfdInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    Int32 temp;
    FAIL_RETURN(source->ReadInt32(&temp));
    mWfdEnabled = (temp == 1);
    FAIL_RETURN(source->ReadInt32(&mDeviceInfo));
    FAIL_RETURN(source->ReadInt32(&mCtrlPort));
    FAIL_RETURN(source->ReadInt32(&mMaxThroughput));

    return NOERROR;
}

ECode CWifiP2pWfdInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    VALIDATE_NOT_NULL(dest);

    FAIL_RETURN(dest->WriteInt32(mWfdEnabled ? 1 : 0));
    FAIL_RETURN(dest->WriteInt32(mDeviceInfo));
    FAIL_RETURN(dest->WriteInt32(mCtrlPort));
    FAIL_RETURN(dest->WriteInt32(mMaxThroughput));

    return NOERROR;
}

} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
