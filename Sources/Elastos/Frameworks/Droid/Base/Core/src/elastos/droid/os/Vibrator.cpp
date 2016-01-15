
#include "Elastos.Droid.Content.h"
#include "elastos/droid/os/Vibrator.h"
#include "elastos/droid/os/Process.h"
// #include "elastos/droid/app/CActivityThread.h"

// using Elastos::Droid::App::CActivityThread;

namespace Elastos {
namespace Droid {
namespace Os {

CAR_INTERFACE_IMPL(Vibrator, Object, IVibrator)

Vibrator::Vibrator()
{
}

ECode Vibrator::constructor()
{
    //mPackageName = CActivityThread::CurrentPackageName();
    return NOERROR;
}

ECode Vibrator::constructor(
    /* [in] */ IContext* context)
{
    return context->GetOpPackageName(&mPackageName);
}

ECode Vibrator::HasVibrator(
    /* [out] */ Boolean* result)
{
    assert(0 && "abstract base methods");
    return E_NOT_IMPLEMENTED;
}

ECode Vibrator::Vibrate(
    /* [in] */ Int64 milliseconds)
{
    return Vibrate(milliseconds, NULL);
}

ECode Vibrator::Vibrate(
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* aa)
{
    Int32 uid = 0;// Process::MyUid();
    assert(0);
    return Vibrate(uid, mPackageName, milliseconds, aa);
}

ECode Vibrator::Vibrate(
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat)
{
    return Vibrate(pattern, repeat, NULL);
}

ECode Vibrator::Vibrate(
    /* [in] */ ArrayOf<Int64>* pattern,
    /* [in] */ Int32 repeat,
    /* [in] */ IAudioAttributes* aa)
{
    Int32 uid = 0;// Process::MyUid();
    assert(0);
    return Vibrate(uid, mPackageName, pattern, repeat, aa);
}

ECode Vibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ Int64 milliseconds,
    /* [in] */ IAudioAttributes* attributes)
{
    assert(0 && "abstract base methods");
    return E_NOT_IMPLEMENTED;
}

ECode Vibrator::Vibrate(
    /* [in] */ Int32 uid,
    /* [in] */ const String& opPkg,
    /* [in] */ ArrayOf<Int64>* milliseconds,
    /* [in] */ Int32 repeat,
    /* [in] */ IAudioAttributes* attributes)
{
    assert(0 && "abstract base methods");
    return E_NOT_IMPLEMENTED;
}

ECode Vibrator::Cancel()
{
    assert(0 && "abstract base methods");
    return E_NOT_IMPLEMENTED;
}

} // namespace Os
} // namespace Droid
} // namespace Elastos
