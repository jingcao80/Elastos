#ifndef __ELASTOS_DROID_OPENGL_GLES_CEGLCONFIGIMPL_H__
#define __ELASTOS_DROID_OPENGL_GLES_CEGLCONFIGIMPL_H__

#include "_Elastos_Droid_Opengl_Gles_CEGLConfigImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

CarClass(CEGLConfigImpl)
{
public:
    CARAPI constructor(
        /* [in] */ Int32 config);

    CARAPI_(Int32) Get();

    CARAPI_(Int32) GetEGLConfig();

private:
    Int32 mEGLConfig;
};

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif
