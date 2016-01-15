#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLDIFPLAYIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLDIFPLAYIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLDisplayImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

class CEGLImpl;

CarClass(CEGLDisplayImpl)
{
public:
    CARAPI constructor(
        /* [in] */ Int32 dpy);

    CARAPI Equals(
            /* [in] */ IInterface *object,
            /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

    CARAPI_(Int32) GetEGLDisplay();

private:
    Int32 mEGLDisplay;
    friend class CEGLImpl;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
#endif
