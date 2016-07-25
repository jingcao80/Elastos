
#ifndef __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__

#include "Elastos.Droid.Content.h"
#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallCameraManager
    : public Object
{
public:
    InCallCameraManager();

    /**
     * Initializes the InCall CameraManager.
     *
     * @param context The current context.
     */
    CARAPI constructor(
        /* [in] */ IContext* context);
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLCAMERAMANAGER_H__
