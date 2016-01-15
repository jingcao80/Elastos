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

