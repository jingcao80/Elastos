#include "CEGL14Helper.h"
#include "CEGL14.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

ECode CEGL14Helper::GetNoContext(
    /* [out] */ IEGLContext** cxt)
{
    *cxt = CEGL14::eglNoContextObject;
    REFCOUNT_ADD(*cxt)
    return NOERROR;
}

ECode CEGL14Helper::SetNoContext(
    /* [in] */ IEGLContext* cxt)
{
    CEGL14::eglNoContextObject = cxt;
    return NOERROR;
}

ECode CEGL14Helper::GetNoDisplay(
    /* [out] */ IEGLDisplay** dsp)
{
    *dsp = CEGL14::eglNoDisplayObject;
    REFCOUNT_ADD(*dsp)
    return NOERROR;
}

ECode CEGL14Helper::SetNoDisplay(
    /* [in] */ IEGLDisplay* dsp)
{
    CEGL14::eglNoDisplayObject = dsp;
    return NOERROR;
}

ECode CEGL14Helper::GetNoSurface(
    /* [out] */ IEGLSurface** sfc)
{
    *sfc = CEGL14::eglNoSurfaceObject;
    REFCOUNT_ADD(*sfc)
    return NOERROR;
}

ECode CEGL14Helper::SetNoSurface(
    /* [in] */ IEGLSurface* sfc)
{
    CEGL14::eglNoSurfaceObject = sfc;
    return NOERROR;
}

}// namespace Opengl
}// namespace Droid
}// namespace Elastos