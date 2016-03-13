
#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSUTILS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Build.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::Build;
using Elastos::Droid::Os::IStrictMode;
// import android.util.Log;

// import java.io.BufferedReader;
// import java.io.FileReader;
// import java.util.regex.Matcher;
// import java.util.regex.Pattern;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * Exposes system related information about the current device.
 */
class SysUtils
    : public Object
{
public:
    /**
     * Return the amount of physical memory on this device in kilobytes.
     * Note: the only reason this is public is for testability reason.
     * @return Amount of physical memory in kilobytes, or 0 if there was
     *         an error trying to access the information.
     *
     * Note that this is CalledByNative for testing purpose only.
     */
    //@CalledByNative
    static CARAPI_(Int32) AmountOfPhysicalMemoryKB();

    /**
     * @return Whether or not this device should be considered a low end device.
     */
    //@CalledByNative
    static CARAPI_(Boolean) IsLowEndDevice();

    static CARAPI_(void*) ElaSysUtilsCallback_Init();

private:
    SysUtils();

    static CARAPI_(Boolean) DetectLowEndDevice();

private:
    // Any device that runs this or an older version of the system cannot be considered 'low-end'
    static const Int32 ANDROID_LOW_MEMORY_ANDROID_SDK_THRESHOLD =
            Build::VERSION_CODES::JELLY_BEAN_MR2;

    // A device reporting strictly more total memory in megabytes cannot be considered 'low-end'.
    static const Int64 ANDROID_LOW_MEMORY_DEVICE_THRESHOLD_MB = 512;

    static const String TAG;

    static Boolean sLowEndDevice;
    static Boolean sLowEndDeviceInit;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_SYSUTILS_H__
