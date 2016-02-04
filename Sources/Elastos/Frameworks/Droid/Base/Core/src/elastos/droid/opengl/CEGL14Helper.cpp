#include "elastos/droid/opengl/CEGL14Helper.h"
#include "elastos/droid/opengl/CEGL14.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

CAR_INTERFACE_IMPL(CEGL14Helper, Singleton, IEGL14Helper)

CAR_SINGLETON_IMPL(CEGL14Helper)

ECode CEGL14Helper::GetNoContext(
    /* [out] */ IEGLContext** cxt)
{
    VALIDATE_NOT_NULL(cxt)

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
    VALIDATE_NOT_NULL(dsp)

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
    VALIDATE_NOT_NULL(sfc)

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
