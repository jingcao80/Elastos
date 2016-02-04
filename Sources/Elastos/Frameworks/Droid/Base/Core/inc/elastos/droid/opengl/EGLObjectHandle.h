#ifndef __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
#define __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__

#include <elastos/core/Object.h>
#include "Elastos.Droid.Opengl.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class EGLObjectHandle
    : public Object
    , public IEGLObjectHandle
{
public:
    CAR_INTERFACE_DECL()

    CARAPI GetHandle(
        /* [out] */ Int32* handle);

    CARAPI GetNativeHandle(
        /* [out] */ Int64* handle);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    EGLObjectHandle(
        /* [in] */ Int32 handle);

    EGLObjectHandle(
        /* [in] */ Int64 handle);

private:
    Int64 mHandle;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
