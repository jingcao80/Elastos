
#ifndef __ELASTOS_DROID_WEBKIT_CHTTPAUTHHANDLER_H__
#define __ELASTOS_DROID_WEBKIT_CHTTPAUTHHANDLER_H__

#include "_Elastos_Droid_Webkit_CHttpAuthHandler.h"

#include "elastos/droid/webkit/HttpAuthHandler.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

CarClass(CHttpAuthHandler), public HttpAuthHandler
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CHTTPAUTHHANDLER_H__
