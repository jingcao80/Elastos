#include "elastos/droid/opengl/gles/CEGL10Helper.h"
#include "elastos/droid/opengl/gles/CEGLDisplayImpl.h"
#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "elastos/droid/opengl/gles/CEGLSurfaceImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastosx::Microedition::Khronos::Egl::EIID_IEGL10Helper;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static AutoPtr<CEGLDisplayImpl> InitDisplay()
{
    AutoPtr<CEGLDisplayImpl> display;
    CEGLDisplayImpl::NewByFriend(0, (CEGLDisplayImpl**)&display);
    return display;
}

static AutoPtr<CEGLContextImpl> InitContext()
{
    AutoPtr<CEGLContextImpl> display;
    CEGLContextImpl::NewByFriend(0, (CEGLContextImpl**)&display);
    return display;
}

static AutoPtr<CEGLSurfaceImpl> InitSurface()
{
    AutoPtr<CEGLSurfaceImpl> display;
    CEGLSurfaceImpl::NewByFriend(0, (CEGLSurfaceImpl**)&display);
    return display;
}


AutoPtr<IInterface> CEGL10Helper::sDefaultDisplay = NULL;
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLDisplay> CEGL10Helper::sNoDisplay = InitDisplay().Get();
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLContext> CEGL10Helper::sNoContext = InitContext().Get();
AutoPtr<Elastosx::Microedition::Khronos::Egl::IEGLSurface> CEGL10Helper::sNoSurface = InitSurface().Get();

CAR_INTERFACE_IMPL(CEGL10Helper, Singleton, IEGL10Helper)
CAR_SINGLETON_IMPL(CEGL10Helper)

ECode CEGL10Helper::GetDefaultDisplay(
    /* [out] */ IInterface** disp)
{
    VALIDATE_NOT_NULL(disp)

    *disp = sDefaultDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoDisplay(
    /* [out] */ XIEGLDisplay** disp)
{
    VALIDATE_NOT_NULL(disp)

    *disp = sNoDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoContext(
    /* [out] */ XIEGLContext** ctx)
{
    VALIDATE_NOT_NULL(ctx)

    *ctx = sNoContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CEGL10Helper::GetNoSurface(
    /* [out] */ XIEGLSurface** sfc)
{
    VALIDATE_NOT_NULL(sfc)

    *sfc = sNoSurface;
    REFCOUNT_ADD(*sfc);
    return NOERROR;
}


} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos