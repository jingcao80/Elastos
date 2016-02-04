
#ifndef __ELASTOS_DROID_OPENGL_EGLCONTEXTC_H__
#define __ELASTOS_DROID_OPENGL_EGLCONTEXTC_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLContextWrapper
    : public EGLObjectHandle
    , public IEGLContext
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IEGLContext>) CreateInstance(
        /* [in] */ Int64 handle);

private:
    EGLContextWrapper(
        /* [in] */ Int64 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __CEGLContext_H__
