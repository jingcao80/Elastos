
#ifndef __CLAUNCHERHELPER_H__
#define __CLAUNCHERHELPER_H__

#include "_CLauncherHelper.h"

using Elastos::Droid::Content::IIntent;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace Launcher {

CarClass(CLauncherHelper)
{
public:
    CARAPI LaunchApp(
        /* [in] */ IIntent* intent);

    CARAPI CloseBootanimation();

    CARAPI LaunchApp2(
        /* [in] */ const String& capsule,
        /* [in] */ const String& action);

    CARAPI StartVideoViewDemo(
        /* [in] */ const String& path);

private:
};

} // namespace Launcher
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif // __CLAUNCHERHELPER_H__
