
#ifndef __ELASTOS_DROID_OPENGL_EGLDISPLAY_H__
#define __ELASTOS_DROID_OPENGL_EGLDISPLAY_H__

#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/opengl/EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLDisplayWrapper
    : public EGLObjectHandle
    , public IEGLDisplay
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IEGLDisplay>) CreateInstance(
        /* [in] */ Int64 handle);

private:
    EGLDisplayWrapper(
        /* [in] */ Int64 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_EGLDISPLAY_H__
