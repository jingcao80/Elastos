
#include "CActionActivityCast.h"
#include "elastos/droid/os/ServiceManager.h"
#include <elastos/System.h>
#include <elastos/core/StringBuilder.h>
#include <elautoptr.h>
#include <stdio.h>
#include "R.h"

using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::Location::ILocation;
using Elastos::Droid::Location::ILocationManager;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Os::IServiceManager;
using Elastos::Droid::Os::CServiceManager;
using Elastos::Droid::Os::ServiceManager;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Core::System;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::CIntentFilter;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

Int32 CActionActivityCast::mNum = 0;

ECode CActionActivityCast::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    PFL_EX(" CActionActivityCast New Message !  %d", mNum++);
    return NOERROR;
}

ECode CActionActivityCast::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActionActivityCast::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActionActivityCast::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActionActivityCast::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActionActivityCast::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActionActivityCast::OnActivityResult(
    /* [in] */ Int32 requestCode,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent *data)
{
    // TODO: Add your code here
    return NOERROR;
}

} // namespace AlarmManagerDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
