#ifndef  __ELASTOS_DROID_LAUNCHER2_CSHORTCUT_AND_WIDGET_CONTAINER_H__
#define  __ELASTOS_DROID_LAUNCHER2_CSHORTCUT_AND_WIDGET_CONTAINER_H__

#include "_Elastos_Droid_Launcher2_CShortcutAndWidgetContainer.h"
#include "elastos/droid/launcher2/ShortcutAndWidgetContainer.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

/**
 * A ViewGroup that coordinates dragging across its descendants
 */
CarClass(CShortcutAndWidgetContainer)
    , public ShortcutAndWidgetContainer
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CSHORTCUT_AND_WIDGET_CONTAINER_H__