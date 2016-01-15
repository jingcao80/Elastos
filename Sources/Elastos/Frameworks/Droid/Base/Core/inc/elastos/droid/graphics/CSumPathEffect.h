
#ifndef __ELASTOS_DROID_GRAPHICS_CSUMPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CSUMPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CSumPathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CSumPathEffect)
    , public PathEffect
    , public ISumPathEffect
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Construct a PathEffect whose effect is to apply two effects, in sequence.
     * (e.g. first(path) + second(path))
     */
    CARAPI constructor(
        /* [in] */ IPathEffect* first,
        /* [in] */ IPathEffect* second);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ Int64 first,
        /* [in] */ Int64 second);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CSUMPATHEFFECT_H__
