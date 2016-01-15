#include "MonkeyActivityEvent.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyActivityEvent::MonkeyActivityEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mAlarmTime(0)
{}

MonkeyActivityEvent::MonkeyActivityEvent(
    /* [in] */ IComponentName *app)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mApp(app)
    , mAlarmTime(0)
{}

MonkeyActivityEvent::MonkeyActivityEvent(
    /* [in] */ IComponentName *app,
    /* [in] */ Int64 arg)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mApp(app)
    , mAlarmTime(arg)
{}

ECode MonkeyActivityEvent::Init(
    /* [in] */ IComponentName *app)
{
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    mApp = app;
    return NOERROR;
}

ECode MonkeyActivityEvent::Init(
    /* [in] */ IComponentName *app,
    /* [in] */ Int64 arg)
{
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    mApp = app;
    mAlarmTime = arg;
    return NOERROR;
}

AutoPtr<IIntent> MonkeyActivityEvent::GetEvent()
{
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    intent->AddCategory(IIntent::CATEGORY_LAUNCHER);
    intent->SetComponent(mApp);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK |
         IIntent::FLAG_ACTIVITY_RESET_TASK_IF_NEEDED);
    return intent;
}

Int32 MonkeyActivityEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    AutoPtr<IIntent> intent = GetEvent();
    if(verbose > 0) {
        String uriString;
        intent->ToUri(0, &uriString);
        PFL_EX(":Switch: %s", uriString.string());
    }

    if (mAlarmTime != 0) {
        AutoPtr<IBundle> args;
        CBundle::New((IBundle**)&args);
        args->PutInt64(String("alarmTime"), mAlarmTime);
        intent->PutExtras(args);
    }

    Int32 status = 0;
    String nullStr;
    ECode ec = iam->StartActivity(NULL, intent, nullStr, NULL, nullStr, 0,
        0, nullStr, NULL, NULL, &status);
    if(FAILED(ec))
        return IMonkeyEvent::INJECT_FAIL;

    return IMonkeyEvent::INJECT_SUCCESS;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
