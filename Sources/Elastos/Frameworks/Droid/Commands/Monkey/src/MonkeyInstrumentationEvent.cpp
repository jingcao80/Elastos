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

#include "MonkeyInstrumentationEvent.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::CComponentName;
using Elastos::Droid::Content::CComponentNameHelper;
using Elastos::Droid::Content::IComponentNameHelper;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::CBundle;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeyInstrumentationEvent::MonkeyInstrumentationEvent()
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
{
}

MonkeyInstrumentationEvent::MonkeyInstrumentationEvent(
    /* [in] */ const String& testCaseName,
    /* [in] */ const String& runnerName)
    : MonkeyEvent(IMonkeyEvent::EVENT_TYPE_ACTIVITY)
    , mRunnerName(runnerName)
    , mTestCaseName(testCaseName)
{
}

Int32 MonkeyInstrumentationEvent::InjectEvent(
    /* [in] */ IIWindowManager *iwm,
    /* [in] */ IIActivityManager *iam,
    /* [in] */ Int32 verbose)
{
    AutoPtr<IComponentName> cn = NULL;
    AutoPtr<IComponentNameHelper> helper;
    CComponentNameHelper::AcquireSingleton((IComponentNameHelper**)&helper);
    helper->UnflattenFromString(mRunnerName, (IComponentName**)&cn);
    if (cn == NULL || mTestCaseName.IsNull()) {
        return IMonkeyEvent::INJECT_FAIL;
    }

    AutoPtr<IBundle> args;
    CBundle::New((IBundle**)&args);
    args->PutString(String("class"), mTestCaseName);
    Boolean ret;
    iam->StartInstrumentation(cn, String(""), 0, args, NULL, 0, &ret);
    return IMonkeyEvent::INJECT_SUCCESS;
}

ECode MonkeyInstrumentationEvent::Init(
    /* [in] */ const String& testCaseName,
    /* [in] */ const String& runnerName)
{
    mRunnerName = runnerName;
    mTestCaseName = testCaseName;
    MonkeyEvent::Init(IMonkeyEvent::EVENT_TYPE_ACTIVITY);
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos

