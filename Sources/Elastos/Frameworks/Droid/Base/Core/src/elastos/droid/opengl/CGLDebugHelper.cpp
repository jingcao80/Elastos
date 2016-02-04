
#include "elastos/droid/opengl/CGLDebugHelper.h"
#include "elastos/droid/opengl/GLErrorWrapper.h"
#include "elastos/droid/opengl/GLLogWrapper.h"
#include "elastos/droid/opengl/EGLLogWrapper.h"
#include "elastos/droid/ext/frameworkext.h"

#include "Elastos.CoreLibrary.IO.h"

using Elastosx::Microedition::Khronos::Opengles::EIID_IGL;

namespace Elastos {
namespace Droid {
namespace Opengl {
CAR_INTERFACE_IMPL(CGLDebugHelper, Singleton, IGLDebugHelper)

CAR_SINGLETON_IMPL(CGLDebugHelper)

ECode CGLDebugHelper::Wrap(
    /* [in] */ IGL* gl,
    /* [in] */ Int32 configFlags,
    /* [in] */ Elastos::IO::IWriter* log,
    /* [out] */ IGL** glInstance)
{
    VALIDATE_NOT_NULL(glInstance)

    AutoPtr<IGL> rst;
    if ( configFlags != 0 ) {
        rst = (IGL*)(new GLErrorWrapper(gl, configFlags))->Probe(EIID_IGL);
    }
    if ( log != NULL ) {
        Boolean logArgumentNames =
            (IGLDebugHelper::_CONFIG_LOG_ARGUMENT_NAMES & configFlags) != 0;
        rst = (IGL*)(new GLLogWrapper(gl, log, logArgumentNames))->Probe(EIID_IGL);
    }
    *glInstance = rst;
    REFCOUNT_ADD(*glInstance)
    return NOERROR;
}

ECode CGLDebugHelper::Wrap(
    /* [in] */ IEGL* egl,
    /* [in] */ Int32 configFlags,
    /* [in] */ Elastos::IO::IWriter* log,
    /* [out] */ IEGL** eglInterface)
{
    VALIDATE_NOT_NULL(eglInterface)

    if (log != NULL) {
        IEGL11* tmp = new EGLLogWrapper(egl, configFlags, log);
        *eglInterface = IEGL::Probe(tmp);
    }
    REFCOUNT_ADD(*eglInterface)
    return NOERROR;
}

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

