
#ifndef __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
#define __ELASTOS_DROID_OPENGL_CGLES10EXT_H__

#include "Elastos.Droid.Opengl.h"
#include "Elastos.CoreLibrary.IO.h"
#include "_Elastos_Droid_Opengl_CGLES10Ext.h"
#include <elastos/core/Singleton.h>

using Elastos::Core::Singleton;
using Elastos::IO::IBuffer;
using Elastos::IO::IInt32Buffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES10Ext)
    , public Singleton
    , public IGLES10Ext
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GlQueryMatrixxOES(
        /* [in] */ ArrayOf<Int32>* mantissa_ref,
        /* [in] */ Int32 mantissaOffset,
        /* [in] */ ArrayOf<Int32>* exponent,
        /* [in] */ Int32 exponentOffset,
        /* [out] */ Int32* matrixx);

    CARAPI GlQueryMatrixxOES(
        /* [in] */ Elastos::IO::IInt32Buffer* mantissa_buf,
        /* [in] */ Elastos::IO::IInt32Buffer* exponent_buf,
        /* [out] */ Int32* matrixx);

private:
    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ Handle64* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ Handle64* rst);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
