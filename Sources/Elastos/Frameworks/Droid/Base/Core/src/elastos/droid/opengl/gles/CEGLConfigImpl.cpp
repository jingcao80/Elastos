#include "elastos/droid/opengl/gles/CEGLConfigImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

UInt32 CEGLConfigImpl::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 CEGLConfigImpl::Release()
{
    return ElRefBase::Release();
}

PInterface CEGLConfigImpl::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_IInterface) {
        return (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this;
    }
    else if (riid == Elastosx::Microedition::Khronos::Egl::EIID_IEGLConfig) {
        return (Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this;
    }
    return Object::Probe(riid);
}

ECode CEGLConfigImpl::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid)

    if (object == (IInterface*)(Elastosx::Microedition::Khronos::Egl::IEGLConfig*)this) {
        *iid = Elastosx::Microedition::Khronos::Egl::EIID_IEGLConfig;
        return NOERROR;
    }
    return Object::GetInterfaceID(object, iid);
}

CAR_OBJECT_IMPL(CEGLConfigImpl)

ECode CEGLConfigImpl::constructor(
    /* [in] */ Int64 config)
{
    mEGLConfig = config;
    return NOERROR;
}

Int64 CEGLConfigImpl::GetEGLConfig()
{
    return mEGLConfig;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
