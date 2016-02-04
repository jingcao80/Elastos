
#ifndef __ELASTOS_DROID_OPENGL_EGLCONFIGC_H__
#define __ELASTOS_DROID_OPENGL_EGLCONFIGC_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLConfigWrapper
    : public EGLObjectHandle
    , public IEGLConfig
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IEGLConfig>) CreateInstance(
        /* [in] */ Int64 handle);

    EGLConfigWrapper(
        /* [in] */ Int64 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __CEGLConfigC_H__
