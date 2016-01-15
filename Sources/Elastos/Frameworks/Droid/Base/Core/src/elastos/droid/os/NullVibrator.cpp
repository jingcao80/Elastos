
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/os/NullVibrator.h"

namespace Elastos {
namespace Droid {
namespace Os {

static AutoPtr<NullVibrator> sInstance;

NullVibrator::NullVibrator()
{}


AutoPtr<NullVibrator> NullVibrator::GetInstance()
{
    if (sInstance == NULL) {
        sInstance = new NullVibrator();
        sInstance->constructor();
    }
    return sInstance;
}

ECode NullVibrator::HasVibrator(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;
    return NOERROR;
}

ECode NullVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* attributes)
{
    return Vibrate(milliseconds);
}

ECode NullVibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IAudioAttributes* attributes)
{
    if (repeat >= pattern->GetLength()) {
        // throw new ArrayIndexOutOfBoundsException();
        return E_ARRAY_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    return NOERROR;
}

ECode NullVibrator::Cancel()
{
    return NOERROR;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
