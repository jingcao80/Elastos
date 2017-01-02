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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANACCESSIBIlITYINJECTOR_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANACCESSIBIlITYINJECTOR_H__

#include "elastos/droid/webkit/webview/chromium/native/content/browser/accessibility/AccessibilityInjector.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Utility::Concurrent::Atomic::IAtomicInteger32;
using Org::Json::IJSONObject;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Content {
namespace Browser {
namespace Accessibility {

/**
 * Handles injecting accessibility Javascript and related Javascript -> Java APIs for JB and newer
 * devices.
 */
class JellyBeanAccessibilityInjector : public AccessibilityInjector
{
    friend class AccessibilityInjector;
private:
    class CallbackHandler
        : public Object
    {
        friend class JellyBeanAccessibilityInjector;
    public:
        /**
         * Callback exposed to JavaScript.  Handles returning the result of a
         * request to a waiting (or potentially timed out) thread.
         *
         * @param id The result id of the request as a {@link String}.
         * @param result The result of a request as a {@link String}.
         */
        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(void) OnResult(
            /* [in] */ const String& id,
            /* [in] */ const String& result);

    private:
        CallbackHandler(
            /* [in] */ const String& interfaceName);

        /**
         * Performs an action and attempts to wait for a result.
         *
         * @param contentView The ContentViewCore to perform the action on.
         * @param code Javascript code that evaluates to a result.
         * @return The result of the action.
         */
        CARAPI_(Boolean) PerformAction(
            /* [in] */ ContentViewCore* contentView,
            /* [in] */ const String& code);

        /**
         * Gets the result of a request to perform an accessibility action.
         *
         * @param resultId The result id to match the result with the request.
         * @return The result of the request.
         */
        CARAPI_(Boolean) GetResultAndClear(
            /* [in] */ Int32 resultId);

        /**
         * Clears the result state.
         */
        CARAPI_(void) ClearResultLocked();

        /**
         * Waits up to a given bound for a result of a request and returns it.
         *
         * @param resultId The result id to match the result with the request.
         * @return Whether the result was received.
         */
        CARAPI_(Boolean) WaitForResultTimedLocked(
            /* [in] */ Int32 resultId);

    private:
        static const String JAVASCRIPT_ACTION_TEMPLATE;

        // Time in milliseconds to wait for a result before failing.
        static const Int64 RESULT_TIMEOUT = 5000;

        /*const*/ AutoPtr<IAtomicInteger32> mResultIdCounter;
        /*const*/ Object mResultLock;
        const String mInterfaceName;

        Boolean mResult;
        Int64 mResultId;
    };

public:
    //@Override
    CARAPI_(void) OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    //@Override
    CARAPI_(Boolean) SupportsAccessibilityAction(
        /* [in] */ Int32 action);

    //@Override
    CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

protected:
    /**
     * Constructs an instance of the JellyBeanAccessibilityInjector.
     * @param view The ContentViewCore that this AccessibilityInjector manages.
     */
    JellyBeanAccessibilityInjector(
        /* [in] */ ContentViewCore* view);

    //@Override
    CARAPI_(void) AddAccessibilityApis();

    //@Override
    CARAPI_(void) RemoveAccessibilityApis();

private:
    /**
     * Packs an accessibility action into a JSON object and sends it to AndroidVox.
     *
     * @param action The action identifier.
     * @param arguments The action arguments, if applicable.
     * @return The result of the action.
     */
    CARAPI_(Boolean) SendActionToAndroidVox(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

private:
    AutoPtr<CallbackHandler> mCallback;
    AutoPtr<IJSONObject> mAccessibilityJSONObject;

    static const String ALIAS_TRAVERSAL_JS_INTERFACE;

    // Template for JavaScript that performs AndroidVox actions.
    static const String ACCESSIBILITY_ANDROIDVOX_TEMPLATE;
};

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_CONTENT_BROWSER_ACCESSIBILITY_JELLYBEANACCESSIBIlITYINJECTOR_H__
