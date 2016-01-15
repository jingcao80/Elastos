
#include <Elastos.CoreLibrary.IO.h>
#include "elastos/droid/wifi/p2p/nsd/WifiP2pServiceResponse.h"
#include "elastos/droid/wifi/p2p/CWifiP2pDevice.h"
#include <Elastos.CoreLibrary.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Slogger;
using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::IO::IByteArrayInputStream;
using Elastos::IO::CByteArrayInputStream;
using Elastos::IO::IDataInputStream;
using Elastos::IO::CDataInputStream;
using Elastos::IO::IDataInput;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL_2(WifiP2pServiceResponse, Object, IWifiP2pServiceResponse, IParcelable)

WifiP2pServiceResponse::WifiP2pServiceResponse()
    : mServiceType(0)
    , mStatus(0)
    , mTransId(0)
{
}

ECode WifiP2pServiceResponse::constructor()
{
    return NOERROR;
}

ECode WifiP2pServiceResponse::constructor(
    /* [in] */ Int32 serviceType,
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* device,
    /* [in] */ ArrayOf<Byte>* data)
{
    mServiceType = serviceType;
    mStatus = status;
    mTransId = transId;
    mDevice = device;
    mData = data;

    return NOERROR;
}

String WifiP2pServiceResponse::StatusToString(
    /* [in] */ Int32 status)
{
    switch(status) {
    case IWifiP2pServiceResponseStatus::SUCCESS:
        return String("SUCCESS");
    case IWifiP2pServiceResponseStatus::SERVICE_PROTOCOL_NOT_AVAILABLE:
        return String("SERVICE_PROTOCOL_NOT_AVAILABLE");
    case IWifiP2pServiceResponseStatus::REQUESTED_INFORMATION_NOT_AVAILABLE:
        return String("REQUESTED_INFORMATION_NOT_AVAILABLE");
    case IWifiP2pServiceResponseStatus::BAD_REQUEST:
        return String("BAD_REQUEST");
    default:
        return String("UNKNOWN");
    }
}

ECode WifiP2pServiceResponse::GetServiceType(
    /* [out] */ Int32* serviceType)
{
    VALIDATE_NOT_NULL(serviceType);

    *serviceType = mServiceType;

    return NOERROR;
}

ECode WifiP2pServiceResponse::GetStatus(
    /* [out] */ Int32* status)
{
    VALIDATE_NOT_NULL(status);

    *status = mStatus;

    return NOERROR;
}

ECode WifiP2pServiceResponse::GetTransactionId(
    /* [out] */ Int32* transactionId)
{
    VALIDATE_NOT_NULL(transactionId);

    *transactionId = mTransId;

    return NOERROR;
}

ECode WifiP2pServiceResponse::GetRawData(
    /* [out, callee] */ ArrayOf<Byte>** rawData)
{
    VALIDATE_NOT_NULL(rawData);
    *rawData = mData;
    REFCOUNT_ADD(*rawData);
    return NOERROR;
}

ECode WifiP2pServiceResponse::GetSrcDevice(
    /* [out] */ IWifiP2pDevice** srcDevice)
{
    VALIDATE_NOT_NULL(srcDevice);

    *srcDevice = mDevice;
    REFCOUNT_ADD(*srcDevice);

    return NOERROR;
}

ECode WifiP2pServiceResponse::SetSrcDevice(
    /* [in] */ IWifiP2pDevice* dev)
{
    if (dev == NULL)
        return NOERROR;

    mDevice = dev;

    return NOERROR;
}

ECode WifiP2pServiceResponse::HexStr2Bin(
    /* [in] */ const String& hex,
    /* [out] */ ArrayOf<Byte>** array)
{
    VALIDATE_NOT_NULL(array);
    *array = NULL;

    Int32 sz = hex.GetLength() / 2;
    AutoPtr<ArrayOf<Byte> > b = ArrayOf<Byte>::Alloc(sz);
    if (b == NULL) return E_OUT_OF_MEMORY;

    String subStr;
    Int32 value;
    ECode ec = NOERROR;
    for (Int32 i = 0; i < sz; i++) {
        subStr = hex.Substring(i * 2, i * 2 + 2);
        ec = StringUtils::Parse(subStr, 16, &value);
        if (FAILED(ec)) {
            Slogger::E("WifiP2pServiceResponse", "HexStr2Bin: failed to ParseInt32 %s", hex.string());
            return ec;
        }

        (*b)[i] = (Byte)value;
    }

    *array = b;
    REFCOUNT_ADD(*array);
    return NOERROR;
}

