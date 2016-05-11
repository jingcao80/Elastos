#ifndef __ELASTOS_NET_CPROXYHELPER_H__
#define __ELASTOS_NET_CPROXYHELPER_H__

#include <_Elastos_Net_CProxyHelper.h>
#include "elastos/core/Singleton.h"

using Elastos::Core::Singleton;

namespace Elastos {
namespace Net {

CarClass(CProxyHelper)
    , public Singleton
    , public IProxyHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNO_PROXY(
        /* [out] */ IProxy** proxy);
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_CPROXYHELPER_H__
