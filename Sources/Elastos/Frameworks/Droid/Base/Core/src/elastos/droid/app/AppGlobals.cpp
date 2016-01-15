#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/AppGlobals.h"
// #include "elastos/droid/app/CActivityThread.h"

namespace Elastos {
namespace Droid {
namespace App {

AutoPtr<IApplication> AppGlobals::GetInitialApplication()
{
    assert(0 && "TODO");
    return NULL;
    // return CActivityThread::GetCurrentApplication();
}

String AppGlobals::GetInitialPackage()
{
    assert(0 && "TODO");
    return String(NULL);
    // return CActivityThread::GetCurrentPackageName();
}

AutoPtr<IIPackageManager> AppGlobals::GetPackageManager()
{
    assert(0 && "TODO");
    return NULL;
    // return CActivityThread::GetPackageManager();
}

Int32 AppGlobals::GetIntCoreSetting(
    /* [in] */ const String& key,
    /* [in] */ Int32 defaultValue)
{
    AutoPtr<IActivityThread> currentActivityThread;
    assert(0 && "TODO");
    // CActivityThread::GetCurrentActivityThread((IActivityThread**)&currentActivityThread);
    if (currentActivityThread != NULL) {
        Int32 value;
        currentActivityThread->GetIntCoreSetting(key, defaultValue, &value);
        return value;
    }
    else {
        return defaultValue;
    }
}

} // namespace App
} // namespace Droid
} // namespace Elastos
