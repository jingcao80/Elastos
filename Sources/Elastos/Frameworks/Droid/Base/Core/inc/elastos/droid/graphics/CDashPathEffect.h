
#ifndef __ELASTOS_DROID_GRAPHICS_CDASHPATHEFFECT_H__
#define __ELASTOS_DROID_GRAPHICS_CDASHPATHEFFECT_H__

#include "_Elastos_Droid_Graphics_CDashPathEffect.h"
#include "elastos/droid/graphics/PathEffect.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CDashPathEffect)
    , public PathEffect
    , public IDashPathEffect
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * The intervals array must contain an even number of entries (>=2), with
     * the even indices specifying the "on" intervals, and the odd indices
     * specifying the "off" intervals. phase is an offset into the intervals
     * array (mod the sum of all of the intervals). The intervals array
     * controls the length of the dashes. The paint's strokeWidth controls the
     * thickness of the dashes.
     * Note: this patheffect only affects drawing with the paint's style is set
     * to STROKE or STROKE_AND_FILL. It is ignored if the drawing is done with
     * style == FILL.
     * @param intervals array of ON and OFF distances
     * @param phase offset into the intervals array
     */
    CARAPI constructor(
        /* [in] */ const ArrayOf<Float>& intervals,
        /* [in] */ Float phase);

private:
    static CARAPI_(Int64) NativeCreate(
        /* [in] */ const ArrayOf<Float>& intervals,
        /* [in] */ Float phase);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CDASHPATHEFFECT_H__
