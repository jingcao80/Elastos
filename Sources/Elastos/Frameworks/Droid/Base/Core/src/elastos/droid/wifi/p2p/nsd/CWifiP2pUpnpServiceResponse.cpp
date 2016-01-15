
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceResponse.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceResponseStatus.h"
#include <elastos/core/StringUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringUtils;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_OBJECT_IMPL(CWifiP2pUpnpServiceResponse)

CWifiP2pUpnpServiceResponse::CWifiP2pUpnpServiceResponse()
    : mVersion(0)
{}

ECode CWifiP2pUpnpServiceResponse::constructor()
{
    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::constructor(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* device,
    /* [in] */ ArrayOf<Byte>* data)
{
    FAIL_RETURN(WifiP2pServiceResponse::constructor(
        IWifiP2pServiceInfo::SERVICE_TYPE_UPNP, status, transId, device, data));

    Boolean ret;
    FAIL_RETURN(Parse(&ret));
    if (!ret) {
        Slogger::E("CWifiP2pUpnpServiceResponse", "Malformed upnp service response");
        // throw new IllegalArgumentException("Malformed upnp service response");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::Parse(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = FALSE;

    /*
     * The data format is as follows.
     *
     * ______________________________________________________
     * |  Version (1)  |          USN (Variable)            |
     */
    if (mData == NULL) {
        // the empty is OK.
        *result = TRUE;
        return NOERROR;
    }

    if (mData->GetLength() < 1) {
        *result = FALSE;
        return NOERROR;
    }

    mVersion = (*mData)[0] & 0xff;
    String temp((const char *)(mData->GetPayload() + 1), mData->GetLength() - 1);
    AutoPtr<ArrayOf<String> > names;
    FAIL_RETURN(StringUtils::Split(temp, String(","), (ArrayOf<String>**)&names));
    mUniqueServiceNames = names;

    *result = TRUE;
    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::GetServiceType(
    /* [out] */ Int32* serviceType)
{
    return WifiP2pServiceResponse::GetServiceType(serviceType);
}

ECode CWifiP2pUpnpServiceResponse::GetStatus(
    /* [out] */ Int32* status)
{
    return WifiP2pServiceResponse::GetStatus(status);
}

ECode CWifiP2pUpnpServiceResponse::GetTransactionId(
    /* [out] */ Int32* transactionId)
{
    return WifiP2pServiceResponse::GetTransactionId(transactionId);
}

ECode CWifiP2pUpnpServiceResponse::GetRawData(
    /* [out, callee] */ ArrayOf<Byte>** rawData)
{
    return WifiP2pServiceResponse::GetRawData(rawData);
}

ECode CWifiP2pUpnpServiceResponse::GetSrcDevice(
    /* [out] */ IWifiP2pDevice** srcDevice)
{
    return WifiP2pServiceResponse::GetSrcDevice(srcDevice);
}

ECode CWifiP2pUpnpServiceResponse::SetSrcDevice(
    /* [in] */ IWifiP2pDevice* dev)
{
    return WifiP2pServiceResponse::SetSrcDevice(dev);
}

ECode CWifiP2pUpnpServiceResponse::ToString(
    /* [out] */ String* string)
{
    VALIDATE_NOT_NULL(string);

    StringBuilder sb;

    String sbuf;
    sb +="serviceType:UPnP(";
    sb += mServiceType;
    sb +=")";

    sb +=" status:";
    sb += StatusToString(mStatus);

    sb +=" srcAddr:";
    String temp;
    mDevice->GetDeviceAddress(&temp);
    sb += temp;

    sb +=" version:";
    String version;
    version.AppendFormat("%02x", mVersion);
    sb += version;

    if (mUniqueServiceNames != NULL) {
        for (Int32 i = 0; i < mUniqueServiceNames->GetLength(); ++i) {
            sb +=" usn:";
            sb += (*mUniqueServiceNames)[i];
        }
    }

    *string = sb.ToString();
    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    return WifiP2pServiceResponse::Equals(obj, isEqual);
}

ECode CWifiP2pUpnpServiceResponse::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return WifiP2pServiceResponse::GetHashCode(hashCode);
}

ECode CWifiP2pUpnpServiceResponse::GetVersion(
    /* [out] */ Int32* versionNumber)
{
    VALIDATE_NOT_NULL(versionNumber);

    *versionNumber = mVersion;

    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::GetUniqueServiceNames(
    /* [out, callee] */ ArrayOf<String>** uniqueServiceNames)
{
    VALIDATE_NOT_NULL(uniqueServiceNames);

    *uniqueServiceNames = mUniqueServiceNames;
    REFCOUNT_ADD(*uniqueServiceNames);

    return NOERROR;
}

ECode CWifiP2pUpnpServiceResponse::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return WifiP2pServiceResponse::ReadFromParcel(source);
}

ECode CWifiP2pUpnpServiceResponse::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return WifiP2pServiceResponse::WriteToParcel(dest);
}

ECode CWifiP2pUpnpServiceResponse::NewInstance(
    /* [in] */ Int32 status,
    /* [in] */ Int32 transId,
    /* [in] */ IWifiP2pDevice* dev,
    /* [in] */ ArrayOf<Byte>* data,
    /* [out] */ IWifiP2pUpnpServiceResponse** instance)
{
    VALIDATE_NOT_NULL(instance);

    if (status != IWifiP2pServiceResponseStatus::SUCCESS) {
        AutoPtr<CWifiP2pUpnpServiceResponse> resp;
        CWifiP2pUpnpServiceResponse::NewByFriend(
            status, transId, dev, NULL, (CWifiP2pUpnpServiceResponse**)&resp);
        *instance = (IWifiP2pUpnpServiceResponse*)resp.Get();
        REFCOUNT_ADD(*instance);
        return NOERROR;
    }

    AutoPtr<CWifiP2pUpnpServiceResponse> resp;
    FAIL_RETURN(CWifiP2pUpnpServiceResponse::NewByFriend(
        status, transId, dev, data, (CWifiP2pUpnpServiceResponse**)&resp));
    *instance = (IWifiP2pUpnpServiceResponse*)resp.Get();
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
