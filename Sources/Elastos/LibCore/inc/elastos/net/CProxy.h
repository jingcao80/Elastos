#ifndef __ELASTOS_NET_CPROXY_H__
#define __ELASTOS_NET_CPROXY_H__

#include "_Elastos_Net_CProxy.h"
#include "Object.h"


namespace Elastos {
namespace Net {

CarClass(CProxy)
    , public Object
    , public IProxy
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ ProxyType type,
        /* [in] */ ISocketAddress* sa);

    CARAPI GetType(
        /* [out] */ ProxyType* type);

    CARAPI GetAddress(
        /* [out] */ ISocketAddress** address);

    CARAPI static GetNO_PROXY(
        /* [out] */ IProxy** proxy);

    /**
     * Gets a textual representation of this {@code Proxy} instance. The string
     * includes the two parts {@code type.toString()} and {@code
     * address.toString()} if {@code address} is not {@code null}.
     *
     * @return the representing string of this proxy.
     */
    CARAPI ToString(
        /* [out] */ String * str);

    CARAPI Equals(
        /* [in] */ IInterface * o,
        /* [out] */ Boolean* resutl);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

public:
    static AutoPtr<IProxy> NO_PROXY;// = new Proxy();

private:
    ProxyType mType;

    AutoPtr<ISocketAddress> mAddress;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPROXY_H__
