#include "CNotificationActivity.h"
#include "R.h"
#include <elastos/utility/logging/Slogger.h>

using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace NotificationDemo {


ECode CNotificationActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    Slogger::D("CNotificationActivity", " >>> OnCreate");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::notification);

    return NOERROR;
}

ECode CNotificationActivity::OnStart()
{
    Slogger::D("CNotificationActivity", " >>> OnStart");
    return NOERROR;
}

ECode CNotificationActivity::OnResume()
{
    Slogger::D("CNotificationActivity", " >>> OnResume");
    return NOERROR;
}

ECode CNotificationActivity::OnPause()
{
    Slogger::D("CNotificationActivity", " >>> OnPause");
    return NOERROR;
}

ECode CNotificationActivity::OnStop()
{
    Slogger::D("CNotificationActivity", " >>> OnStop");
    return NOERROR;
}

ECode CNotificationActivity::OnDestroy()
{
    Slogger::D("CNotificationActivity", " >>> OnDestroy");
    return NOERROR;
}

ECode CNotificationActivity::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    return NOERROR;
}


} // namespace NotificationDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
