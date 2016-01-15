#include "gles/CEGLConfigImpl.h"

namespace Elastos {
namespace Droid {
namespace Opengl {
namespace Gles {

ECode CEGLConfigImpl::constructor(
    /* [in] */ Int32 config)
{
    mEGLConfig = config;
    return NOERROR;
}

Int32 CEGLConfigImpl::Get()
{
    return mEGLConfig;
}

Int32 CEGLConfigImpl::GetEGLConfig()
{
    return mEGLConfig;
}

} // namespace Gles
} // namespace Opengl
} // namespace Droid
} // namespace Elastos
