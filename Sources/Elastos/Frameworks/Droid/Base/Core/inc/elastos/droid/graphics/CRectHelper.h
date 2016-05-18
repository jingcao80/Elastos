
#ifndef __ELASTOS_DROID_GRAPHICS_CRECTHELPER_H__
#define __ELASTOS_DROID_GRAPHICS_CRECTHELPER_H__

#include "_Elastos_Droid_Graphics_CRectHelper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CRectHelper)
    , public Singleton
    , public IRectHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL();

    CARAPI UnflattenFromString(
        /* [in] */ const String& str,
        /* [out] */ IRect** rect);

    /**
     * Returns true iff the two specified rectangles intersect. In no event are
     * either of the rectangles modified. To record the intersection,
     * use {@link #intersect(Rect)} or {@link #setIntersect(Rect, Rect)}.
     *
     * @param a The first rectangle being tested for intersection
     * @param b The second rectangle being tested for intersection
     * @return true iff the two specified rectangles intersect. In no event are
     *              either of the rectangles modified.
     */
    CARAPI Intersects(
        /* [in] */ IRect* a,
        /* [in] */ IRect* b,
        /* [out] */ Boolean* result);
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CRECTHELPER_H__
