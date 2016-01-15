
#ifndef __ELASTOS_DROID_OS_VIBRATOR_H__
#define __ELASTOS_DROID_OS_VIBRATOR_H__

#include "Elastos.Droid.Os.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioAttributes;

namespace Elastos {
namespace Droid {
namespace Os {

/**
 * Class that operates the vibrator on the device.
 * <p>
 * If your process exits, any vibration you started will stop.
 * </p>
 *
 * To obtain an instance of the system vibrator, call
 * {@link Context#getSystemService} with {@link Context#VIBRATOR_SERVICE} as the argument.
 */
class Vibrator
    : public Object
    , public IVibrator
{
public:
    CAR_INTERFACE_DECL()

protected:
    /**
     * @hide to prevent subclassing from outside of the framework
     */
    Vibrator();

    CARAPI constructor();

    /**
     * @hide to prevent subclassing from outside of the framework
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

public:
    /**
     * Check whether the hardware has a vibrator.
     *
     * @return True if the hardware has a vibrator, else false.
     */
    CARAPI HasVibrator(
        /* [out] */ Boolean* result);

    /**
     * Vibrate constantly for the specified period of time.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#VIBRATE}.
     *
     * @param milliseconds The number of milliseconds to vibrate.
     */
    CARAPI Vibrate(
        /* [in] */ Int64 milliseconds);

    /**
     * Vibrate constantly for the specified period of time.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#VIBRATE}.
     *
     * @param milliseconds The number of milliseconds to vibrate.
     * @param attributes {@link AudioAttributes} corresponding to the vibration. For example,
     *        specify {@link AudioAttributes#USAGE_ALARM} for alarm vibrations or
     *        {@link AudioAttributes#USAGE_NOTIFICATION_RINGTONE} for
     *        vibrations associated with incoming calls.
     */
    CARAPI Vibrate(
        /* [in] */ Int64 milliseconds,
        /* [in] */ IAudioAttributes* aa);

    /**
     * Vibrate with a given pattern.
     *
     * <p>
     * Pass in an array of ints that are the durations for which to turn on or off
     * the vibrator in milliseconds.  The first value indicates the number of milliseconds
     * to wait before turning the vibrator on.  The next value indicates the number of milliseconds
     * for which to keep the vibrator on before turning it off.  Subsequent values alternate
     * between durations in milliseconds to turn the vibrator off or to turn the vibrator on.
     * </p><p>
     * To cause the pattern to repeat, pass the index into the pattern array at which
     * to start the repeat, or -1 to disable repeating.
     * </p>
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#VIBRATE}.
     *
     * @param pattern an array of longs of times for which to turn the vibrator on or off.
     * @param repeat the index into pattern at which to repeat, or -1 if
     *        you don't want to repeat.
     */
    CARAPI Vibrate(
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat);

    /**
     * Vibrate with a given pattern.
     *
     * <p>
     * Pass in an array of ints that are the durations for which to turn on or off
     * the vibrator in milliseconds.  The first value indicates the number of milliseconds
     * to wait before turning the vibrator on.  The next value indicates the number of milliseconds
     * for which to keep the vibrator on before turning it off.  Subsequent values alternate
     * between durations in milliseconds to turn the vibrator off or to turn the vibrator on.
     * </p><p>
     * To cause the pattern to repeat, pass the index into the pattern array at which
     * to start the repeat, or -1 to disable repeating.
     * </p>
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#VIBRATE}.
     *
     * @param pattern an array of longs of times for which to turn the vibrator on or off.
     * @param repeat the index into pattern at which to repeat, or -1 if
     *        you don't want to repeat.
     * @param attributes {@link AudioAttributes} corresponding to the vibration. For example,
     *        specify {@link AudioAttributes#USAGE_ALARM} for alarm vibrations or
     *        {@link AudioAttributes#USAGE_NOTIFICATION_RINGTONE} for
     *        vibrations associated with incoming calls.
     */
    CARAPI Vibrate(
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes);

    /**
     * @hide
     * Like {@link #vibrate(long, AudioAttributes)}, but allowing the caller to specify that
     * the vibration is owned by someone else.
     */
    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ Int64 milliseconds,
        /* [in] */ IAudioAttributes* attributes);

    /**
     * @hide
     * Like {@link #vibrate(long[], int, AudioAttributes)}, but allowing the caller to specify that
     * the vibration is owned by someone else.
     */
    CARAPI Vibrate(
        /* [in] */ Int32 uid,
        /* [in] */ const String& opPkg,
        /* [in] */ ArrayOf<Int64>* pattern,
        /* [in] */ Int32 repeat,
        /* [in] */ IAudioAttributes* attributes);

    /**
     * Turn the vibrator off.
     * <p>This method requires the caller to hold the permission
     * {@link android.Manifest.permission#VIBRATE}.
     */
    CARAPI Cancel();


protected:
    String mPackageName;
};

} // namespace Os
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_OS_VIBRATOR_H__
