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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ChildProcessConnection.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {

/**
 * Manages oom bindings used to bound child services. "Oom binding" is a binding that raises the
 * process oom priority so that it shouldn't be killed by the OS out-of-memory killer under
 * normal conditions (it can still be killed under drastic memory pressure). ChildProcessConnections
 * have two oom bindings: initial binding and strong binding.
 *
 * BindingManager receives calls that signal visibility of each service (setInForeground()) and the
 * entire embedding application (onSentToBackground(), onBroughtToForeground()) and manipulates
 * child process bindings accordingly.
 *
 * In particular, BindingManager is responsible for:
 * - removing the initial binding of a service when its visibility is determined for the first time
 * - addition and (possibly delayed) removal of a strong binding when service visibility changes
 * - dropping the current oom bindings when a new connection is started on a low-memory device
 * - keeping a strong binding on the foreground service while the entire application is in
 *   background
 *
 * Thread-safety: most of the methods will be called only on the main thread, exceptions are
 * explicitly noted.
 */
class BindingManager : public Object
{
public:
    /**
     * Registers a freshly started child process. On low-memory devices this will also drop the
     * oom bindings of the last process that was oom-bound. We can do that, because every time a
     * connection is created on the low-end, it is used in foreground (no prerendering, no
     * loading of tabs opened in background). This can be called on any thread.
     * @param pid handle of the service process
     */
    virtual CARAPI_(void) AddNewConnection(
        /* [in] */ Int32 pid,
        /* [in] */ ChildProcessConnection* connection) = 0;

    /**
     * Called when the service visibility changes or is determined for the first time.
     * @param pid handle of the service process
     * @param inForeground true iff the service is visibile to the user
     */
    virtual CARAPI_(void) SetInForeground(
        /* [in] */ Int32 pid,
        /* [in] */ Boolean inForeground) = 0;

    /**
     * Called when the embedding application is sent to background. We want to maintain a strong
     * binding on the most recently used renderer while the embedder is in background, to indicate
     * the relative importance of the renderer to system oom killer.
     *
     * The embedder needs to ensure that:
     *  - every onBroughtToForeground() is followed by onSentToBackground()
     *  - pairs of consecutive onBroughtToForeground() / onSentToBackground() calls do not overlap
     */
    virtual CARAPI_(void) OnSentToBackground() = 0;

    /**
     * Called when the embedding application is brought to foreground. This will drop the strong
     * binding kept on the main renderer during the background period, so the embedder should make
     * sure that this is called after the regular strong binding is attached for the foreground
     * session.
     */
    virtual CARAPI_(void) OnBroughtToForeground() = 0;

    /**
     * @return True iff the given service process is protected from the out-of-memory killing, or it
     * was protected when it died unexpectedly. This can be used to decide if a disconnection of a
     * renderer was a crash or a probable out-of-memory kill. This can be called on any thread.
     */
    virtual CARAPI_(Boolean) IsOomProtected(
        /* [in] */ Int32 pid) = 0;

    /**
     * Should be called when the connection to the child process goes away (either after a clean
     * exit or an unexpected crash). At this point we let go of the reference to the
     * ChildProcessConnection. This can be called on any thread.
     */
    virtual CARAPI_(void) ClearConnection(
        /* [in] */ Int32 pid) = 0;
};

} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_BINDINGMANAGER_H__
