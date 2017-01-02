//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWCLIENT_H__

#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentVideoViewClient.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/SelectActionModeCallback.h"
#include <elastos/core/Object.h>

//using Elastos::Droid::Content::IActivityNotFoundException;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
// import android.util.Log;
using Elastos::Droid::View::IActionMode;
using Elastos::Droid::View::IKeyEvent;

// import org.chromium.content.browser.SelectActionModeCallback.ActionHandler;

// import java.net.URISyntaxException;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 *  Main callback class used by ContentView.
 *
 *  This contains the superset of callbacks required to implement the browser UI and the callbacks
 *  required to implement the WebView API.
 *  The memory and reference ownership of this class is unusual - see the .cc file and ContentView
 *  for more details.
 *
 *  TODO(mkosiba): Rid this guy of default implementations. This class is used by both WebView and
 *  the browser and we don't want a the browser-specific default implementation to accidentally leak
 *  over to WebView.
 */
class ContentViewClient : public Object
{
public:
    CARAPI_(void) OnUpdateTitle(
        /* [in] */ const String& title);

    /**
     * Called whenever the background color of the page changes as notified by WebKit.
     * @param color The new ARGB color of the page background.
     */
    CARAPI_(void) OnBackgroundColorChanged(
        /* [in] */ Int32 color);

    /**
     * Notifies the client that the position of the top controls has changed.
     * @param topControlsOffsetYPix The Y offset of the top controls in physical pixels.
     * @param contentOffsetYPix The Y offset of the content in physical pixels.
     * @param overdrawBottomHeightPix The overdraw height.
     */
    CARAPI_(void) OnOffsetsForFullscreenChanged(
        /* [in] */ Float topControlsOffsetYPix,
        /* [in] */ Float contentOffsetYPix,
        /* [in] */ Float overdrawBottomHeightPix);

    CARAPI_(Boolean) ShouldOverrideKeyEvent(
        /* [in] */ IKeyEvent* event);

    /**
     * Called when an ImeEvent is sent to the page. Can be used to know when some text is entered
     * in a page.
     */
    CARAPI_(void) OnImeEvent();

    /**
     * Notified when a change to the IME was requested.
     *
     * @param requestShow Whether the IME was requested to be shown (may already be showing
     *                    though).
     */
    CARAPI_(void) OnImeStateChangeRequested(
        /* [in] */ Boolean requestShow);

    /**
     * Returns an ActionMode.Callback for in-page selection.
     */
    CARAPI_(AutoPtr<IActionModeCallback>) GetSelectActionModeCallback(
        /* [in] */ IContext* context,
        /* [in] */ SelectActionModeCallback::ActionHandler* actionHandler,
        /* [in] */ Boolean incognito);

    /**
     * Called when the contextual ActionBar is shown.
     */
    CARAPI_(void) OnContextualActionBarShown();

    /**
     * Called when the contextual ActionBar is hidden.
     */
    CARAPI_(void) OnContextualActionBarHidden();

    /**
     * Perform a search on {@code searchQuery}.  This method is only called if
     * {@link #doesPerformWebSearch()} returns {@code true}.
     * @param searchQuery The string to search for.
     */
    CARAPI_(void) PerformWebSearch(
        /* [in] */ const String& searchQuery);

    /**
     * If this returns {@code true} contextual web search attempts will be forwarded to
     * {@link #performWebSearch(String)}.
     * @return {@code true} iff this {@link ContentViewClient} wants to consume web search queries
     *         and override the default intent behavior.
     */
    CARAPI_(Boolean) DoesPerformWebSearch();

    /**
     * Notification that the selection has changed.
     * @param selection The newly established selection.
     */
    CARAPI_(void) OnSelectionChanged(
        /* [in] */ const String& selection);

    /**
     * Called when a new content intent is requested to be started.
     */
    CARAPI_(void) OnStartContentIntent(
        /* [in] */ IContext* context,
        /* [in] */ const String& intentUrl);

    CARAPI_(AutoPtr<ContentVideoViewClient>) GetContentVideoViewClient();

    /**
     * Called when BrowserMediaPlayerManager wants to load a media resource.
     * @param url the URL of media resource to load.
     * @return true to prevent the resource from being loaded.
     */
    CARAPI_(Boolean) ShouldBlockMediaRequest(
        /* [in] */ const String& url);

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
    static CARAPI_(Boolean) ShouldPropagateKey(
        /* [in] */ Int32 keyCode);

private:
    // Tag used for logging.
    static const String TAG;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_CONTENTVIEWCLIENT_H__
