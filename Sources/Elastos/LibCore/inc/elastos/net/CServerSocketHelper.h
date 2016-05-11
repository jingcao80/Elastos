
#ifndef __ELASTOS_NET_CSERVERSOCKETHELPER_H__
#define __ELASTOS_NET_CSERVERSOCKETHELPER_H__

#include "_Elastos_Net_CServerSocketHelper.h"
#include "Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CServerSocketHelper)
    , public Singleton
    , public IServerSocketHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetSocketFactory(
        /* [in] */ ISocketImplFactory* aFactory);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CSERVERSOCKETHELPER_H__
