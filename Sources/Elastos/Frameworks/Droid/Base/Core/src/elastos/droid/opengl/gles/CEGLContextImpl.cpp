#include "elastos/droid/opengl/gles/CEGLContextImpl.h"
#include "elastos/droid/opengl/gles/CGLImpl.h"
#include "elastos/droid/opengl/gles/CEGLImpl.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastos::Droid::Opengl::Gles::CEGLImpl;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastosx::Microedition::Khronos::Opengles::IGL11;
using Elastosx::Microedition::Khronos::Egl::EIID_IEGLContext;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

static AutoPtr<IEGL> InitStatic()
{
    AutoPtr<IEGL> tmp;
    CEGLImpl::New((IEGL**)&tmp);
    return tmp;
}

AutoPtr<IEGL> CEGLContextImpl::EGL_INSTANCE = InitStatic();

UInt32 CEGLContextImpl::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEGLContextImpl::Release()
{
    return ElRefBase::Release();
}

PInterface CEGLContextImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLContext*)this;
    }
    else if (riid == Elastosx::Microedition::Khronos::Egl::EIID_IEGLContext) {
        return (Elastosx::Microedition::Khronos::Egl::IEGLContext*)this;
    }
    return Object::Probe(riid);
}

ECode CEGLContextImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)

    if (object == (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLContext*)this) {
        *iid = Elastosx::Microedition::Khronos::Egl::EIID_IEGLContext;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

CAR_OBJECT_IMPL(CEGLContextImpl)

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
    VALIDATE_NOT_NULL(equals)

    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLContext::Probe(o) == NULL) return FALSE;

    CEGLContextImpl* that = (CEGLContextImpl*)(IEGLContext::Probe(o));

    *equals = mEGLContext == that->mEGLContext;
    return NOERROR;
}

ECode CEGLContextImpl::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

    /*
     * Based on the algorithm suggested in
     * http://developer.android.com/reference/java/lang/Object.html
     */
    Int32 result = 17;
    result = 31 * result + (Int32) (mEGLContext ^ (mEGLContext >> 32));
    *hashCode = result;
    return NOERROR;
}

ECode CEGLContextImpl::constructor(
    /* [in] */ Int64 ctx)
{
    mEGLContext = ctx;
    AutoPtr<IGL11> tmp11;
    CGLImpl::New((IGL11**)&tmp11);
    mGLContext = IGL::Probe(tmp11);
    return NOERROR;
}

Int64 CEGLContextImpl::GetEGLContext()
{
    return mEGLContext;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

