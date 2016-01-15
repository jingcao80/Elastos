
#ifndef __ELASTOS_DROID_WEBKIT_CPLUGIN_H__
#define __ELASTOS_DROID_WEBKIT_CPLUGIN_H__

#include "_Elastos_Droid_Webkit_CPlugin.h"
#include "elastos/droid/webkit/Plugin.h"

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;

namespace Elastos {
namespace Droid {
namespace Webkit {

/**
 * Represents a plugin (Java equivalent of the PluginPackageAndroid
 * C++ class in libs/WebKitLib/WebKit/WebCore/plugins/android/)
 *
 * @hide
 * @deprecated This interface was intended to be used by Gears. Since Gears was
 * deprecated, so is this class.
 */
CarClass(CPlugin) , public Plugin
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_WEBKIT_CPLUGIN_H__
