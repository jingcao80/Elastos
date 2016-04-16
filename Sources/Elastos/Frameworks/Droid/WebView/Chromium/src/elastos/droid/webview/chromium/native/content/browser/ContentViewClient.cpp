#include "Elastos.Droid.Content.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewClient.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/SelectActionModeCallback.h"

using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::CIntentHelper;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

const String ContentViewClient::TAG("ContentViewClient");

void ContentViewClient::OnUpdateTitle(
    /* [in] */ const String& title)
{
}

/**
 * Called whenever the background color of the page changes as notified by WebKit.
 * @param color The new ARGB color of the page background.
 */
void ContentViewClient::OnBackgroundColorChanged(
    /* [in] */ Int32 color)
{
}

/**
 * Notifies the client that the position of the top controls has changed.
 * @param topControlsOffsetYPix The Y offset of the top controls in physical pixels.
 * @param contentOffsetYPix The Y offset of the content in physical pixels.
 * @param overdrawBottomHeightPix The overdraw height.
 */
void ContentViewClient::OnOffsetsForFullscreenChanged(
    /* [in] */ Float topControlsOffsetYPix,
    /* [in] */ Float contentOffsetYPix,
    /* [in] */ Float overdrawBottomHeightPix)
{
}

Boolean ContentViewClient::ShouldOverrideKeyEvent(
    /* [in] */ IKeyEvent* event)
{
    Int32 keyCode;
    event->GetKeyCode(&keyCode);

    if (!ShouldPropagateKey(keyCode)) {
        return TRUE;
    }

    // We also have to intercept some shortcuts before we send them to the ContentView.
    Boolean bFlag = FALSE;
    if ((event->IsCtrlPressed(&bFlag), bFlag) && (
            keyCode == IKeyEvent::KEYCODE_TAB ||
            keyCode == IKeyEvent::KEYCODE_W ||
            keyCode == IKeyEvent::KEYCODE_F4)) {
        return TRUE;
    }

    return FALSE;
}

/**
 * Called when an ImeEvent is sent to the page. Can be used to know when some text is entered
 * in a page.
 */
void ContentViewClient::OnImeEvent()
{
}

/**
 * Notified when a change to the IME was requested.
 *
 * @param requestShow Whether the IME was requested to be shown (may already be showing
 *                    though).
 */
void ContentViewClient::OnImeStateChangeRequested(
    /* [in] */ Boolean requestShow)
{
}

/**
 * Returns an ActionMode.Callback for in-page selection.
 */
AutoPtr<IActionModeCallback> ContentViewClient::GetSelectActionModeCallback(
    /* [in] */ IContext* context,
    /* [in] */ SelectActionModeCallback::ActionHandler* actionHandler,
    /* [in] */ Boolean incognito)
{
    return new SelectActionModeCallback(context, actionHandler, incognito);
}

/**
 * Called when the contextual ActionBar is shown.
 */
void ContentViewClient::OnContextualActionBarShown()
{
}

/**
 * Called when the contextual ActionBar is hidden.
 */
void ContentViewClient::OnContextualActionBarHidden()
{
}

/**
 * Perform a search on {@code searchQuery}.  This method is only called if
 * {@link #doesPerformWebSearch()} returns {@code true}.
 * @param searchQuery The string to search for.
 */
void ContentViewClient::PerformWebSearch(
    /* [in] */ const String& searchQuery)
{
}

/**
 * If this returns {@code true} contextual web search attempts will be forwarded to
 * {@link #performWebSearch(String)}.
 * @return {@code true} iff this {@link ContentViewClient} wants to consume web search queries
 *         and override the default intent behavior.
 */
Boolean ContentViewClient::DoesPerformWebSearch()
{
    return FALSE;
}

/**
 * Notification that the selection has changed.
 * @param selection The newly established selection.
 */
void ContentViewClient::OnSelectionChanged(
    /* [in] */ const String& selection)
{
}

/**
 * Called when a new content intent is requested to be started.
 */
void ContentViewClient::OnStartContentIntent(
    /* [in] */ IContext* context,
    /* [in] */ const String& intentUrl)
{
    AutoPtr<IIntent> intent;
    // Perform generic parsing of the URI to turn it into an Intent.
    // try {
        AutoPtr<IIntentHelper> heper;
        CIntentHelper::AcquireSingleton((IIntentHelper**)&heper);
        heper->ParseUri(intentUrl, IIntent::URI_INTENT_SCHEME, (IIntent**)&intent);
    // } catch (URISyntaxException ex) {
    //     Log.w(TAG, "Bad URI " + intentUrl + ": " + ex.getMessage());
    //     return;
    // }

    // try {
        if( intent != NULL)
            context->StartActivity(intent);
    // } catch (ActivityNotFoundException ex) {
    //     Log.w(TAG, "No application can handle " + intentUrl);
    // }
}

AutoPtr<ContentVideoViewClient> ContentViewClient::GetContentVideoViewClient()
{
    return NULL;
}

/**
 * Called when BrowserMediaPlayerManager wants to load a media resource.
 * @param url the URL of media resource to load.
 * @return true to prevent the resource from being loaded.
 */
Boolean ContentViewClient::ShouldBlockMediaRequest(
    /* [in] */ const String& url)
{
    return FALSE;
}

/**
 * Check whether a key should be propagated to the embedder or not.
 * We need to send almost every key to Blink. However:
 * 1. We don't want to block the device on the renderer for
 * some keys like menu, home, call.
 * 2. There are no WebKit equivalents for some of these keys
 * (see app/keyboard_codes_win.h)
 * Note that these are not the same set as KeyEvent.isSystemKey:
 * for instance, AKEYCODE_MEDIA_* will be dispatched to webkit*.
 */
Boolean ContentViewClient::ShouldPropagateKey(
    /* [in] */ Int32 keyCode)
{
    if (keyCode == IKeyEvent::KEYCODE_MENU ||
        keyCode == IKeyEvent::KEYCODE_HOME ||
        keyCode == IKeyEvent::KEYCODE_BACK ||
        keyCode == IKeyEvent::KEYCODE_CALL ||
        keyCode == IKeyEvent::KEYCODE_ENDCALL ||
        keyCode == IKeyEvent::KEYCODE_POWER ||
        keyCode == IKeyEvent::KEYCODE_HEADSETHOOK ||
        keyCode == IKeyEvent::KEYCODE_CAMERA ||
        keyCode == IKeyEvent::KEYCODE_FOCUS ||
        keyCode == IKeyEvent::KEYCODE_VOLUME_DOWN ||
        keyCode == IKeyEvent::KEYCODE_VOLUME_MUTE ||
        keyCode == IKeyEvent::KEYCODE_VOLUME_UP) {
        return FALSE;
    }

    return TRUE;
}

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
