
#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Droid::Os::Runnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

class TakeScreenshotService
    : public Elastos::Droid::App::Service
{
private:
    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ IMessenger* callback);

        CARAPI Run();

    private:
        AutoPtr<IMessenger> mCallback;
    };

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("TakeScreenshotService::MyHandler")

        MyHandler(
            /* [in] */ TakeScreenshotService* host);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        TakeScreenshotService* mHost;
    };

public:
    CARAPI constructor();

    // @Override
    CARAPI OnBind(
        /* [in] */ IIntent* intent,
        /* [out] */ IBinder** b);

private:
    const static String TAG;

    static AutoPtr<GlobalScreenshot> mScreenshot;

    AutoPtr<IHandler> mHandler;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_TAKESCREENSHOTSERVICE_H__