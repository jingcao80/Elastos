
#ifndef __ELASTOSX_NET_ELASTOS_NET_CSERVERSOCKETFACTORYHELPER_H__
#define __ELASTOSX_NET_ELASTOS_NET_CSERVERSOCKETFACTORYHELPER_H__

#include "_Elastosx_Net_CServerSocketFactoryHelper.h"
#include "Singleton.h"

namespace Elastosx {
namespace Net {

CarClass(CServerSocketFactoryHelper)
    , public Singleton
    , public IServerSocketFactoryHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetDefault(
        /* [out] */ IServerSocketFactory** factory);
};

} // namespace Net
} // namespace Elastosx

#endif //__ELASTOSX_NET_ELASTOS_NET_CSERVERSOCKETFACTORYHELPER_H__
