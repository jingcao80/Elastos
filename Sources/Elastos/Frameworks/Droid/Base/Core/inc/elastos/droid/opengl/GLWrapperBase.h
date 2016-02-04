#ifndef __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__
#define __ELASTOS_DROID_OPENGL_GLWRAPPERBASE_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

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
    : public Object
    , public IGL
    , public IGL10
    , public IGL10Ext
    , public IGL11
    , public IGL11Ext
    , public IGL11ExtensionPack
{
public:
    GLWrapperBase(
        /* [in] */ IGL* gl);

    CAR_INTERFACE_DECL()

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
