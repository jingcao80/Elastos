
#ifndef __ELASTOS_DROID_GRAPHICS_CINSETSHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CINSETSHELPER_H__

#include "_Elastos_Droid_Graphics_CInsetsHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CInsetsHelper)
    , public Singleton
    , public IInsetsHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param left the left inset
     * @param top the top inset
     * @param right the right inset
     * @param bottom the bottom inset
     *
     * @return Insets instance with the appropriate values
     */
    CARAPI Of(
        /* [in] */ Int32 left,
        /* [in] */ Int32 top,
        /* [in] */ Int32 right,
        /* [in] */ Int32 bottom,
        /* [out] */ IInsets** insets);

    /**
     * Return an Insets instance with the appropriate values.
     *
     * @param r the rectangle from which to take the values
     *
     * @return an Insets instance with the appropriate values
     */
    CARAPI Of(
        /* [in] */ IRect* r,
        /* [out] */ IInsets** insets);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CINSETSHELPER_H__
