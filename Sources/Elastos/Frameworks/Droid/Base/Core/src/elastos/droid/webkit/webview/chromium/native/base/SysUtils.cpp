#include "Elastos.Droid.Os.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/webkit/webview/chromium/native/base/SysUtils.h"
#include "elastos/droid/webkit/webview/chromium/native/base/CommandLine.h"
#include "elastos/droid/webkit/webview/chromium/native/base/BaseSwitches.h"
#include "elastos/droid/webkit/webview/chromium/native/base/api/SysUtils_dec.h"
//#include "elastos/droid/os/CStrictMode.h"
#include "elastos/utility/regex/Pattern.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Os::IStrictModeThreadPolicy;
//TODO using Elastos::Droid::Os::CStrictMode;
using Elastos::Core::StringUtils;
using Elastos::IO::IReader;
using Elastos::IO::IFileReader;
using Elastos::IO::CFileReader;
using Elastos::IO::IBufferedReader;
using Elastos::IO::CBufferedReader;
using Elastos::IO::ICloseable;
using Elastos::Utility::Regex::IPattern;
using Elastos::Utility::Regex::Pattern;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IMatchResult;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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

    AutoPtr<IPattern> pattern;
    Pattern::Compile(String("^MemTotal:\\s+([0-9]+) kB$"), (IPattern**)&pattern);
    // Synchronously reading files in /proc in the UI thread is safe.
    //StrictMode.ThreadPolicy oldPolicy = StrictMode.allowThreadDiskReads();
    AutoPtr<IStrictModeThreadPolicy> oldPolicy;
    AutoPtr<IStrictMode> strictMode;
    //TODO CStrictMode::New((IStrictMode**)&strictMode);
    strictMode->AllowThreadDiskReads((IStrictModeThreadPolicy**)&oldPolicy);
    //try {
        AutoPtr<IFileReader> fileReader;
        CFileReader::New(String("/proc/meminfo"), (IFileReader**)&fileReader);
        //try {
            AutoPtr<IBufferedReader> reader;
            CBufferedReader::New(IReader::Probe(fileReader), (IBufferedReader**)&reader);
            //try {
                String line;
                for (;;) {
                    reader->ReadLine(&line);
                    if (line.IsNullOrEmpty()) {
                        Logger::W(TAG, "/proc/meminfo lacks a MemTotal entry?");
                        break;
                    }
                    AutoPtr<IMatcher> m;
                    pattern->Matcher(line, (IMatcher**)&m);
                    Boolean find;
                    if (!(m->Find(&find), find)) continue;

                    String group;
                    Int32 totalMemoryKB = StringUtils::ParseInt32((IMatchResult::Probe(m)->Group(1, &group), group));
                    // Sanity check.
                    if (totalMemoryKB <= 1024) {
                        Logger::W(TAG, "Invalid /proc/meminfo total size in kB: %s", group.string());
                        break;
                    }


                    ICloseable::Probe(reader)->Close();
                    ICloseable::Probe(fileReader)->Close();
                    strictMode->SetThreadPolicy(oldPolicy);
                    return totalMemoryKB;
                }

            //} finally {
                ICloseable::Probe(reader)->Close();
            //}
        //} finally {
                ICloseable::Probe(fileReader)->Close();
        //}
    //} catch (Exception e) {
    //    Log.w(TAG, "Cannot get total physical size from /proc/meminfo", e);
    //} finally {
    strictMode->SetThreadPolicy(oldPolicy);
    //}
    return 0;
}

/**
 * @return Whether or not this device should be considered a low end device.
 */
//@CalledByNative
Boolean SysUtils::IsLowEndDevice()
{
    if (sLowEndDeviceInit == FALSE) {
        sLowEndDevice = DetectLowEndDevice();
        sLowEndDeviceInit = TRUE;
    }
    return sLowEndDevice;
}

Boolean SysUtils::DetectLowEndDevice()
{
    if (CommandLine::IsInitialized()) {
        if (CommandLine::GetInstance()->HasSwitch(BaseSwitches::ENABLE_LOW_END_DEVICE_MODE)) {
            return TRUE;
        }

        if (CommandLine::GetInstance()->HasSwitch(BaseSwitches::DISABLE_LOW_END_DEVICE_MODE)) {
            return FALSE;
        }
    }

    if (Build::VERSION::SDK_INT <= ANDROID_LOW_MEMORY_ANDROID_SDK_THRESHOLD) {
        return false;
    }

    Int32 ramSizeKB = AmountOfPhysicalMemoryKB();
    return (ramSizeKB > 0 && ramSizeKB / 1024 < ANDROID_LOW_MEMORY_DEVICE_THRESHOLD_MB);
}

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
