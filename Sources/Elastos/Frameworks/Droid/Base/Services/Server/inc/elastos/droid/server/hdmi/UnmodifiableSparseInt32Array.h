
#ifndef __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__
#define __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Utility::ISparseInt32Array;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Unmodifiable version of {@link SparseIntArray}.
 */
class UnmodifiableSparseInt32Array
    : public Object
{
public:
    CARAPI constructor(
        /* [in] */ ISparseInt32Array* array);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ Int32 valueIfKeyNotFound,
        /* [out] */ Int32* result);

    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI IndexOfValue(
        /* [in] */ Int32 value,
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static const String TAG;

    AutoPtr<ISparseInt32Array> mArray;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEINTARRAY_H__
