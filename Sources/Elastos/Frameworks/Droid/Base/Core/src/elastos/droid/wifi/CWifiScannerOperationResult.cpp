
#include "elastos/droid/wifi/CWifiScannerOperationResult.h"

namespace Elastos {
namespace Droid {
namespace Wifi {

CAR_INTERFACE_IMPL_2(CWifiScannerOperationResult, Object, IWifiScannerOperationResult, IParcelable)

CAR_OBJECT_IMPL(CWifiScannerOperationResult)

ECode CWifiScannerOperationResult::constructor()
{
    return NOERROR;
}

ECode CWifiScannerOperationResult::constructor(
    /* [in] */ Int32 reason,
    /* [in] */ const String& description)
{
    mReason = reason;
    mDescription = description;
    return NOERROR;
}

ECode CWifiScannerOperationResult::GetReason(
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mReason;
    return NOERROR;
}

ECode CWifiScannerOperationResult::SetReason(
    /* [in] */ Int32 reason)
{
    mReason = reason;
    return NOERROR;
}

ECode CWifiScannerOperationResult::GetDescription(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mDescription;
    return NOERROR;
}

ECode CWifiScannerOperationResult::SetDescription(
    /* [in] */ const String& description)
{
    mDescription = description;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerOperationResult::DescribeContents(
    /* [out] */ Int32* contents)
{
    VALIDATE_NOT_NULL(contents);
    *contents = 0;
    return NOERROR;
}

/** Implement the Parcelable interface {@hide} */
ECode CWifiScannerOperationResult::WriteToParcel(
    /* [in] */ IParcel* dest,
    /* [in] */ Int32 flags)
{
    dest->WriteInt32(mReason);
    dest->WriteString(mDescription);
    return NOERROR;
}

ECode CWifiScannerOperationResult::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

ECode CWifiScannerOperationResult::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(0);
    // TODO
    return E_NOT_IMPLEMENTED;
}

} // namespace Wifi
} // namespace Droid
} // namespace Elastos
