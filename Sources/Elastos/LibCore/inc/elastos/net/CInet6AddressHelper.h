#ifndef __ELASTOS_NET_CINET6ADDRESSHELPER_H__
#define __ELASTOS_NET_CINET6ADDRESSHELPER_H__

#include "_Elastos_Net_CInet6AddressHelper.h"
#include "elastos/net/InetAddress.h"
#include "elastos/core/Singleton.h"

namespace Elastos {
namespace Net {

CarClass(CInet6AddressHelper)
    , public Singleton
    , public IInet6AddressHelper
{
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ Int32 scope_id,
        /* [out] */ IInet6Address** address);

    CARAPI GetByAddress(
        /* [in] */ const String& host,
        /* [in] */ ArrayOf<Byte>* addr,
        /* [in] */ INetworkInterface* nif,
        /* [out] */ IInet6Address** address);

    /**
     * @hide
     */
    CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CINET6ADDRESSHELPER_H__
