#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLDIFPLAYIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLDIFPLAYIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLDisplayImpl.h"
#include "Elastos.Droid.Opengl.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLDisplayImpl)
    , public Object
    , public Elastosx::Microedition::Khronos::Egl::IEGLDisplay
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int64 dpy);

    CARAPI Equals(
            /* [in] */ IInterface *object,
            /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

    CARAPI_(Int64) GetEGLDisplay();

private:
    Int64 mEGLDisplay;
    friend class CEGLImpl;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
#endif
