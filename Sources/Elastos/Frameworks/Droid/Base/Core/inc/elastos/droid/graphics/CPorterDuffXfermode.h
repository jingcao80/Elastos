
#ifndef __ELASTOS_DROID_GRAPHICS_CPORTERDUFFXFERMODE_H__
#define __ELASTOS_DROID_GRAPHICS_CPORTERDUFFXFERMODE_H__

#include "_Elastos_Droid_Graphics_CPorterDuffXfermode.h"
#include "elastos/droid/graphics/Xfermode.h"

namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CPorterDuffXfermode)
    , public Xfermode
    , public IPorterDuffXfermode
{
public:
    CAR_OBJECT_DECL();

    CAR_INTERFACE_DECL();

    /**
     * Create an xfermode that uses the specified porter-duff mode.
     *
     * @param mode           The porter-duff mode that is applied
     */
    CARAPI constructor(
        /* [in] */ PorterDuffMode mode);

    CARAPI GetMode(
        /* [out] */ PorterDuffMode* mode);

private:
    static CARAPI_(Int64) NativeCreateXfermode(
        /* [in] */ PorterDuffMode mode);

public:
    PorterDuffMode mMode;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CPORTERDUFFXFERMODE_H__
