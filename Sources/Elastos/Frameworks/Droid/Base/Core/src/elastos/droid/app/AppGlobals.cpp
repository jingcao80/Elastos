#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/AppGlobals.h"
#include "elastos/droid/app/CActivityThread.h"

namespace Elastos {
namespace Droid {
namespace App {

AutoPtr<IApplication> AppGlobals::GetInitialApplication()
{
    return CActivityThread::GetCurrentApplication();
}

String AppGlobals::GetInitialPackage()
{
    return CActivityThread::GetCurrentPackageName();
}

AutoPtr<IIPackageManager> AppGlobals::GetPackageManager()
{
    return CActivityThread::GetPackageManager();
}

Int32 AppGlobals::GetIntCoreSetting(
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue)
{
    AutoPtr<IActivityThread> currentActivityThread = CActivityThread::GetCurrentActivityThread();
    if (currentActivityThread != NULL) {
        Int32 value;
        currentActivityThread->GetIntCoreSetting(key, defaultValue, &value);
        return value;
    }

    return defaultValue;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
