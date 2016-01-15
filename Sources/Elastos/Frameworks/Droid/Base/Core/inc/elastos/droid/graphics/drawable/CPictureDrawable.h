
#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CPICTUREDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CPICTUREDRAWABLE_H__

#include "_Elastos_Droid_Graphics_Drawable_CPictureDrawable.h"
#include "elastos/droid/graphics/drawable/PictureDrawable.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CPictureDrawable), public PictureDrawable
{
public:
    CAR_OBJECT_DECL();

    /**
     * Construct a new drawable referencing the specified picture. The picture
     * may be null.
     *
     * @param picture The picture to associate with the drawable. May be null.
     */
    CARAPI constructor(
        /* [in] */ IPicture* picture);
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_DRAWABLE_CPICTUREDRAWABLE_H__
