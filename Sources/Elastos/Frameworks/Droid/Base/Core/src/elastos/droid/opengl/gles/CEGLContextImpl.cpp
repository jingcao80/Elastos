#include "gles/CEGLContextImpl.h"
#include "gles/CGLImpl.h"
#include "gles/CEGLImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Opengl::gles::CEGLImpl;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastosx::Microedition::Khronos::Opengles::IGL11;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static AutoPtr<IEGL> InitStatic()
{
    AutoPtr<CEGLImpl> tmp;
    CEGLImpl::NewByFriend((CEGLImpl**)&tmp);
    return tmp;
}

AutoPtr<IEGL> CEGLContextImpl::EGL_INSTANCE = InitStatic();

ECode CEGLContextImpl::GetGL(
    /* [in] */ IGL** gl)
{
    *gl = mGLContext;
    REFCOUNT_ADD(*gl);
    return NOERROR;
}

AutoPtr<IEGL> CEGLContextImpl::GetEGL()
{
    return EGL_INSTANCE;
}

ECode CEGLContextImpl::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *equals)
{
    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLContext::Probe(o) == NULL) return FALSE;

    CEGLContextImpl* that = (CEGLContextImpl*)(IEGLContext::Probe(o));

    *equals = mEGLContext == that->mEGLContext;
    return NOERROR;
}

ECode CEGLContextImpl::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    *hashCode = mEGLContext;
    return NOERROR;
}

ECode CEGLContextImpl::constructor(
    /* [in] */ Int32 ctx)
{
    mEGLContext = ctx;
    AutoPtr<IGL11> tmp11;
    CGLImpl::New((IGL11**)&tmp11);
    mGLContext = tmp11;
    return NOERROR;
}

Int32 CEGLContextImpl::GetEGLContext()
{
    return mEGLContext;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

