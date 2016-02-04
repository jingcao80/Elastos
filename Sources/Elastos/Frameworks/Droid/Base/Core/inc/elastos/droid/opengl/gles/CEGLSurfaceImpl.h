
#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_Gles_CEGLSurfaceImpl.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {
class CEGLImpl;

CarClass(CEGLSurfaceImpl)
    , public Object
    , public Elastosx::Microedition::Khronos::Egl::IEGLSurface
{
    friend class CEGLImpl;
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Int64 surface);

    CARAPI Equals(
        /* [in] */ IInterface *object,
        /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
        /* [out] */ Int32 *hashCode);

    CARAPI_(Int64) GetEGLSurface();

private:
    Int64 mEGLSurface;
    Int64 mNativePixelRef;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_GLES_CEGLSURFACEIMPL_H__
