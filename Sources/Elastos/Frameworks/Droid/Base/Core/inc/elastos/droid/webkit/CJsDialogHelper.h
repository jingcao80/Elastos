
#ifndef __ELASTOS_DROID_WEBKIT_CJSDIALOGHELPER_H__
#define __ELASTOS_DROID_WEBKIT_CJSDIALOGHELPER_H__

#include "_Elastos_Droid_Webkit_CJsDialogHelper.h"
#include "elastos/droid/webkit/JsDialogHelper.h"

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Helper class to create JavaScript dialogs. It is used by
 * different WebView implementations.
 *
 * @hide Helper class for internal use
 */
CarClass(CJsDialogHelper), public JsDialogHelper
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CJSDIALOGHELPER_H__
