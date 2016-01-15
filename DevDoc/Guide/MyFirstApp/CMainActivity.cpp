
#include "CMainActivity.h"
#include "R.h"
#include <elastos/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace MyFirstApp {

const String CMainActivity::TAG("MyFirstApp");

ECode CMainActivity::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::activity_main);
    Logger::D(TAG,"Activity OnCreate");
    return NOERROR;
}

ECode CMainActivity::OnStart()
{
    Activity::OnStart();
    Logger::D(TAG,"Activity onStart");
    return NOERROR;
}

ECode CMainActivity::OnRestart()
{
    Activity::OnRestart();
    Logger::D(TAG,"Activity onReStart");
    return NOERROR;
}

ECode CMainActivity::OnPause()
{
    Activity::OnPause();
    Logger::D(TAG,"Activity onPause");
    return NOERROR;
}

ECode CMainActivity::OnResume()
{
    Activity::OnResume();
    Logger::D(TAG,"Activity onResume");
    return NOERROR;
}

ECode CMainActivity::OnStop()
{
    Activity::OnStop();
    Logger::D(TAG,"Activity onStop");
    return NOERROR;
}

ECode CMainActivity::OnDestroy()
{
    Activity::OnDestroy();
    Logger::D(TAG,"Activity onDestory");
    return NOERROR;
}

} //namespace Myfirstapp
} //namespace DevSamples
} //namespace Droid
} //namespace Elastos
