
#include "elastos/droid/wifi/p2p/nsd/WifiP2pServiceInfo.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Slogger.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace Wifi {
namespace P2p {
namespace Nsd {

CAR_INTERFACE_IMPL_2(WifiP2pServiceInfo, Object, IWifiP2pServiceInfo, IParcelable)

ECode WifiP2pServiceInfo::constructor()
{
    return NOERROR;
}

ECode WifiP2pServiceInfo::constructor(
    /* [in] */ ArrayOf<String>* queryList)
{
    if (queryList == NULL) {
        Slogger::E("WifiP2pServiceInfo", "query list cannot be null");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    mQueryList = queryList;
    return NOERROR;
}

ECode WifiP2pServiceInfo::GetSupplicantQueryList(
    /* [out, callee] */ ArrayOf<String>** list)
{
    VALIDATE_NOT_NULL(list);
    *list = mQueryList;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode WifiP2pServiceInfo::Bin2HexStr(
    /* [in] */ const ArrayOf<Byte>& data,
    /* [out] */ String* hexStr)
{
    VALIDATE_NOT_NULL(hexStr);
    *hexStr = String(NULL);

    StringBuilder sb;
    String s;

    for (Int32 i = 0; i < data.GetLength(); ++i) {
       // try {
       s = StringUtils::ToString((data[i] & 0xff));
       // } catch (Exception e) {
       //     e.printStackTrace();
       //     return null;
       // }

       //add 0 padding
       if (s.GetLength() == 1) {
            sb += "0";
       }
       sb += s;
    }

    *hexStr = sb.ToString();
    return NOERROR;
}

ECode WifiP2pServiceInfo::Bin2HexStr(
    /* [in] */ const char * data,
    /* [in] */ Int32 length,
    /* [out] */ String* hexStr)
{
    VALIDATE_NOT_NULL(hexStr);
    *hexStr = String(NULL);

    StringBuilder sb;
    String s;
    for (Int32 i = 0; i < length; ++i) {
       // try {
      s = StringUtils::ToString((data[i] & 0xff));
       // } catch (Exception e) {
       //     e.printStackTrace();
       //     return null;
       // }

       //add 0 padding
       if (s.GetLength() == 1) {
           sb += "0";
       }
       sb += s;
    }

    *hexStr = sb.ToString();
    return NOERROR;
}

ECode WifiP2pServiceInfo::Equals(
    /* [in] */ IInterface* obj,
    /* [out] */ Boolean* isEqual)
{
    VALIDATE_NOT_NULL(isEqual);

    if (obj == this->Probe(EIID_IInterface)) {
       *isEqual = TRUE;
       return NOERROR;
    }

    if (IWifiP2pServiceInfo::Probe(obj) == NULL) {
       *isEqual = FALSE;
       return NOERROR;
    }

    assert(0);
    WifiP2pServiceInfo* servInfo;// TODO = reinterpret_cast<WifiP2pServiceInfo*>(
      // TODO obj->Probe(EIID_WifiP2pServiceInfo));
    assert(servInfo != NULL);
    assert(0);
    // TODO
    // *isEqual = ArrayOf<String>::Equals(mQueryList, servInfo->mQueryList);
    return NOERROR;
}

ECode WifiP2pServiceInfo::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int32 result = 17;
    result = 31 * result + ((mQueryList == NULL) ? 0 : (Int32)mQueryList.Get());
    *hashCode = result;
    return NOERROR;
}

ECode WifiP2pServiceInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    VALIDATE_NOT_NULL(source);

    AutoPtr<ArrayOf<String> > list;
    FAIL_RETURN(source->ReadArrayOfString((ArrayOf<String>**)&list));
    mQueryList = list;

    return NOERROR;
}

ECode WifiP2pServiceInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    return dest->WriteArrayOfString(mQueryList);
}

} // namespace Nsd
} // namespace P2p
} // namespace Wifi
} // namespace Droid
} // namespace Elastos
