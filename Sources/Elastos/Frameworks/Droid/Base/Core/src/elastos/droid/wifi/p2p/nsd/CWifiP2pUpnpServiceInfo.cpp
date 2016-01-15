
#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pUpnpServiceInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/wifi/p2p/nsd/CWifiP2pServiceInfo.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::IUUID;
using Elastos::Utility::IUUIDHelper;
using Elastos::Utility::CUUIDHelper;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_OBJECT_IMPL(CWifiP2pUpnpServiceInfo)

ECode CWifiP2pUpnpServiceInfo::constructor()
{
    return NOERROR;
}

ECode CWifiP2pUpnpServiceInfo::constructor(
    /* [in] */ ArrayOf<String>* queryList)
{
    return WifiP2pServiceInfo::constructor(queryList);
}

ECode CWifiP2pUpnpServiceInfo::GetSupplicantQueryList(
    /* [out, callee] */ ArrayOf<String>** list)
{
    return WifiP2pServiceInfo::GetSupplicantQueryList(list);
}

ECode CWifiP2pUpnpServiceInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    return WifiP2pServiceInfo::Equals(obj, isEqual);
}

ECode CWifiP2pUpnpServiceInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    return WifiP2pServiceInfo::GetHashCode(hashCode);
}

ECode CWifiP2pUpnpServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    return WifiP2pServiceInfo::ReadFromParcel(source);
}

ECode CWifiP2pUpnpServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return WifiP2pServiceInfo::WriteToParcel(dest);
}

ECode CWifiP2pUpnpServiceInfo::NewInstance(
    /* [in] */ const String& uuid,
    /* [in] */ const String& device,
    /* [in] */ ArrayOf<String>* services,
    /* [out] */ IWifiP2pUpnpServiceInfo** instance)
{
    VALIDATE_NOT_NULL(instance);
    *instance = NULL;

    if (uuid.IsNull() || device.IsNull()) {
        Slogger::E("CWifiP2pUpnpServiceInfo", "uuid or device cannnot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IUUIDHelper> helper;
    CUUIDHelper::AcquireSingleton((IUUIDHelper**)&helper);
    AutoPtr<IUUID> uObj;
    helper->FromString(uuid, (IUUID**)&uObj);

    Int32 count = 3;
    if (services != NULL) count += services->GetLength();

    AutoPtr<ArrayOf<String> > info = ArrayOf<String>::Alloc(count);
    if (info == NULL) return E_OUT_OF_MEMORY;

    String temp;
    FAIL_RETURN(CreateSupplicantQuery(uuid, String(NULL), &temp));
    info->Set(0, temp);
    FAIL_RETURN(CreateSupplicantQuery(uuid, String("upnp:rootdevice"), &temp));
    info->Set(1, temp);
    FAIL_RETURN(CreateSupplicantQuery(uuid, device, &temp));
    info->Set(2, temp);

    if (services != NULL) {
        for (Int32 i = 0; i < services->GetLength(); ++i) {
            FAIL_RETURN(CreateSupplicantQuery(uuid, (*services)[i], &temp));
            info->Set(3 + i, temp);
        }
    }

    AutoPtr<CWifiP2pUpnpServiceInfo> wpusi;
    CWifiP2pUpnpServiceInfo::NewByFriend(info, (CWifiP2pUpnpServiceInfo**)&wpusi);
    assert(0);
    // TODO
    // *instance = (CWifiP2pUpnpServiceInfo*)wpusi.Get();
    REFCOUNT_ADD(*instance);
    return NOERROR;
}

ECode CWifiP2pUpnpServiceInfo::CreateSupplicantQuery(
    /* [in] */ const String& uuid,
    /* [in] */ const String& data,
    /* [out] */ String* queryString)
{
    VALIDATE_NOT_NULL(queryString);

    StringBuilder sb;
    sb += "upnp ";
    String temp;
    assert(0);
    // TODO
    // String usStr;
    // Locale::US->ToString(&usStr)
    // temp.AppendFormat(usStr, "%02x ", IWifiP2pUpnpServiceInfo::VERSION_1_0);
    sb += temp;
    sb += "uuid:";
    sb += uuid;

    if (!data.IsNull()) {
        sb += "::";
        sb += data;
    }

    *queryString = sb.ToString();
    return NOERROR;
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
