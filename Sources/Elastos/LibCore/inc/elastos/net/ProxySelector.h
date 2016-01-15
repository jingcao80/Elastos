
#ifndef __ELASTOS_NET_PROXYSELECTOR_H__
#define __ELASTOS_NET_PROXYSELECTOR_H__

#include "Elastos.CoreLibrary.Net.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Net::INetPermission;

namespace Elastos {
namespace Net {

class ECO_PUBLIC ProxySelector
    : public Object
    , public IProxySelector
{
public:
    CAR_INTERFACE_DECL()

    ProxySelector();

    virtual ~ProxySelector();

    static CARAPI GetDefault(
        /* [out] */ IProxySelector** defaultSelector);

    static CARAPI SetDefault(
        /* [in] */ IProxySelector* selector);

    virtual CARAPI Select(
        /* [in] */ IURI* uri,
        /* [out] */ IList** container) = 0;

    virtual CARAPI ConnectFailed(
        /* [in] */ IURI* uri,
        /* [in] */ ISocketAddress* sa,
        /* [in] */ ECode ec) = 0;

private:
    ECO_LOCAL static AutoPtr<IProxySelector> sDefaultSelector;
};

} // namespace Net
} // namespace Elastos

#endif //__ELASTOS_NET_ProxySelector_H__
