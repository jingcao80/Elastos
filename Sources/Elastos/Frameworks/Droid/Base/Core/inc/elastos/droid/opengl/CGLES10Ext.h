
#ifndef __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
#define __ELASTOS_DROID_OPENGL_CGLES10EXT_H__

#include "_Elastos_Droid_Opengl_CGLES10Ext.h"

using Elastos::IO::IBuffer;

namespace Elastos {
namespace Droid {
namespace Opengl {

CarClass(CGLES10Ext)
{
public:
    CARAPI glQueryMatrixxOES(
        /* [in] */ ArrayOf<Int32>* mantissa_ref,
        /* [in] */ Int32 mantissaOffset,
        /* [in] */ ArrayOf<Int32>* exponent,
        /* [in] */ Int32 exponentOffset,
        /* [out] */ Int32* matrixx);

    CARAPI glQueryMatrixxOES(
        /* [in] */ Elastos::IO::IInt32Buffer* mantissa_buf,
        /* [in] */ Elastos::IO::IInt32Buffer* exponent_buf,
        /* [out] */ Int32* matrixx);

private:
    CARAPI GetPointer(
        /* [in] */ IBuffer* buffer,
        /* [in, out] */ Handle32* array,
        /* [in, out] */ Int32* remaining,
        /* [in, out] */ Int32* offset,
        /* [out] */ Handle32* rst);
};

} // namespace Opengl
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_OPENGL_CGLES10EXT_H__
