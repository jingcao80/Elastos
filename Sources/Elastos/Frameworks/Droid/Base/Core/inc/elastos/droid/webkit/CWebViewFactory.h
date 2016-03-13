
#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWFACTORY_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWFACTORY_H__

#include "_Elastos_Droid_Webkit_CWebViewFactory.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Webkit::IWebViewFactory;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewFactory)
    , public Singleton
    , public IWebViewFactory
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetWebViewPackageName(
        /* [out] */ String* packageName);

    CARAPI GetLoadedPackageInfo(
        /* [out] */ IPackageInfo** packageInfo);

    CARAPI PrepareWebViewInZygote();

    CARAPI PrepareWebViewInSystemServer();

    CARAPI OnWebViewUpdateInstalled();
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CWEBVIEWFACTORY_H__