
#ifndef __ELASTOS_NET_CINET4ADDRESSHELPER_H__
#define __ELASTOS_NET_CINET4ADDRESSHELPER_H__

#include "_Elastos_Net_CInet4AddressHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

/**
 * An IPv4 address. See {@link InetAddress}.
 */
CarClass(CInet4AddressHelper)
    , public Singleton
    , public IInet4AddressHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * @hide
     */
    CARAPI GetANY(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetALL(
        /* [out] */ IInetAddress** result);

    /**
     * @hide
     */
    CARAPI GetLOOPBACK(
        /* [out] */ IInetAddress** result);

};

} // namespace Net
} // namespace Elastos

#endif //  __ELASTOS_NET_CINET4ADDRESSHELPER_H__
