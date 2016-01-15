
#ifndef __ELASTOS_DROID_NET_PACPROXYSELECTOR_H__
#define __ELASTOS_DROID_NET_PACPROXYSELECTOR_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "_Elastos.Droid.Core.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/net/ProxySelector.h>

using Elastos::Droid::Net::IIProxyService;

using Elastos::Net::ISocketAddress;
using Elastos::Net::IURI;
using Elastos::Net::ProxySelector;
using Elastos::Net::ProxyType;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Net {

/**
 * @hide
 */
class PacProxySelector
    : public ProxySelector
    , public IPacProxySelector
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    // @Override
    CARAPI Select(
        /* [in] */ IURI* uri,
        /* [out] */ IList** result);

    // @Override
    CARAPI ConnectFailed(
        /* [in] */ IURI* uri,
        /* [in] */ ISocketAddress* address,
        /* [in] */ ECode failure);

private:
    static CARAPI_(AutoPtr<IList>) ParseResponse(
        /* [in] */ const String& response);

    static CARAPI ProxyFromHostPort(
        /* [in] */ ProxyType type,
        /* [in] */ const String& hostPortString,
        /* [out] */ Elastos::Net::IProxy** result);

private:
    static const String TAG;

    static const String SOCKS;

    static const String PROXY;

    AutoPtr<IIProxyService> mProxyService;

    AutoPtr<IList> mDefaultList;
};

} // namespace Net
} // namespace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_NET_PACPROXYSELECTOR_H__
