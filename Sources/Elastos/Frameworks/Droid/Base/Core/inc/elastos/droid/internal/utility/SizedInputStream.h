#ifndef __ELASTOS_DROID_INTERNAL_UTILITY_SIZEDINPUTSTREAM_H__
#define __ELASTOS_DROID_INTERNAL_UTILITY_SIZEDINPUTSTREAM_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/core/Object.h>
#include <elastos/io/InputStream.h>

using Elastos::IO::InputStream;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Utility {

class SizedInputStream
    : public InputStream
    , public ISizedInputStream
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IInputStream* wrapped,
        /* [in] */ Int64 length);

    // @Override
    CARAPI Close();

    // @Override
    CARAPI Read(
        /* [out] */ Int32* num);

    // @Override
    CARAPI Read(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 byteOffset,
        /* [in] */ Int32 byteCount,
        /* [out] */ Int32* num);

private:
    AutoPtr<IInputStream> mWrapped;
    Int64 mLength;
};

} // namespace Utility
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_UTILITY_SIZEDINPUTSTREAM_H__
