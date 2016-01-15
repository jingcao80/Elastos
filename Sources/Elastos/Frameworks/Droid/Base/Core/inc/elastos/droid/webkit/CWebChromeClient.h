
#ifndef __ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENT_H__
#define __ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENT_H__

#include "_Elastos_Droid_Webkit_CWebChromeClient.h"
#include "elastos/droid/webkit/WebChromeClient.h"

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CWebChromeClient), public WebChromeClient
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CWEBCHROMECLIENT_H__
