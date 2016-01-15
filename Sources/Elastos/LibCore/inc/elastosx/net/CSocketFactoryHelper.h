
#ifndef __ELASTOSX_NET_ELASTOS_NET_CSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_ELASTOS_NET_CSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_CSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {

CarClass(CSocketFactoryHelper)
    , public Singleton
    , public ISocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefault(
        /* [out] */ ISocketFactory** factory);
};

} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_ELASTOS_NET_CSOCKETFACTORYHELPER_H__
