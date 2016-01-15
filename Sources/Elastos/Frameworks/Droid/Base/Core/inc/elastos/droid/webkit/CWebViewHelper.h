
#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWHELPER_H__

#include "_Elastos_Droid_Webkit_CWebViewHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewHelper)
    , public Singleton
    , public IWebViewHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI EnablePlatformNotifications();

    CARAPI DisablePlatformNotifications();

    CARAPI FindAddress(
        /* [in] */ const String& addr,
        /* [out] */ String* outAddr);

    CARAPI GetPluginList(
        /* [out] */ IPluginList** pluginList);

    CARAPI ClearClientCertPreferences(
        /* [in] */ IRunnable* onCleared);

    CARAPI EnableSlowWholeDocumentDraw();

    CARAPI SetWebContentsDebuggingEnabled(
        /* [in] */ Boolean enabled);
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBVIEWHELPER_H__
