
#include "CTimeService.h"
#include "R.h"
#include "MyTimerTask.h"
#include <unistd.h>

using Elastos::Utility::CTimer;
using Elastos::Utility::EIID_ITimerTask;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Droid::AppWidget::CAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManagerHelper;
using Elastos::Droid::AppWidget::IAppWidgetManager;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Widget::CRemoteViews;
using Elastos::Droid::Widget::IRemoteViews;

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace AppWidgetDemo {

const Int32 CTimeService::mDigits[] = { R::drawable::digit00, R::drawable::digit01,
        R::drawable::digit02, R::drawable::digit03, R::drawable::digit04,
        R::drawable::digit05, R::drawable::digit06, R::drawable::digit07,
        R::drawable::digit08, R::drawable::digit09, };

const Int32 CTimeService::mDigitViews[] = { R::id::img01, R::id::img02, R::id::img03,
        R::id::img04, R::id::img06, R::id::img07, R::id::img09, R::id::img10,
        R::id::img12, R::id::img13, R::id::img15, R::id::img16, R::id::img18,
        R::id::img19 };

CTimeService::CTimeService()
{
    mTimerTask = new MyTimerTask(this);
}

ECode CTimeService::OnCreate()
{
    Service::OnCreate();
    CTimer::New((ITimer**)&mTimer);
    mTimer->Schedule((ITimerTask*)mTimerTask->Probe(EIID_ITimerTask), 0, 1 * 1000);
    return NOERROR;
}

ECode CTimeService::OnBind(
    /* [in] */ IIntent* intent,
    /* [out] */ IBinder** binder)
{
    return NOERROR;
}

ECode CTimeService::OnDestroy()
{
    Service::OnDestroy();
    mTimer->Cancel();
    mTimer = NULL;
    return NOERROR;
}

ECode CTimeService::UpdateView()
{
    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("yyyyMMddHHmmss"), (ISimpleDateFormat**)&sdf);
    AutoPtr<IDate> date;
    CDate::New((IDate**)&date);
    String timeStr;
    sdf->FormatDate(date, &timeStr);
    AutoPtr<IRemoteViews> views;
    String pkgName;
    GetPackageName(&pkgName);
    CRemoteViews::New(pkgName, R::layout::main, (IRemoteViews**)&views);
    assert(views != NULL);
    for (Int32 i = 0; i < timeStr.GetLength(); i++) {
        Int32 num = timeStr.GetChar(i) - 48;
        views->SetImageViewResource(mDigitViews[i], mDigits[num]);
    }
    // AutoPtr<IIntent> intent;
    // CIntent::New(IIntent::ACTION_VIEW, Uri.parse("tel:10086"), (IIntent**)&intent);

    // AutoPtr<IPendingIntentHelper> helper;
    // CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&helper);
    // AutoPtr<IPendingIntent> pendingIntent;
    // AutoPtr<IContext> ctx;
    // GetApplicationContext((IContext**)&ctx);
    // helper->GetActivity(ctx, 0, intent, 0, (IPendingIntent**)&pendingIntent);
    // views->SetOnClickPendingIntent(R::id::digit, pendingIntent);

    AutoPtr<IContext> ctx;
    GetApplicationContext((IContext**)&ctx);
    assert(ctx != NULL);

    AutoPtr<IAppWidgetManagerHelper> appHelper;
    CAppWidgetManagerHelper::AcquireSingleton((IAppWidgetManagerHelper**)&appHelper);
    AutoPtr<IAppWidgetManager> appWidgetManager;
    appHelper->GetInstance(ctx, (IAppWidgetManager**)&appWidgetManager);

    AutoPtr<IModuleInfo> moduleInfo;
    AutoPtr<IClassInfo> classInfo;
    String path = String("/data/data/com.elastos.runtime/elastos/AppWidgetDemo/AppWidgetDemo.eco");
    CReflector::AcquireModuleInfo(path, (IModuleInfo**)&moduleInfo);
    assert(moduleInfo != NULL);
    moduleInfo->GetClassInfo(String("CTimeWidgetProvider"), (IClassInfo**)&classInfo);
    assert(classInfo != NULL);
    AutoPtr<IComponentName> componentName;
    CComponentName::New(ctx, classInfo, (IComponentName**)&componentName);
    return appWidgetManager->UpdateAppWidget(componentName, views);
}

} // namespace AppWidgetDemo
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos
