#include "CEGL10Helper.h"
#include "CEGLDisplayImpl.h"
#include "CEGLContextImpl.h"
#include "CEGLSurfaceImpl.h"
#include "elastos/droid/ext/frameworkext.h"

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
AutoPtr<Elastosx::Microedition::Khronos::egl::IEGLDisplay> CEGL10Helper::sNoDisplay = InitDisplay();
AutoPtr<Elastosx::Microedition::Khronos::egl::IEGLContext> CEGL10Helper::sNoContext = InitContext();
AutoPtr<Elastosx::Microedition::Khronos::egl::IEGLSurface> CEGL10Helper::sNoSurface = InitSurface();

ECode CEGL10Helper::GetDefaultDisplay(
    /* [out] */ IInterface** disp)
{
    *disp = sDefaultDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoDisplay(
    /* [out] */ XIEGLDisplay** disp)
{
    *disp = sNoDisplay;
    REFCOUNT_ADD(*disp);
    return NOERROR;
}

ECode CEGL10Helper::GetNoContext(
    /* [out] */ XIEGLContext** ctx)
{
    *ctx = sNoContext;
    REFCOUNT_ADD(*ctx);
    return NOERROR;
}

ECode CEGL10Helper::GetNoSurface(
    /* [out] */ XIEGLSurface** sfc)
{
    *sfc = sNoSurface;
    REFCOUNT_ADD(*sfc);
    return NOERROR;
}


} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos