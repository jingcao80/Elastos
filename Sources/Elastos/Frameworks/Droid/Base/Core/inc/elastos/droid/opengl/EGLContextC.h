
#ifndef __ELASTOS_DROID_OPENGL_EGLCONTEXTC_H__
#define __ELASTOS_DROID_OPENGL_EGLCONTEXTC_H__

#include "EGLObjectHandle.h"

namespace Elastos {
namespace Droid {
namespace Opengl {

class CEGL14;

class EGLContextC
    : public EGLObjectHandle
    , public IEGLContext
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
    static CARAPI_(AutoPtr<IEGLContext>) CreateInstance(
        /* [in] */ Int32 handle);

private:
    EGLContextC(
        /* [in] */ Int32 handle);

    friend class CEGL14;
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __CEGLContext_H__
