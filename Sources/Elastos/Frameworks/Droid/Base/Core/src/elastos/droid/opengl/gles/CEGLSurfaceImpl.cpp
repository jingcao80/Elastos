#include "elastos/droid/opengl/gles/CEGLSurfaceImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

UInt32 CEGLSurfaceImpl::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEGLSurfaceImpl::Release()
{
    return ElRefBase::Release();
}

PInterface CEGLSurfaceImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLSurface*)this;
    }
    else if (riid == Elastosx::Microedition::Khronos::Egl::EIID_IEGLSurface) {
        return (Elastosx::Microedition::Khronos::Egl::IEGLSurface*)this;
    }
    return Object::Probe(riid);
}

ECode CEGLSurfaceImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)

    if (object == (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLSurface*)this) {
        *iid = Elastosx::Microedition::Khronos::Egl::EIID_IEGLSurface;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

CAR_OBJECT_IMPL(CEGLSurfaceImpl)

ECode CEGLSurfaceImpl::constructor()
{
    mEGLSurface = 0;
    mNativePixelRef = 0;
    return NOERROR;
}

ECode CEGLSurfaceImpl::constructor(
    /* [in] */ Int64 surface)
{
    mEGLSurface = surface;
    mNativePixelRef = 0;
    return NOERROR;
}

ECode CEGLSurfaceImpl::Equals(
    /* [in] */ IInterface *o,
    /* [out] */ Boolean *equals)
{
    VALIDATE_NOT_NULL(equals)
    if (Probe(EIID_IInterface) == o) return TRUE;
    if (o == NULL || IEGLSurface::Probe(o) == NULL) return FALSE;

    CEGLSurfaceImpl* that = (CEGLSurfaceImpl*)(IEGLSurface::Probe(o));

    *equals = (mEGLSurface == that->mEGLSurface);
    return NOERROR;
}

ECode CEGLSurfaceImpl::GetHashCode(
    /* [out] */ Int32 *hashCode)
{
    VALIDATE_NOT_NULL(hashCode)
    *hashCode = mEGLSurface;
    return NOERROR;
}

Int64 CEGLSurfaceImpl::GetEGLSurface()
{
    return mEGLSurface;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos