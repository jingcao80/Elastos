
#ifndef __ELASTOS_DROID_NET_CDHCPINFO_H__
#define __ELASTOS_DROID_NET_CDHCPINFO_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Net_CDhcpInfo.h"
#include <elastos/core/Object.h>
#include <elastos/core/StringBuffer.h>

using Elastos::Core::CString;
using Elastos::Core::StringBuffer;

namespace Elastos {
namespace Droid {
namespace Net {

CarClass(CDhcpInfo)
    , public Object
    , public IDhcpInfo
    , public IParcelable
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDhcpInfo* source);

    CARAPI ToString(
        /* [out] */ String* result);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI GetIpAddress(
        /* [out] */ Int32* ipaddress);

    CARAPI GetGateway(
        /* [out] */ Int32* gateway);

    CARAPI GetNetmask(
        /* [out] */Int32* netmask);

    CARAPI GetDns1(
        /* [out] */ Int32* dns1);

    CARAPI GetDns2(
        /* [out] */ Int32* dns2);

    CARAPI GetServerAddress(
        /* [out] */ Int32* serverAddress);

    CARAPI GetLeaseDuration(
        /* [out] */ Int32* leaseDuration);

    CARAPI SetIpAddress(
        /* [in] */ Int32 ipaddress);

    CARAPI SetGateway(
        /* [in] */ Int32 gateway);

    CARAPI SetNetmask(
        /* [in] */ Int32 netmask);

    CARAPI SetDns1(
        /* [in] */ Int32 dns1);

    CARAPI SetDns2(
        /* [in] */ Int32 dns2);

    CARAPI SetServerAddress(
        /* [in] */ Int32 serverAddress);

    CARAPI SetLeaseDuration(
        /* [in] */ Int32 leaseDuration);

private:
    static CARAPI_(void) PutAddress(
        /* [in] */ StringBuffer* buff,
        /* [in] */ Int32 addr);

private:
    Int32 mIpAddress;
    Int32 mGateway;
    Int32 mNetmask;

    Int32 mDns1;
    Int32 mDns2;

    Int32 mServerAddress;
    Int32 mLeaseDuration;
};

} // namespace Net
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_NET_CDHCPINFO_H__

