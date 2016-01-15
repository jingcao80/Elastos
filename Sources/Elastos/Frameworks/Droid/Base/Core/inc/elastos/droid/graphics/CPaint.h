
#ifndef __ELASTOS_DROID_GRAPHICS_CPAINT_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINT_H__

#include "_Elastos_Droid_Graphics_CPaint.h"
#include "elastos/droid/graphics/Paint.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPaint), public Paint
{
public:
    CAR_OBJECT_DECL();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPAINT_H__
