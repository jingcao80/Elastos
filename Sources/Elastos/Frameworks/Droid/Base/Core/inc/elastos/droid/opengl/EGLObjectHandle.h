#ifndef __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
#define __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

extern "C" const InterfaceID EIID_EGLObjectHandle;

class EGLObjectHandle
{
public:
    virtual CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid) = 0;

    CARAPI GetHandle(
        /* [out] */ Int32* handle);

    CARAPI GetHashCode(
        /* [out] */ Int32* hashCode);

protected:
    EGLObjectHandle(
        /* [in] */ Int32 handle);

private:
    Int32 mHandle;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_EGLOBJECTHANDLE_H__
