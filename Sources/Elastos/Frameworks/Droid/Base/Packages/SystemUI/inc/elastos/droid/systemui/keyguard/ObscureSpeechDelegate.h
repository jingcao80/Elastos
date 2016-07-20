
#ifndef __ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__
#define __ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__

#include "_Elastos.Droid.SystemUI.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

/**
 * Accessibility delegate that obscures speech for a view when the user has
 * not turned on the "speak passwords" preference and is not listening
 * through headphones.
 */
class ObscureSpeechDelegate
    : public AccessibilityDelegate
{
public:
    TO_STRING_IMPL("ObscureSpeechDelegate")

    ObscureSpeechDelegate(
        /* [in] */ IContext* context);

    //@Override
    CARAPI SendAccessibilityEvent(
        /* [in] */ IView* host,
        /* [in] */ Int32 eventType);

    //@Override
    CARAPI OnPopulateAccessibilityEvent(
        /* [in] */ IView* host,
        /* [in] */ IAccessibilityEvent* event);

    //@Override
    CARAPI OnInitializeAccessibilityNodeInfo(
        /* [in] */ IView* host,
        /* [in] */ IAccessibilityNodeInfo* info);

private:
    //@SuppressWarnings("deprecation")
    CARAPI_(Boolean) ShouldObscureSpeech();

private:
    /** Whether any client has announced the "headset" notification. */
    static Boolean sAnnouncedHeadset;

    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IAudioManager> mAudioManager;
};

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_KEYGUARD_OBSCURESPEECHDELEGATE_H__