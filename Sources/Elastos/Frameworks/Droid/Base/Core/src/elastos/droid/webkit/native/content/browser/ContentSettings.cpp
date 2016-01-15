
#include "elastos/droid/webkit/native/content/browser/ContentSettings.h"
#include "elastos/droid/webkit/native/content/api/ContentSettings_dec.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/base/ThreadUtils.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Webkit::Base::ThreadUtils;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Content {
namespace Browser {

const String ContentSettings::TAG("ContentSettings");

/**
 * Package constructor to prevent clients from creating a new settings
 * instance. Must be called on the UI thread.
 */
ContentSettings::ContentSettings(
    /* [in] */ ContentViewCore* contentViewCore,
    /* [in] */ Int64 nativeContentView)
{
    ThreadUtils::AssertOnUiThread();
    mContentViewCore = contentViewCore;
    mNativeContentSettings = NativeInit(nativeContentView);
    assert(mNativeContentSettings != 0);
}

/**
 * Notification from the native side that it is being destroyed.
 * @param nativeContentSettings the native instance that is going away.
 */
//@CalledByNative
void ContentSettings::OnNativeContentSettingsDestroyed(
    /* [in] */ Int64 nativeContentSettings)
{
    assert(mNativeContentSettings == nativeContentSettings);
    mNativeContentSettings = 0;
}

/**
 * Return true if JavaScript is enabled. Must be called on the UI thread.
 *
 * @return True if JavaScript is enabled.
 */
Boolean ContentSettings::GetJavaScriptEnabled()
{
    ThreadUtils::AssertOnUiThread();
    return mNativeContentSettings != 0 ?
            NativeGetJavaScriptEnabled(mNativeContentSettings) : FALSE;
}

// Initialize the ContentSettings native side.
Int64 ContentSettings::NativeInit(
    /* [in] */ Int64 contentViewPtr)
{
    return Elastos_ContentSettings_nativeInit(THIS_PROBE(IInterface), contentViewPtr);
}

Boolean ContentSettings::NativeGetJavaScriptEnabled(
    /* [in] */ Int64 nativeContentSettings)
{
    return Elastos_ContentSettings_nativeGetJavaScriptEnabled(THIS_PROBE(IInterface), (Handle32)nativeContentSettings);
}

void ContentSettings::OnNativeContentSettingsDestroyed(
    /* [in] */ IInterface* obj,
    /* [in] */ Int64 nativeContentSettings)
{
    AutoPtr<ContentSettings> mObj = (ContentSettings*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E(TAG, "ContentSettings::OnNativeContentSettingsDestroyed, mObj is NULL");
        return;
    }
    mObj->OnNativeContentSettingsDestroyed(nativeContentSettings);
}

} // namespace Browser
} // namespace Content
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
