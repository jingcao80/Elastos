
#include "elastos/droid/webkit/webview/chromium/native/components/InterceptNavigationDelegate.h"
#include "elastos/droid/webkit/webview/chromium/native/components/api/InterceptNavigationDelegate_dec.h"
#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Components {

//=====================================================================
//                           InterceptNavigationDelegate
//=====================================================================
Boolean InterceptNavigationDelegate::ShouldIgnoreNavigation(
    /* [in] */ IInterface* obj,
    /* [in] */ IInterface* navigationParams)
{
    InterceptNavigationDelegate* mObj = (InterceptNavigationDelegate*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("InterceptNavigationDelegate", "InterceptNavigationDelegate::ShouldIgnoreNavigation, mObj is NULL");
        return FALSE;
    }
    NavigationParams* np = (NavigationParams*)(IObject::Probe(navigationParams));
    return mObj->ShouldIgnoreNavigation(np);
}

} // namespace Components
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

