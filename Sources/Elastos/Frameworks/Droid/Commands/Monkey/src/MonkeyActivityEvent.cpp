//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
