
#include "elastos/droid/systemui/keyguard/ObscureSpeechDelegate.h"
#include "Elastos.Droid.View.h"

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Keyguard {

Boolean ObscureSpeechDelegate::sAnnouncedHeadset = FALSE;

ObscureSpeechDelegate::ObscureSpeechDelegate(
    /* [in] */ IContext* context)
{
    context->GetContentResolver((IContentResolver**)&mContentResolver);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
    mAudioManager = IAudioManager::Probe(obj);
}

ECode ObscureSpeechDelegate::SendAccessibilityEvent(
    /* [in] */ IView* host,
    /* [in] */ Int32 eventType)
{
    AccessibilityDelegate::SendAccessibilityEvent(host, eventType);

    // Play the "headset required" announcement the first time the user
    // places accessibility focus on a key.
    Boolean res;
    if ((eventType == IAccessibilityEvent::TYPE_VIEW_ACCESSIBILITY_FOCUSED)
            && !sAnnouncedHeadset && (ShouldObscureSpeech(&res), res)) {
        sAnnouncedHeadset = TRUE;

        AutoPtr<IContext> context;
        host->GetContext((IContext**)&context);
        String passwords;
        context->GetString(R::string::keyboard_headset_required_to_hear_password, &passwords);
        host->AnnounceForAccessibility(passwords);
    }
    return NOERROR;
}

ECode ObscureSpeechDelegate::OnPopulateAccessibilityEvent(
    /* [in] */ IView* host,
    /* [in] */ IAccessibilityEvent* event)
{
    AccessibilityDelegate::OnPopulateAccessibilityEvent(host, event);

    Int32 type;
    event->GetEventType(&type);
    Boolean res;
    if ((type != IAccessibilityEvent::TYPE_ANNOUNCEMENT)
            && (ShouldObscureSpeech(&res), res)) {
        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        list->Clear();

        AutoPtr<IContext> context;
        host->GetContext((IContext**)&context);
        String headset;
        context->GetString(R::string::keyboard_password_character_no_headset, &headset);
        event->SetContentDescription(headset);
    }
}

ECode ObscureSpeechDelegate::OnInitializeAccessibilityNodeInfo(
    /* [in] */ IView* host,
    /* [in] */ IAccessibilityNodeInfo* info)
{
    AccessibilityDelegate::OnInitializeAccessibilityNodeInfo(host, info);

    Boolean res;
    if (ShouldObscureSpeech(&res), res) {
        AutoPtr<IContext> ctx;
        host->GetContext((IContext**)&ctx);
        info->SetText(NULL);
        String headset;
        ctx->GetString(R::string::keyboard_password_character_no_headset, &headset);
        info->SetContentDescription(headset);
    }
    return NOERROR;
}

Boolean ObscureSpeechDelegate::ShouldObscureSpeech()
{
    // The user can optionally force speaking passwords.
    AutoPtr<ISettingsSecure> helper;
    CSettingsSecure::AcquireSingleton((ISettingsSecure**)&helper);
    Int32 password;
    helper->GetInt32(mContentResolver, ISettingsSecure::ACCESSIBILITY_SPEAK_PASSWORD, 0, &password);
    if (password != 0) {
        return FALSE;
    }

    // Always speak if the user is listening through headphones.
    Boolean res1, res2;
    if ((mAudioManager->IsWiredHeadsetOn(&res1), res1) ||
        (mAudioManager->IsBluetoothA2dpOn(&res2), res2)) {
        return FALSE;
    }

    // Don't speak since this key is used to type a password.
    return TRUE;
}

} // namespace Keyguard
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
