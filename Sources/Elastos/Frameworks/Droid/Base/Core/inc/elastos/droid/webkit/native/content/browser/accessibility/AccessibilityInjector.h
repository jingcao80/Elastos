
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_ACCESSIBILITY_ACCESSIBILITYINJECTOR_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_ACCESSIBILITY_ACCESSIBILITYINJECTOR_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/webkit/native/content/browser/WebContentsObserverElastos.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IVibrator;
using Elastos::Droid::Speech::Tts::ITextToSpeech;
// import android.util.Log;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::IAccessibilityNodeInfo;
using Elastos::Droid::Webkit::Content::Browser::WebContentsObserverElastos;

// import com.googlecode.eyesfree.braille.selfbraille.SelfBrailleClient;
// import com.googlecode.eyesfree.braille.selfbraille.WriteData;

// import org.apache.http.NameValuePair;
// import org.apache.http.client.utils.URLEncodedUtils;
// import org.chromium.base.CommandLine;
// import org.chromium.content.browser.ContentViewCore;
// import org.chromium.content.browser.JavascriptInterface;
// import org.chromium.content.browser.WebContentsObserverAndroid;
// import org.chromium.content.common.ContentSwitches;
// import org.json.JSONException;
// import org.json.JSONObject;

// import java.net.URI;
// import java.net.URISyntaxException;
// import java.util.HashMap;
// import java.util.Iterator;
// import java.util.List;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

