
#include "elastos/droid/wifi/CWifiConfigurationHelper.h"
#include "elastos/droid/wifi/CWifiConfigurationVisibility.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL(CWifiConfigurationVisibility, Object, IWifiConfigurationVisibility)

CAR_OBJECT_IMPL(CWifiConfigurationVisibility)

CWifiConfigurationVisibility::CWifiConfigurationVisibility()
    : mRssi5(0)
    , mRssi24(0)
    , mNum5(0)
    , mNum24(0)
    , mAge5(0)
    , mAge24(0)
{
}

ECode CWifiConfigurationVisibility::constructor()
{
    AutoPtr<IWifiConfigurationHelper> helper;
    CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&helper);
    Int32 INVALID_RSSI;
    helper->GetINVALID_RSSI(&INVALID_RSSI);
    mRssi5 = INVALID_RSSI;
    mRssi24 = INVALID_RSSI;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::constructor(
    /* [in] */ IWifiConfigurationVisibility* source)
{
    source->GetRssi5(&mRssi5);
    source->GetRssi24(&mRssi24);
    source->GetAge24(&mAge24);
    source->GetAge5(&mAge5);
    source->GetNum24(&mNum24);
    source->GetNum5(&mNum5);
    source->GetBSSID5(&mBSSID5);
    source->GetBSSID24(&mBSSID24);
    return NOERROR;
}

ECode CWifiConfigurationVisibility::GetRssi5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssi5;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetRssi5(
    /* [in] */ Int32 rssi5)
{
    mRssi5 = rssi5;
    return NOERROR;
}

// strongest 5GHz RSSI
ECode CWifiConfigurationVisibility::GetRssi24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mRssi24;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetRssi24(
    /* [in] */ Int32 rssi24)
{
    mRssi24 = rssi24;
    return NOERROR;
}

// strongest 2.4GHz RSSI
ECode CWifiConfigurationVisibility::GetNum5(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNum5;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetNum5(
    /* [in] */ Int32 num5)
{
    mNum5 = num5;
    return NOERROR;
}

// number of BSSIDs on 5GHz
ECode CWifiConfigurationVisibility::GetNum24(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mNum24;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetNum24(
    /* [in] */ Int32 num24)
{
    mNum24 = num24;
    return NOERROR;
}

// number of BSSIDs on 2.4GHz
ECode CWifiConfigurationVisibility::GetAge5(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAge5;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetAge5(
    /* [in] */ Int64 age5)
{
    mAge5 = age5;
    return NOERROR;
}

// timestamp of the strongest 5GHz BSSID (last time it was seen)
ECode CWifiConfigurationVisibility::GetAge24(
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mAge24;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetAge24(
    /* [in] */ Int64 age24)
{
    mAge24 = age24;
    return NOERROR;
}

// timestamp of the strongest 2.4GHz BSSID (last time it was seen)
ECode CWifiConfigurationVisibility::GetBSSID24(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBSSID24;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetBSSID24(
    /* [in] */ const String& BSSID24)
{
    mBSSID24 = BSSID24;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::GetBSSID5(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mBSSID5;
    return NOERROR;
}

ECode CWifiConfigurationVisibility::SetBSSID5(
    /* [in] */ const String& BSSID5)
{
    mBSSID5 = BSSID5;
    return NOERROR;
}

//@Override
ECode CWifiConfigurationVisibility::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sbuf;
    sbuf.Append("[");
    AutoPtr<IWifiConfigurationHelper> helper;
    CWifiConfigurationHelper::AcquireSingleton((IWifiConfigurationHelper**)&helper);
    Int32 INVALID_RSSI;
    helper->GetINVALID_RSSI(&INVALID_RSSI);
    if (mRssi24 > INVALID_RSSI) {
        sbuf.Append(mRssi24);
        sbuf.Append(",");
        sbuf.Append(mNum24);
        if (mBSSID24 != NULL) {
            sbuf.Append(",");
            sbuf.Append(mBSSID24);
        }
    }
    else {
        sbuf.Append("*");
    }
    sbuf.Append(" - ");
    if (mRssi5 > INVALID_RSSI) {
        sbuf.Append(mRssi5);
        sbuf.Append(",");
        sbuf.Append(mNum5);
        if (mBSSID5 != NULL) {
            sbuf.Append(",");
            sbuf.Append(mBSSID5);
        }
    }
    sbuf.Append("]");

    return sbuf.ToString(str);
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
