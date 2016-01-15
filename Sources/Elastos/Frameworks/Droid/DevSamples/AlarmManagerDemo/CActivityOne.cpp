
#include "CActivityOne.h"
#include "elastos/droid/os/ServiceManager.h"
#include "elastos/droid/os/SystemProperties.h"
#include <elastos/System.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Slogger.h>
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
using Elastos::Droid::Os::SystemProperties;
using Elastos::Droid::App::IIActivityManager;
using Elastos::Core::System;
using Elastos::Core::StringBuilder;
using Elastos::Droid::Content::CIntentFilter;
using Elastos::Utility::Logging::Slogger;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AlarmManagerDemo {

ECode CActivityOne::OnCreate(
    /* [in] */ IBundle* savedInstanceState)
{
    Activity::OnCreate(savedInstanceState);
    SetContentView(R::layout::main);
    Slogger::I("CActivityOne::OnCreate","============OnCreate");

    GetSystemService(String("alarm"), (IInterface**)&am);
    assert(am != NULL);

    // AutoPtr<IContext> context;
    // GetApplicationContext((IContext**)&context);
    // assert(context != NULL);

    // AutoPtr<IIntent> intent;
    // AutoPtr<IModuleInfo> moduleInfo;
    // AutoPtr<IClassInfo> classInfo;
    // StringBuilder sb;
    // sb.Append("/data/data/com.elastos.runtime/elastos/");
    // sb.Append(String("AlarmManagerDemo"));
    // sb.Append("/");
    // sb.Append(String("AlarmManagerDemo"));
    // sb.Append(".eco");
    // String path = sb.ToString();

    // ECode ec = _CReflector_AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    // if (FAILED(ec)) {
    //     PFL_EX("Acquire \"%s\" module info failed!\n", path.string());
    //     Slogger::I("CActivityOne::OnCreate","Acquire \"%s\" module info failed!\n", path.string());
    //     return -1;
    // }

    /************************************************************************
     *
     *1:GetBroadcast
     *
     ************************************************************************/
    // ec = moduleInfo->GetClassInfo(String("CActionBroadCast"), (IClassInfo**)&classInfo);
    // if (FAILED(ec)) {
    //     PFL_EX("Acquire \"%s\" class info failed====!\n", String("CActionBroadCast").string());
    //     Slogger::I("CActivityOne::OnCreate","Acquire \"%s\" class info failed====!\n", String("CActionBroadCast").string());
    //     return -1;
    // }

    /************************************************************************
     *
     *2:GetService
     *
     ************************************************************************/
    // ec = moduleInfo->GetClassInfo(String("CActionServiceCast"), (IClassInfo**)&classInfo);
    // if (FAILED(ec)) {
    //     PFL_EX("Acquire \"%s\" class info failed====!\n", String("CActionServiceCast").string());
    //     return -1;
    // }

    /************************************************************************
     *
     *3:GetActivity
     *
     ************************************************************************/
    // ec = moduleInfo->GetClassInfo(String("CActionActivityCast"), (IClassInfo**)&classInfo);
    // if (FAILED(ec)) {
    //     PFL_EX("Acquire \"%s\" class info failed====!\n", String("CActionActivityCast").string());
    //     return -1;
    // }

    // CIntent::New(context, classInfo, (IIntent**)&intent);
    // assert(intent != NULL);

    // AutoPtr<IPendingIntentHelper> pendingHelper;
    // AutoPtr<IPendingIntent> pendingIntent;
    // CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pendingHelper);
    // assert(pendingHelper != NULL);

    /************************************************************************
     *
     *1:GetBroadcast
     *
     ************************************************************************/
    // FAIL_RETURN(pendingHelper->GetBroadcast(context, 0 /* ignored */, intent,
    //     IIntent::FLAG_ACTIVITY_NEW_TASK, (IPendingIntent**)&pendingIntent))


    /************************************************************************
     *
     *2:GetService
     *
     ************************************************************************/
    // FAIL_RETURN(pendingHelper->GetService(context, 0 /* ignored */, intent,
    //     IIntent::FLAG_ACTIVITY_NEW_TASK, (IPendingIntent**)&pendingIntent))


    /************************************************************************
     *
     *3:GetActivity
     *
     ************************************************************************/
    // FAIL_RETURN(pendingHelper->GetActivity(context, 0 /* ignored */, intent,
    //     IIntent::FLAG_ACTIVITY_NEW_TASK, (IPendingIntent**)&pendingIntent))


    // assert(pendingIntent != NULL);
    // AutoPtr<ISystem> system;
    // Elastos::Core::CSystem::AcquireSingleton((ISystem**)&system);
    // Int64 now;
    // system->GetCurrentTimeMillis(&now);

    /************************************************************************
     *
     *FUN:SetInexactRepeating
     *
     ************************************************************************/
    //alarm 10 Second later,and repet per 5 Seconds. alarm using absolutely time
    //Slogger::I("CActivityOne::OnCreate","============am->SetInexactRepeating");
    //am->SetInexactRepeating(IAlarmManager::RTC_WAKEUP, now+10*1000, 5*1000, pendingIntent);


    /************************************************************************
     *
     *FUN:SetTime
     *
     ************************************************************************/
    //Slogger::I("CActivityOne::OnCreate","============am->SetTime");
    //am->SetTime(now+60*60*1000);


    /************************************************************************
     *
     *FUN:SetTimeZone    eg:   Australia/Sydney    Asia/Shanghai
     *
     ************************************************************************/
    Slogger::I("CActivityOne::OnCreate","============am->SetTimeZone");
    am->SetTimeZone(String("Asia/Shanghai"));  //  String("Australia/Sydney")   String("Asia/Shanghai")

    return NOERROR;
}

ECode CActivityOne::OnStart()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnResume()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnPause()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnStop()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnDestroy()
{
    // TODO: Add your code here
    return NOERROR;
}

ECode CActivityOne::OnActivityResult(
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
