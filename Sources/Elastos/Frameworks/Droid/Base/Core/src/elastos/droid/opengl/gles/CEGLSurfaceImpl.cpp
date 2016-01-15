#include "CEGLSurfaceImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

ECode CEGLSurfaceImpl::constructor()
{
    mEGLSurface = 0;
    mNativePixelRef = 0;
    return NOERROR;
}

ECode CEGLSurfaceImpl::constructor(
    /* [in] */ Int32 surface)
{
    mEGLSurface = surface;
    mNativePixelRef = 0;
    return NOERROR;
}

ECode CEGLSurfaceImpl::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *equals)
{
    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLSurface::Probe(o) == NULL) return FALSE;

    CEGLSurfaceImpl* that = (CEGLSurfaceImpl*)(IEGLSurface::Probe(o));

    *equals = (mEGLSurface == that->mEGLSurface);
    return NOERROR;
}

ECode CEGLSurfaceImpl::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    *hashCode = mEGLSurface;
    return NOERROR;
}

Int32 CEGLSurfaceImpl::GetEGLSurface()
{
    return mEGLSurface;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos