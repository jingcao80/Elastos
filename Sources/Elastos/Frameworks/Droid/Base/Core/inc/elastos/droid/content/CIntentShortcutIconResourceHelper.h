
#ifndef __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__
#define __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__

#include "_Elastos_Droid_Content_CIntentShortcutIconResourceHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CIntentShortcutIconResourceHelper)
    , public Singleton
    , public IIntentShortcutIconResourceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Creates a new ShortcutIconResource for the specified context and resource
     * identifier.
     *
     * @param context The context of the application.
     * @param resourceId The resource idenfitier for the icon.
     * @return A new ShortcutIconResource with the specified's context package name
     *         and icon resource idenfitier.
     */
    CARAPI FromContext(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resourceId,
        /* [out] */ IIntentShortcutIconResource** shortcutIconRes);
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CINTENTSHORTCUTICONRESOURCEHELPER_H__
