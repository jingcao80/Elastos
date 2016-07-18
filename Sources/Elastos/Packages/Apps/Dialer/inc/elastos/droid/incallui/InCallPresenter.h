
#ifndef __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class InCallPresenter
    : public Object
{
public:
    CAR_INTERFACE_DECL();

    /**
     * @return True if the application is currently running in a right-to-left locale.
     */
    static CARAPI_(Boolean) IsRtl();
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_INCALLPRESENTER_H__
