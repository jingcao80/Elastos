
#ifndef __ELASTOS_DROID_GRAPHICS_CTYPEFACE_H__
#define __ELASTOS_DROID_GRAPHICS_CTYPEFACE_H__

#include "_Elastos_Droid_Graphics_CTypeface.h"
#include "elastos/droid/graphics/Typeface.h"

using namespace Elastos;

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * The Typeface class specifies the typeface and intrinsic style of a font.
 * This is used in the paint, along with optionally Paint settings like
 * textSize, textSkewX, textScaleX to specify
 * how text appears when drawn (and measured).
 */
CarClass(CTypeface), public Typeface
{
public:
    CAR_OBJECT_DECL();
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CTYPEFACE_H__
