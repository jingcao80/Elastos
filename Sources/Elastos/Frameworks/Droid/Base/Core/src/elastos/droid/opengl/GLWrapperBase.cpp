#include "elastos/droid/opengl/GLWrapperBase.h"

using Elastosx::Microedition::Khronos::Opengles::EIID_IGL;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::EIID_IGL11ExtensionPack;

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL_6(GLWrapperBase, Object, IGL, IGL10, IGL10Ext, IGL11, IGL11Ext, IGL11ExtensionPack)

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