ECode WifiP2pServiceResponse::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb("serviceType:");
    sb += mServiceType;
    sb += " status:";

    sb += StatusToString(mStatus);

    String temp;
    FAIL_RETURN(mDevice->GetDeviceAddress(&temp));
    sb += " srcAddr:";
    sb += temp;

    sb += " data:";
    sb += mData ? (const char*)mData->GetPayload() : "";

    *string = sb.ToString();
    return NOERROR;
}

ECode WifiP2pServiceResponse::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);
    *isEqual = FALSE;

    if (THIS_PROBE(IInterface) == obj) {
        *isEqual = TRUE;
        return NOERROR;
    }

    if (IWifiP2pServiceResponse::Probe(obj) == NULL) {
        *isEqual = FALSE;
        return NOERROR;
    }

    WifiP2pServiceResponse* req;// TODO = reinterpret_cast<WifiP2pServiceResponse*>(
        // TODO obj->Probe(EIID_WifiP2pServiceResponse));
    assert(req != NULL);

    String temp1, temp2;
    FAIL_RETURN(req->mDevice->GetDeviceAddress(&temp1));
    FAIL_RETURN(mDevice->GetDeviceAddress(&temp2));

    assert(0);
    // TODO
    // *isEqual = (req->mServiceType == mServiceType) &&
    //     (req->mStatus == mStatus) &&
    //     Equals(temp1, temp2) &&
    //     req->mData->Equals(mData);

    return NOERROR;
}

Boolean WifiP2pServiceResponse::Equals(
    /* [in] */ const String& a,
    /* [in] */ const String& b)
{
    if (a.IsNull() && b.IsNull()) {
        return TRUE;
    }
    else if (!a.IsNull()) {
        return a.Equals(b);
    }
    return FALSE;
}

ECode WifiP2pServiceResponse::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    if (hashCode == NULL) return NOERROR;

    Int32 result = 17;
    result = 31 * result + mServiceType;
    result = 31 * result + mStatus;
    result = 31 * result + mTransId;
    String temp;
    FAIL_RETURN(mDevice->GetDeviceAddress(&temp));
    result = 31 * result + (temp.IsNull() ? 0 : temp.GetHashCode());
    result = 31 * result + (Int32)mData.Get();
    return result;
}

ECode WifiP2pServiceResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mServiceType));
    FAIL_RETURN(source->ReadInt32(&mStatus));
    FAIL_RETURN(source->ReadInt32(&mTransId));

    Int32 hasValue;
    FAIL_RETURN(source->ReadInt32(&hasValue));
    mDevice = NULL;
    if (hasValue != 0) {
        CWifiP2pDevice::New((IWifiP2pDevice**)&mDevice);
        IParcelable* p = IParcelable::Probe(mDevice);
        p->ReadFromParcel(source);
    }

    Int32 len;
    FAIL_RETURN(source->ReadInt32(&len));

    mData = NULL;
    if (len > 0) {
        source->ReadArrayOf((Handle32*)(&mData));
    }

    return NOERROR;
}

ECode WifiP2pServiceResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mServiceType));
    FAIL_RETURN(dest->WriteInt32(mStatus));
    FAIL_RETURN(dest->WriteInt32(mTransId));

    if (mDevice != NULL) {
        dest->WriteInt32(1);
        IParcelable* p = IParcelable::Probe(mDevice);
        p->WriteToParcel(dest);
    }
    else {
        dest->WriteInt32(0);
    }

    if (mData == NULL || mData->GetLength() == 0) {
        FAIL_RETURN(dest->WriteInt32(0));
    }
    else {
        FAIL_RETURN(dest->WriteInt32(mData->GetLength()));
        dest->WriteArrayOf((Handle32)mData.Get());
    }

    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
