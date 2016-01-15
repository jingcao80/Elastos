
#ifndef __ELASTOS_DROID_OS_NULLVIBRATOR_H__
#define __ELASTOS_DROID_OS_NULLVIBRATOR_H__

#include "elastos/droid/os/Vibrator.h"

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Vibrator implementation that does nothing.
 *
 * @hide
 */
class NullVibrator
    : public Vibrator
{
private:
    NullVibrator();

public:
    static AutoPtr<NullVibrator> GetInstance();

    CARAPI HasVibrator(
        /* [out] */ Boolean* result);

    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ Int64 milliseconds,
        /* [in] */ IAudioAttributes* attributes);

    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ ArrayOf<Int64>* milliseconds,
        /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes);

    using Vibrator::Vibrate;

    CARAPI Cancel();
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_NULLVIBRATOR_H__
