
#include "elastos/droid/systemui/statusbar/policy/NetworkController.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {
namespace Policy {

CAR_INTERFACE_IMPL(AccessPoint, Object, INetworkControllerAccessPoint);
AccessPoint::AccessPoint()
    : mNetworkId(0)
    , mIconId(0)
    , mIsConnected(FALSE)
    , mLevel(0)
{}

ECode AccessPoint::SetNetworkId(
    /* [in] */ Int32 id)
{
    mNetworkId = id;
    return NOERROR;
}

ECode AccessPoint::GetNetworkId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mNetworkId;
    return NOERROR;
}

ECode AccessPoint::SetIconId(
    /* [in] */ Int32 id)
{
    mIconId = id;
    return NOERROR;
}

ECode AccessPoint::GetIconId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIconId;
    return NOERROR;
}

ECode AccessPoint::SetSsid(
    /* [in] */ const String& ssid)
{
    mSsid = ssid;
    return NOERROR;
}

ECode AccessPoint::GetSsid(
    /* [out] */ String* ssid)
{
    VALIDATE_NOT_NULL(ssid);
    *ssid = mSsid;
    return NOERROR;
}

ECode AccessPoint::IsConnected(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mIsConnected;
    return NOERROR;
}

ECode AccessPoint::SetIsConnected(
    /* [in] */ Boolean connected)
{
    mIsConnected = connected;
    return NOERROR;
}

ECode AccessPoint::SetLevel(
    /* [in] */ Int32 level)
{
    mLevel = level;
    return NOERROR;
}

ECode AccessPoint::GetLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mLevel;
    return NOERROR;
}

CAR_INTERFACE_IMPL(DataUsageInfo, Object, INetworkControllerDataUsageInfo);
DataUsageInfo::DataUsageInfo()
    : mLimitLevel(0)
    , mWarningLevel(0)
    , mUsageLevel(0)
{}

ECode DataUsageInfo::SetCarrier(
    /* [in] */ const String& carrier)
{
    mCarrier = carrier;
    return NOERROR;
}

ECode DataUsageInfo::GetCarrier(
    /* [out] */ String* carrier)
{
    VALIDATE_NOT_NULL(carrier);
    *carrier = mCarrier;
    return NOERROR;
}

ECode DataUsageInfo::SetPeriod(
    /* [in] */ const String& period)
{
    mPeriod = period;
    return NOERROR;
}

ECode DataUsageInfo::GetPeriod(
    /* [out] */ String* period)
{
    VALIDATE_NOT_NULL(period);
    *period = mPeriod;
    return NOERROR;
}

ECode DataUsageInfo::SetLimitLevel(
    /* [in] */ Int64 level)
{
    mLimitLevel = level;
    return NOERROR;
}

ECode DataUsageInfo::GetLimitLevel(
    /* [out] */ Int64* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mLimitLevel;
    return NOERROR;
}

ECode DataUsageInfo::SetWarningLevel(
    /* [in] */ Int64 level)
{
    mWarningLevel = level;
    return NOERROR;
}

ECode DataUsageInfo::GetWarningLevel(
    /* [out] */ Int64* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mWarningLevel;
    return NOERROR;
}

ECode DataUsageInfo::SetUsageLevel(
    /* [in] */ Int64 level)
{
    mUsageLevel = level;
    return NOERROR;
}

ECode DataUsageInfo::GetUsageLevel(
    /* [out] */ Int64* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mUsageLevel;
    return NOERROR;
}

}// namespace Policy
}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
