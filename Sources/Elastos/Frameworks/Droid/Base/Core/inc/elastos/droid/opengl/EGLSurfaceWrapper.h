
#ifndef __ELASTOS_DROID_OPENGL_EGLSURFACEC_H__
#define __ELASTOS_DROID_OPENGL_EGLSURFACEC_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLSurfaceWrapper
    : public EGLObjectHandle
    , public IEGLSurface
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IEGLSurface>) CreateInstance(
        /* [in] */ Int64 handle);

    EGLSurfaceWrapper(
        /* [in] */ Int64 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __EGLSURFACE_H__
