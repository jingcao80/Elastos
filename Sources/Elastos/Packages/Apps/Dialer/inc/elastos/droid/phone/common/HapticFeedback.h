
#ifndef __ELASTOS_DROID_PHONE_COMMON_HAPTICFEEDBACK_H__
#define __ELASTOS_DROID_PHONE_COMMON_HAPTICFEEDBACK_H__

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Phone {
namespace Common {

/**
 * Handles the haptic feedback: a light buzz happening when the user
 * presses a soft key (UI button or capacitive key).  The haptic
 * feedback is controlled by:
 * - a system resource for the pattern
 *   The pattern used is tuned per device and stored in an internal
 *   resource (config_virtualKeyVibePattern.)
 * - a system setting HAPTIC_FEEDBACK_ENABLED.
 *   HAPTIC_FEEDBACK_ENABLED can be changed by the user using the
 *   system Settings activity. It must be rechecked each time the
 *   activity comes in the foreground (onResume).
 *
 * This class is not thread safe. It assumes it'll be called from the
 * UI thead.
 *
 * Typical usage:
 * --------------
 *   static private final boolean HAPTIC_ENABLED = true;
 *   private HapticFeedback mHaptic = new HapticFeedback();
 *
 *   protected void onCreate(Bundle icicle) {
 *     mHaptic.init((Context)this, HAPTIC_ENABLED);
 *   }
 *
 *   protected void onResume() {
 *     // Refresh the system setting.
 *     mHaptic.checkSystemSetting();
 *   }
 *
 *   public void foo() {
 *     mHaptic.vibrate();
 *   }
 *
 */
class HapticFeedback
    : public Object
{
public:
    /**
     * Initialize this instance using the app and system
     * configs. Since these don't change, init is typically called
     * once in 'onCreate'.
     * checkSettings is not called during init.
     * @param context To look up the resources and system settings.
     * @param enabled If false, vibrate will be a no-op regardless of
     * the system settings.
     */
    CARAPI Init(
        /* [in] */ IContext* context,
        /* [in] */ Boolean enabled);

    /**
     * Reload the system settings to check if the user enabled the
     * haptic feedback.
     */
    CARAPI CheckSystemSetting();

    /**
     * Generate the haptic feedback vibration. Only one thread can
     * request it. If the phone is already in a middle of an haptic
     * feedback sequence, the request is ignored.
     */
    CARAPI Vibrate();

private:
    /** If no pattern was found, vibrate for a small amount of time. */
    static const Int64 DURATION;  // millisec.
    /** Play the haptic pattern only once. */
    static const Int32 NO_REPEAT;

    static const String TAG;
    AutoPtr<IContext> mContext;
    AutoPtr< ArrayOf<Int64> > mHapticPattern;
    AutoPtr<IVibrator> mVibrator;

    Boolean mEnabled;
    AutoPtr<ISettingsSystem> mSystemSettings;
    AutoPtr<IContentResolver> mContentResolver;
    Boolean mSettingEnabled;
};

} // namespace Common
} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PHONE_COMMON_HAPTICFEEDBACK_H__
