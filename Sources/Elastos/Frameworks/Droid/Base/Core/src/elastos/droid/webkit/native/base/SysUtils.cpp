
#include "elastos/droid/webkit/native/base/SysUtils.h"
#include "elastos/droid/webkit/native/base/api/SysUtils_dec.h"

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Base {

const Int32 SysUtils::ANDROID_LOW_MEMORY_ANDROID_SDK_THRESHOLD;
const Int64 SysUtils::ANDROID_LOW_MEMORY_DEVICE_THRESHOLD_MB;
const String SysUtils::TAG("SysUtils");
Boolean SysUtils::sLowEndDevice = FALSE;
Boolean SysUtils::sLowEndDeviceInit = FALSE;

SysUtils::SysUtils()
{
}

/**
 * Return the amount of physical memory on this device in kilobytes.
 * Note: the only reason this is public is for testability reason.
 * @return Amount of physical memory in kilobytes, or 0 if there was
 *         an error trying to access the information.
 *
 * Note that this is CalledByNative for testing purpose only.
 */
//@CalledByNative
Int32 SysUtils::AmountOfPhysicalMemoryKB()
{
    assert(0);
#if 0
    // Extract total memory RAM size by parsing /proc/meminfo, note that
    // this is exactly what the implementation of sysconf(_SC_PHYS_PAGES)
    // does. However, it can't be called because this method must be
    // usable before any native code is loaded.

    // An alternative is to use ActivityManager.getMemoryInfo(), but this
    // requires a valid ActivityManager handle, which can only come from
    // a valid Context object, which itself cannot be retrieved
    // during early startup, where this method is called. And making it
    // an explicit parameter here makes all call paths _much_ more
    // complicated.

    Pattern pattern = Pattern.compile("^MemTotal:\\s+([0-9]+) kB$");
    // Synchronously reading files in /proc in the UI thread is safe.
    StrictMode.ThreadPolicy oldPolicy = StrictMode.allowThreadDiskReads();
    try {
        FileReader fileReader = new FileReader("/proc/meminfo");
        try {
            BufferedReader reader = new BufferedReader(fileReader);
            try {
                String line;
                for (;;) {
                    line = reader.readLine();
                    if (line == null) {
                        Log.w(TAG, "/proc/meminfo lacks a MemTotal entry?");
                        break;
                    }
                    Matcher m = pattern.matcher(line);
                    if (!m.find()) continue;

                    int totalMemoryKB = Integer.parseInt(m.group(1));
                    // Sanity check.
                    if (totalMemoryKB <= 1024) {
                        Log.w(TAG, "Invalid /proc/meminfo total size in kB: " + m.group(1));
                        break;
                    }

                    return totalMemoryKB;
                }

            } finally {
                reader.close();
            }
        } finally {
            fileReader.close();
        }
    } catch (Exception e) {
        Log.w(TAG, "Cannot get total physical size from /proc/meminfo", e);
    } finally {
        StrictMode.setThreadPolicy(oldPolicy);
    }
#endif
    return 0;
}

/**
 * @return Whether or not this device should be considered a low end device.
 */
//@CalledByNative
Boolean SysUtils::IsLowEndDevice()
{
    assert(0);
#if 0
    if (sLowEndDeviceInit == FALSE) {
        sLowEndDevice = DetectLowEndDevice();
        sLowEndDeviceInit = TRUE;
    }
    return sLowEndDevice;
#endif
    return FALSE;
}

Boolean SysUtils::DetectLowEndDevice()
{
    assert(0);
#if 0
    if (CommandLine::IsInitialized()) {
        if (CommandLine::GetInstance()->HasSwitch(BaseSwitches::ENABLE_LOW_END_DEVICE_MODE)) {
            return TRUE;
        }

        if (CommandLine::GetInstance()->HasSwitch(BaseSwitches::DISABLE_LOW_END_DEVICE_MODE)) {
            return FALSE;
        }
    }

    if (Build.VERSION.SDK_INT <= ANDROID_LOW_MEMORY_ANDROID_SDK_THRESHOLD) {
        return false;
    }

    Int32 ramSizeKB = AmountOfPhysicalMemoryKB();
    return (ramSizeKB > 0 && ramSizeKB / 1024 < ANDROID_LOW_MEMORY_DEVICE_THRESHOLD_MB);
#endif
    return FALSE;
}

} // namespace Base
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
