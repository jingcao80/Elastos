
#ifndef __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__
#define __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__

#include "_Elastos_Droid_Graphics_CAtlasEntry.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CAtlasEntry)
    , public Object
    , public IAtlasEntry
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CAtlasEntry();

    CARAPI constructor();

    CARAPI SetX(
        /* [in] */ Int32 value);

    CARAPI GetX(
        /* [out] */ Int32* value);

    CARAPI SetY(
        /* [in] */ Int32 value);

    CARAPI GetY(
        /* [out] */ Int32* value);

    CARAPI SetRotated(
        /* [in] */ Boolean value);

    CARAPI GetRotated(
        /* [out] */ Boolean* value);

private:
    /**
     * Location, in pixels, of the bitmap on the X axis in the atlas.
     */
    Int32 mX;
    /**
     * Location, in pixels, of the bitmap on the Y axis in the atlas.
     */
    Int32 mY;

    /**
     * If true, the bitmap must be rotated 90 degrees in the atlas.
     */
    Boolean mRotated;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_GRAPHICS_CATLASENTRY_H__
