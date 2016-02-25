
#ifndef __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__
#define __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__

#include "_Elastos.Droid.Server.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Utility::ISparseArray;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Hdmi {

/**
 * Unmodifiable version of {@link SparseArray}.
 */
class UnmodifiableSparseArray
    : public Object
{
public:
    CARAPI constructor(
        /* [in] */ ISparseArray* array);

    CARAPI GetSize(
        /* [out] */ Int32* result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [out] */ IInterface** result);

    CARAPI Get(
        /* [in] */ Int32 key,
        /* [in] */ IInterface* valueIfKeyNotFound,
        /* [out] */ IInterface** result);

    CARAPI KeyAt(
        /* [in] */ Int32 index,
        /* [out] */ Int32* result);

    CARAPI ValueAt(
        /* [in] */ Int32 index,
        /* [out] */ IInterface** result);

    CARAPI IndexOfValue(
        /* [in] */ IInterface* value,
        /* [out] */ Int32* result);

    // @Override
    CARAPI ToString(
        /* [out] */ String* result);

private:
    static const String TAG;

    AutoPtr<ISparseArray> mArray;
};

} // namespace Hdmi
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_HDMI_UNMODIFIABLESPARSEARRAY_H__
