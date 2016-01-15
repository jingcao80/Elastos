#include "gles/CEGLDisplayImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

ECode CEGLDisplayImpl::constructor(
    /* [in] */ Int32 dpy)
{
    mEGLDisplay = dpy;
    return NOERROR;
}

ECode CEGLDisplayImpl::Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *equals)
{
    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLDisplay::Probe(o) == NULL) return FALSE;

    CEGLDisplayImpl* that = (CEGLDisplayImpl*)(IEGLDisplay::Probe(o));

    *equals = mEGLDisplay == that->mEGLDisplay;
    return NOERROR;
}

ECode CEGLDisplayImpl::GetHashCode(
        /* [out] */ Int32 *hashCode)
{
    *hashCode = mEGLDisplay;
    return NOERROR;
}

Int32 CEGLDisplayImpl::GetEGLDisplay()
{
    return mEGLDisplay;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos