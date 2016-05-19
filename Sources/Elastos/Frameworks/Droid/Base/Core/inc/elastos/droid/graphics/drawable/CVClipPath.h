#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVCLIPPATH_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVCLIPPATH_H__

#include "elastos/droid/graphics/drawable/VectorDrawable.h"
#include "_Elastos_Droid_Graphics_Drawable_CVClipPath.h"

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

CarClass(CVClipPath)
    , public VectorDrawable::VClipPath
{
public:
    CAR_OBJECT_DECL()
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_CVCLIPPATH_H__
