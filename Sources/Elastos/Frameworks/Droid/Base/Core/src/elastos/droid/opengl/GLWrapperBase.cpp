#include "GLWrapperBase.h"

using Elastosx::Microedition::Khronos::Opengles::EIID_IGL;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11ExtensionPack;

namespace Elastos {
namespace Droid {
namespace Opengl {

PInterface GLWrapperBase::Probe(
    /* [in] */ REIID riid)
{
    if ( riid == EIID_IInterface) {
        return (IInterface*)(IGL11 *)this;
    } else if ( riid == EIID_IGL11 ) {
        return (IGL11 *)this;
    } else if ( riid == EIID_IGL10) {
        return (IGL10*)(IGL11 *)this;
    } else if ( riid == EIID_IGL) {
        return (IGL*)(IGL11 *)this;
    } else if ( riid == EIID_IGL10Ext) {
        return (IInterface*)(IGL10Ext *)this;
    } else if ( riid == EIID_IGL11Ext) {
        return (IInterface*)(IGL11Ext *)this;
    } else if ( riid == EIID_IGL11ExtensionPack) {
        return (IInterface*)(IGL11ExtensionPack *)this;
    }

    return NULL;
}

UInt32 GLWrapperBase::AddRef()
{
    return ElRefBase::AddRef();
}

UInt32 GLWrapperBase::Release()
{
    return ElRefBase::Release();
}

ECode GLWrapperBase::GetInterfaceID(
    /* [in] */ IInterface* object,
    /* [out] */ InterfaceID* iid)
{
    VALIDATE_NOT_NULL(iid);
    if (object == (IInterface*)(IGL11 *)this) {
        *iid = EIID_IGL11 ;
    } else if (object == (IInterface*)(IGL10Ext *)this) {
        *iid = EIID_IGL10Ext ;
    } else if (object == (IInterface*)(IGL11Ext *)this) {
        *iid = EIID_IGL11Ext ;
    } else if (object == (IInterface*)(IGL11ExtensionPack *)this) {
        *iid = EIID_IGL11ExtensionPack ;
    } else{
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

GLWrapperBase::GLWrapperBase(
    /* [in] */ IGL* gl)
{
    mgl = IGL10::Probe(gl);
    if (IGL10Ext::Probe(gl) != NULL) {
        mgl10Ext = IGL10Ext::Probe(gl);
    }
    if (IGL11::Probe(gl) != NULL) {
        mgl11 = IGL11::Probe(gl);
    }
    if (IGL11Ext::Probe(gl) != NULL) {
        mgl11Ext = IGL11Ext::Probe(gl);
    }
    if (IGL11ExtensionPack::Probe(gl) != NULL) {
        mgl11ExtensionPack = IGL11ExtensionPack::Probe(gl);
    }
}


} // namespace Opengl
} // namespace Droid
} // namespace Elastos

