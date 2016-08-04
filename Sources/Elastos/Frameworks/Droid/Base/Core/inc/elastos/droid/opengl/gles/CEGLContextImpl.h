#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLContextImpl.h"
#include <elastos/core/Object.h>

using Elastosx::Microedition::Khronos::Egl::IEGLContext;
using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLContextImpl)
    , public Object
    , public Elastosx::Microedition::Khronos::Egl::IEGLContext
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    static CARAPI_(AutoPtr<IEGL>) GetEGL();

    CARAPI GetGL(
        /* [in] */ IGL** gl);

    CARAPI Equals(
            /* [in] */ IInterface *object,
            /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ Int64 ctx);

    CARAPI_(Int64) GetEGLContext();

protected:
    Int64 mEGLContext;

private:
    static AutoPtr<IEGL> EGL_INSTANCE;
    AutoPtr<IGL> mGLContext;
    friend class CEGLImpl;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos


#endif
