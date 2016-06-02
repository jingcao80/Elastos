
#ifndef __ELASTOS_DROID_VIEW_CGLES20CANVAS_HELPER_H__
#define __ELASTOS_DROID_VIEW_CGLES20CANVAS_HELPER_H__

#include "_Elastos_Droid_View_CGLES20CanvasHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CGLES20CanvasHelper)
    , public Singleton
    , public IGLES20CanvasHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI SetProperty(
        /* [in] */ const String& name,
        /* [in] */ const String& value);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_VIEW_CGLES20CANVAS_HELPER_H__
