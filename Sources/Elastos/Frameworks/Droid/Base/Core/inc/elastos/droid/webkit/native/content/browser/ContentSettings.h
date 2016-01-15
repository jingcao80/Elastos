
#ifndef __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTSETTINGS_H__
#define __ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTSETTINGS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;
// import org.chromium.base.ThreadUtils;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

class ContentViewCore;

/**
 * Manages settings state for a ContentView. A ContentSettings instance is obtained
 * from ContentViewCore.getContentSettings().
 */
//@JNINamespace("content")
class ContentSettings
    : public Object
{

public:
    /**
     * Package constructor to prevent clients from creating a new settings
     * instance. Must be called on the UI thread.
     */
    ContentSettings(
        /* [in] */ ContentViewCore* contentViewCore,
        /* [in] */ Int64 nativeContentView);

    /**
     * Return true if JavaScript is enabled. Must be called on the UI thread.
     *
     * @return True if JavaScript is enabled.
     */
    CARAPI_(Boolean) GetJavaScriptEnabled();

    static CARAPI_(void*) ElaContentSettingsCallback_Init();

private:
    static CARAPI_(void) OnNativeContentSettingsDestroyed(
        /* [in] */ IInterface* obj,
        /* [in] */ Int64 nativeContentSettings);

    /**
     * Notification from the native side that it is being destroyed.
     * @param nativeContentSettings the native instance that is going away.
     */
    //@CalledByNative
    CARAPI_(void) OnNativeContentSettingsDestroyed(
        /* [in] */ Int64 nativeContentSettings);

    // Initialize the ContentSettings native side.
    CARAPI_(Int64) NativeInit(
        /* [in] */ Int64 contentViewPtr);

    CARAPI_(Boolean) NativeGetJavaScriptEnabled(
        /* [in] */ Int64 nativeContentSettings);

private:
    static const String TAG;

    // The native side of this object. Ownership is retained native-side by the WebContents
    // instance that backs the associated ContentViewCore.
    Int64 mNativeContentSettings;

    AutoPtr<ContentViewCore> mContentViewCore;
};

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_CONTENT_BROWSER_CONTENTSETTINGS_H__
