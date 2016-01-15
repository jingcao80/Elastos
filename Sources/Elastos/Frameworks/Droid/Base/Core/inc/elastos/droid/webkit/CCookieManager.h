
#ifndef __ELASTOS_DROID_WEBKIT_CCOOKIEMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_CCOOKIEMANAGER_H__

#include "_Elastos_Droid_Webkit_CCookieManager.h"
#include "elastos/droid/webkit/CookieManager.h"

using Elastos::Droid::Net::IWebAddress;

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CCookieManager), public CookieManager
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CCOOKIEMANAGER_H__
