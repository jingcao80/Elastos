#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLContextImpl.h"

using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Opengles::IGL;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {
class CEGLImpl;
CarClass(CEGLContextImpl)
{
public:
    static CARAPI_(AutoPtr<IEGL>) GetEGL();

    CARAPI GetGL(
        /* [in] */ IGL** gl);

    CARAPI Equals(
            /* [in] */ IInterface *object,
            /* [out] */ Boolean *equals);

    CARAPI GetHashCode(
            /* [out] */ Int32 *hashCode);

    CARAPI constructor(
        /* [in] */ Int32 ctx);

    CARAPI_(Int32) GetEGLContext();

protected:
    Int32 mEGLContext;

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
