
#ifndef __ELASTOS_DROID_GRAPHICS_CPAINTFLAGSDRAWFILTER_H__
#define __ELASTOS_DROID_GRAPHICS_CPAINTFLAGSDRAWFILTER_H__

#include "_Elastos_Droid_Graphics_CPaintFlagsDrawFilter.h"
#include "elastos/droid/graphics/DrawFilter.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

/**
 * Subclass of DrawFilter that affects every paint by first clearing
 * the specified clearBits in the paint's flags, and then setting the
 * specified setBits in the paint's flags.
 *
 * @param clearBits These bits will be cleared in the paint's flags
 * @param setBits These bits will be set in the paint's flags
 */
CarClass(CPaintFlagsDrawFilter)
    , public DrawFilter
    , public IPaintFlagsDrawFilter
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    CARAPI constructor(
        /* [in] */ Int32 clearBits,
        /* [in] */ Int32 setBits);

private:
    static CARAPI_(Int64) NativeConstructor(
        /* [in] */ Int32 clearBits,
        /* [in] */ Int32 setBits);

public:
    Int32 mClearBits;
    Int32 mSetBits;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CPAINTFLAGSDRAWFILTER_H__
