#ifndef __ELASTOS_DROID_SERVER_WEBKIT_CWEBVIEW_UPDATE_BINDER_SERVICE_H__
#define __ELASTOS_DROID_SERVER_WEBKIT_CWEBVIEW_UPDATE_BINDER_SERVICE_H__

#include "_Elastos_Droid_Server_Webkit_CWebViewUpdateBinderService.h"
#include "elastos/droid/server/webkit/WebViewUpdateService.h"

namespace Elastos {
namespace Droid {
namespace Server {
namespace Webkit {

CarClass(CWebViewUpdateBinderService)
    , public WebViewUpdateService::BinderService
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Server
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SERVER_WEBKIT_CWEBVIEW_UPDATE_BINDER_SERVICE_H__
