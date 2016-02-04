#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLCONFIGIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLCONFIGIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLConfigImpl.h"
#include "Elastos.Droid.Opengl.h"

#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLConfigImpl)
    , public Object
    , public Elastosx::Microedition::Khronos::Egl::IEGLConfig
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int64 config);

    CARAPI_(Int64) GetEGLConfig();

private:
    Int64 mEGLConfig;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
