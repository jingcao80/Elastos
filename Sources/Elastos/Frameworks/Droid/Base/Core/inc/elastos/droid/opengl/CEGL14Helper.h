#ifndef __ELASTOS_DROID_OPENGL_CEGL14HELPER_H__
#define __ELASTOS_DROID_OPENGL_CEGL14HELPER_H__

#include "Elastos.Droid.Opengl.h"
#include "_Elastos_Droid_Opengl_CEGL14Helper.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CEGL14Helper)
    , public Singleton
    , public IEGL14Helper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetNoContext(
        /* [out] */ IEGLContext** cxt);

    CARAPI SetNoContext(
        /* [in] */ IEGLContext* cxt);

    CARAPI GetNoDisplay(
        /* [out] */ IEGLDisplay** dsp);

    CARAPI SetNoDisplay(
        /* [in] */ IEGLDisplay* dsp);

    CARAPI GetNoSurface(
        /* [out] */ IEGLSurface** sfc);

    CARAPI SetNoSurface(
        /* [in] */ IEGLSurface* sfc);
};

}// namespace Opengl
}// namespace Droid
}// namespace Elastos

#endif
