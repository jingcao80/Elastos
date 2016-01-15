
#ifndef __ELASTOS_DROID_GRAPHICS_CEMBOSSMASKFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_CEMBOSSMASKFILTER_H__

#include "_Elastos_Droid_Graphics_CEmbossMaskFilter.h"
#include "elastos/droid/graphics/MaskFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CEmbossMaskFilter)
    , public MaskFilter
    , public IEmbossMaskFilter
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Create an emboss maskfilter
     *
     * @param direction  array of 3 scalars [x, y, z] specifying the direction of the light source
     * @param ambient    0...1 amount of ambient light
     * @param specular   coefficient for specular highlights (e.g. 8)
     * @param blurRadius amount to blur before applying lighting (e.g. 3)
     * @return           the emboss maskfilter
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Float>& direction,
        /* [in] */ Float ambient,
        /* [in] */ Float specular,
        /* [in] */ Float blurRadius);

private:
    static CARAPI_(Int64) NativeConstructor(
        /* [in] */ const ArrayOf<Float>& dirArray,
        /* [in] */ Float ambient,
        /* [in] */ Float specular,
        /* [in] */ Float blurRadius);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CEMBOSSMASKFILTER_H__
