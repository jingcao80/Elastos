
#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXTHELPER_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXTHELPER_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLContextHelper.h"
#include <elastos/core/Singleton.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastosx::Microedition::Khronos::Egl::IEGL;
using Elastosx::Microedition::Khronos::Egl::IEGLContextHelper;
using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLContextHelper)
    , public Singleton
    , public IEGLContextHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetEGL(
        /* [out] */ IEGL** egl);
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_GLES_CEGLCONTEXTHELPER_H__
