
#ifndef __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__

#include "_Elastos_Droid_Webkit_CWebViewClient.h"
#include "elastos/droid/webkit/WebViewClient.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Net::Http::ISslError;
using Elastos::Droid::View::IInputEvent;
using Elastos::Droid::View::IKeyEvent;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebViewClient), public WebViewClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBVIEWCLIENT_H__