namespace Accessibility {

/**
 * Responsible for accessibility injection and management of a {@link ContentViewCore}.
 */
class AccessibilityInjector
    : public WebContentsObserverElastos
{
private:
    /**
     * Used to protect how long JavaScript can vibrate for.  This isn't a good comprehensive
     * protection, just used to cover mistakes and protect against long vibrate durations/repeats.
     *
     * Also only exposes methods we *want* to expose, no others for the class.
     */
    class VibratorWrapper
        : public Object
    {
    public:
        VibratorWrapper(
            /* [in] */ IContext* context);

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(Boolean) HasVibrator();

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(void) Vibrate(
            /* [in] */ Int64 milliseconds);

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(void) Vibrate(
            /* [in] */ ArrayOf<Int64>* pattern,
            /* [in] */ Int32 repeat);

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(void) Cancel();

    private:
        static const Int64 MAX_VIBRATE_DURATION_MS = 5000;

        AutoPtr<IVibrator> mVibrator;
    };

    /**
     * Used to protect the TextToSpeech class, only exposing the methods we want to expose.
     */
    class TextToSpeechWrapper
        : public Object
    {
        friend class AccessibilityInjector;
    public:
        TextToSpeechWrapper(
            /* [in] */ IView* view,
            /* [in] */ IContext* context);

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(Boolean) IsSpeaking();

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(Int32) Speak(
            /* [in] */ const String& text,
            /* [in] */ Int32 queueMode,
            /* [in] */ const String& jsonParams);

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(Int32) Stop();

        //@JavascriptInterface
        //@SuppressWarnings("unused")
        CARAPI_(void) Braille(
            /* [in] */ const String& jsonString);

    protected:
        //@SuppressWarnings("unused")
        CARAPI_(void) ShutdownInternal();

    private:
        AutoPtr<ITextToSpeech> mTextToSpeech;
//        AutoPtr<SelfBrailleClient> mSelfBrailleClient;
        AutoPtr<IView> mView;
    };

public:
    /**
     * Returns an instance of the {@link AccessibilityInjector} based on the SDK version.
     * @param view The ContentViewCore that this AccessibilityInjector manages.
     * @return An instance of a {@link AccessibilityInjector}.
     */
    static CARAPI_(AutoPtr<AccessibilityInjector>) NewInstance(
        /* [in] */ ContentViewCore* view);

    /**
     * Injects a <script> tag into the current web site that pulls in the ChromeVox script for
     * accessibility support.  Only injects if accessibility is turned on by
     * {@link AccessibilityManager#isEnabled()}, accessibility script injection is turned on, and
     * javascript is enabled on this page.
     *
     * @see AccessibilityManager#isEnabled()
     */
    CARAPI_(void) InjectAccessibilityScriptIntoPage();

    /**
     * Handles adding or removing accessibility related Java objects ({@link TextToSpeech} and
     * {@link Vibrator}) interfaces from Javascript.  This method should be called at a time when it
     * is safe to add or remove these interfaces, specifically when the {@link ContentViewCore} is
     * first initialized or right before the {@link ContentViewCore} is about to navigate to a URL
     * or reload.
     * <p>
     * If this method is called at other times, the interfaces might not be correctly removed,
     * meaning that Javascript can still access these Java objects that may have been already
     * shut down.
     */
    CARAPI_(void) AddOrRemoveAccessibilityApisIfNecessary();

    /**
     * Checks whether or not touch to explore is enabled on the system.
     */
    CARAPI_(Boolean) AccessibilityIsAvailable();

    /**
     * Sets whether or not the script is enabled.  If the script is disabled, we also stop any
     * we output that is occurring. If the script has not yet been injected, injects it.
     * @param enabled Whether or not to enable the script.
     */
    CARAPI_(void) SetScriptEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Notifies this handler that a page load has started, which means we should mark the
     * accessibility script as not being injected.  This way we can properly ignore incoming
     * accessibility gesture events.
     */
    //@Override
    CARAPI DidStartLoading(
        /* [in] */ const String& url);

    //@Override
    CARAPI DidStopLoading(
        /* [in] */ const String& url);

    /**
     * Stop any notifications that are currently going on (e.g. Text-to-Speech).
     */
    CARAPI_(void) OnPageLostFocus();

    /**
     * Initializes an {@link AccessibilityNodeInfo} with the actions and movement granularity
     * levels supported by this {@link AccessibilityInjector}.
     * <p>
     * If an action identifier is added in this method, this {@link AccessibilityInjector} should
     * also return {@code true} from {@link #supportsAccessibilityAction(int)}.
     * </p>
     *
     * @param info The info to initialize.
     * @see View#onInitializeAccessibilityNodeInfo(AccessibilityNodeInfo)
     */
    CARAPI_(void) OnInitializeAccessibilityNodeInfo(
        /* [in] */ IAccessibilityNodeInfo* info);

    /**
     * Returns {@code true} if this {@link AccessibilityInjector} should handle the specified
     * action.
     *
     * @param action An accessibility action identifier.
     * @return {@code true} if this {@link AccessibilityInjector} should handle the specified
     *         action.
     */
    CARAPI_(Boolean) SupportsAccessibilityAction(
        /* [in] */ Int32 action);

    /**
     * Performs the specified accessibility action.
     *
     * @param action The identifier of the action to perform.
     * @param arguments The action arguments, or {@code null} if no arguments.
     * @return {@code true} if the action was successful.
     * @see View#performAccessibilityAction(int, Bundle)
     */
    CARAPI_(Boolean) PerformAccessibilityAction(
        /* [in] */ Int32 action,
        /* [in] */ IBundle* arguments);

protected:
    /**
     * Creates an instance of the IceCreamSandwichAccessibilityInjector.
     * @param view The ContentViewCore that this AccessibilityInjector manages.
     */
    AccessibilityInjector(
        /* [in] */ ContentViewCore* view);

    CARAPI_(void) AddAccessibilityApis();

    CARAPI_(void) RemoveAccessibilityApis();

protected:
    // The ContentView this injector is responsible for managing.
    AutoPtr<ContentViewCore> mContentViewCore;

    // Whether or not we should be injecting the script.
    Boolean mInjectedScriptEnabled;
    Boolean mScriptInjected;

private:
    CARAPI_(Int32) GetAxsUrlParameterValue();

    CARAPI_(String) GetScreenReaderInjectingJs();

    CARAPI_(AutoPtr<IAccessibilityManager>) GetAccessibilityManager();

private:
    static const String TAG;

    // The Java objects that are exposed to JavaScript
    AutoPtr<TextToSpeechWrapper> mTextToSpeech;
    AutoPtr<VibratorWrapper> mVibrator;
    /*const*/ Boolean mHasVibratePermission;

    // Lazily loaded helper objects.
    AutoPtr<IAccessibilityManager> mAccessibilityManager;

    const String mAccessibilityScreenReaderUrl;

    // To support building against the JELLY_BEAN and not JELLY_BEAN_MR1 SDK we need to add this
    // constant here.
    static const Int32 FEEDBACK_BRAILLE = 0x00000020;

    // constants for determining script injection strategy
    static const Int32 ACCESSIBILITY_SCRIPT_INJECTION_UNDEFINED = -1;
    static const Int32 ACCESSIBILITY_SCRIPT_INJECTION_OPTED_OUT = 0;
    static const Int32 ACCESSIBILITY_SCRIPT_INJECTION_PROVIDED = 1;
    static const String ALIAS_ACCESSIBILITY_JS_INTERFACE;
    static const String ALIAS_ACCESSIBILITY_JS_INTERFACE_2;

    // Template for JavaScript that injects a screen-reader.
    static const String DEFAULT_ACCESSIBILITY_SCREEN_READER_URL;

    static const String ACCESSIBILITY_SCREEN_READER_JAVASCRIPT_TEMPLATE;

    // JavaScript call to turn ChromeVox on or off.
    static const String TOGGLE_CHROME_VOX_JAVASCRIPT;
};

} // namespace Accessibility
} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_ACCESSIBILITY_ACCESSIBILITYINJECTOR_H__
