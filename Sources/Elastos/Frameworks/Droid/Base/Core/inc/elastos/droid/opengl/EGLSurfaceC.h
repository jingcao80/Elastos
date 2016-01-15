
#ifndef __ELASTOS_DROID_OPENGL_EGLSURFACEC_H__
#define __ELASTOS_DROID_OPENGL_EGLSURFACEC_H__

#include "EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLSurfaceC
    : public EGLObjectHandle
    , public IEGLSurface
    , public ElRefBase
{
public:
    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface* object,
        /* [out] */ InterfaceID* iid);

    CARAPI GetHandle(
        /* [out] */ Int32* handle);

    CARAPI GetHashCode(
        /* [out] */ Int32* code);

    CARAPI Equals(
        /* [in] */ IInterface* o,
        /* [out] */ Boolean* result);

private:
    static CARAPI_(AutoPtr<IEGLSurface>) CreateInstance(
        /* [in] */ Int32 handle);

    EGLSurfaceC(
        /* [in] */ Int32 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __EGLSURFACE_H__
