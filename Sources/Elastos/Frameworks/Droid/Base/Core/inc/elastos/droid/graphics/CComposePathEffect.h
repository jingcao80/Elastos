
#ifndef __ELASTOS_DROID_GRAPHICS_CCOMPOSEPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CCOMPOSEPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CComposePathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CComposePathEffect)
    , public PathEffect
    , public IComposePathEffect
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    /**
     * Construct a PathEffect whose effect is to apply first the inner effect
     * and the the outer pathEffect (e.g. outer(inner(path))).
     */
    CARAPI constructor(
        /* [in] */ IPathEffect* outerpe,
        /* [in] */ IPathEffect* innerpe);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 outerpe,
        /* [in] */ Int64 innerpe);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CCOMPOSEPATHEFFECT_H__
