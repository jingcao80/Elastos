#include "CActivityOne.h"
#include "R.h"
#include "Elastos.CoreLibrary.Core.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace DevSamples {
namespace ProgressBarDemo {

static const String TAG("ProgressBarDemo::CActivityOne");

//============================================================
//     CActivityOne
//============================================================

CAR_OBJECT_IMPL(CActivityOne)

ECode CActivityOne::constructor()
{
    Logger::I(TAG, " >> constructor()");
    return Activity::constructor();
}

ECode CActivityOne::OnCreate(
        /* [in] */ IBundle* savedInstanceState)
{
    Logger::I(TAG, " >> OnCreate()");
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    return Activity::OnStart();
}

ECode CActivityOne::OnResume()
{
    return Activity::OnResume();
}

ECode CActivityOne::OnPause()
{
    return Activity::OnPause();
}

ECode CActivityOne::OnStop()
{
    return Activity::OnStop();
}

ECode CActivityOne::OnDestroy()
{
    return Activity::OnDestroy();
}

} // namespace ProgressBarDemo
} // namespace DevSamples
} // namespace Elastos
