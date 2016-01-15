#ifndef __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__
#define __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__

#include "Elastos.Droid.Core_server.h"
#include "elastos/droid/ext/frameworkext.h"

using Elastosx::Microedition::Khronos::Opengles::IGL;
using Elastosx::Microedition::Khronos::Opengles::IGL10;
using Elastosx::Microedition::Khronos::Opengles::IGL10Ext;
using Elastosx::Microedition::Khronos::Opengles::IGL11;
using Elastosx::Microedition::Khronos::Opengles::IGL11Ext;
using Elastosx::Microedition::Khronos::Opengles::IGL11ExtensionPack;

namespace Elastos {
namespace Droid {
namespace Opengl {

class GLWrapperBase
    : public IGL11
    , public IGL10Ext
    , public IGL11Ext
    , public IGL11ExtensionPack
    , public ElRefBase
{
public:
    GLWrapperBase(
        /* [in] */ IGL* gl);

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

protected:
    AutoPtr<IGL10> mgl;
    AutoPtr<IGL10Ext> mgl10Ext;
    AutoPtr<IGL11> mgl11;
    AutoPtr<IGL11Ext> mgl11Ext;
    AutoPtr<IGL11ExtensionPack> mgl11ExtensionPack;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
