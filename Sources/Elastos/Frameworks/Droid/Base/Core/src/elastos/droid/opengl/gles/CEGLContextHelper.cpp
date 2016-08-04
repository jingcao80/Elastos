
#include "elastos/droid/opengl/gles/CEGLContextHelper.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastosx::Microedition::Khronos::Egl::EIID_IEGLContextHelper;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CAR_INTERFACE_IMPL(CEGLContextHelper, Singleton, IEGLContextHelper)

CAR_SINGLETON_IMPL(CEGLContextHelper)

ECode CEGLContextHelper::GetEGL(
    /* [out] */ IEGL** egl)
{
    VALIDATE_NOT_NULL(egl)
    AutoPtr<IEGL> gl = CEGLContextImpl::GetEGL();
    *egl = gl;
    REFCOUNT_ADD(*egl)
    return NOERROR;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

