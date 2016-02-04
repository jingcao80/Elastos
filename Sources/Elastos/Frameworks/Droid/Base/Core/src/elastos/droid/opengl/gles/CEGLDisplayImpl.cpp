#include "elastos/droid/opengl/gles/CEGLDisplayImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

UInt32 CEGLDisplayImpl::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEGLDisplayImpl::Release()
{
    return ElRefBase::Release();
}

PInterface CEGLDisplayImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLDisplay*)this;
    }
    else if (riid == Elastosx::Microedition::Khronos::Egl::EIID_IEGLDisplay) {
        return (Elastosx::Microedition::Khronos::Egl::IEGLDisplay*)this;
    }
    return Object::Probe(riid);
}

ECode CEGLDisplayImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)

    if (object == (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLDisplay*)this) {
        *iid = Elastosx::Microedition::Khronos::Egl::EIID_IEGLDisplay;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

CAR_OBJECT_IMPL(CEGLDisplayImpl)

ECode CEGLDisplayImpl::constructor(
    /* [in] */ Int64 dpy)
{
    mEGLDisplay = dpy;
    return NOERROR;
}

ECode CEGLDisplayImpl::Equals(
        /* [in] */ IInterface *o,
        /* [out] */ Boolean *equals)
{
    VALIDATE_NOT_NULL(equals)

    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLDisplay::Probe(o) == NULL) return FALSE;

    CEGLDisplayImpl* that = (CEGLDisplayImpl*)(IEGLDisplay::Probe(o));

    *equals = mEGLDisplay == that->mEGLDisplay;
    return NOERROR;
}

ECode CEGLDisplayImpl::GetHashCode(
        /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)

        /*
         * Based on the algorithm suggested in
         * http://developer.android.com/reference/java/lang/Object.html
         */
        Int32 result = 17;
        result = 31 * result + (Int32) (mEGLDisplay ^ (mEGLDisplay >> 32));
        *hashCode = result;
        return NOERROR;
}

Int64 CEGLDisplayImpl::GetEGLDisplay()
{
    return mEGLDisplay;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos