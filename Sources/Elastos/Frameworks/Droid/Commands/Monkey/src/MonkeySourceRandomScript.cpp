#include "MonkeySourceRandomScript.h"
#include "CMonkeySourceScript.h"

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Monkey {

MonkeySourceRandomScript::MonkeySourceRandomScript()
    : mVerbose(0)
    , mScriptSources(new List<AutoPtr<IMonkeySourceScript> >())
    , mRandomizeScript(FALSE)
    , mScriptCount(0)
{
}

MonkeySourceRandomScript::~MonkeySourceRandomScript()
{
}

CARAPI MonkeySourceRandomScript::Init(
    /* [in] */ const String& setupFileName,
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    if (!setupFileName.IsNull()) {
        CMonkeySourceScript::New(random, setupFileName, throttle,
            randomizeThrottle, profileWaitTime, deviceSleepTime,
            (IMonkeySourceScript**)&mSetupSource);
        mCurrentSource = mSetupSource;
    }

    Int32 length = scriptFileNames->GetLength();
    for (Int32 i = 0; i < length; ++i) {
        AutoPtr<IMonkeySourceScript> script;
        CMonkeySourceScript::New(random, (*scriptFileNames)[i], throttle,
            randomizeThrottle, profileWaitTime, deviceSleepTime, (IMonkeySourceScript**)&script);
        mScriptSources->PushBack(script);
    }

    mRandom = random;
    mRandomizeScript = randomizeScript;
    return NOERROR;
}

CARAPI MonkeySourceRandomScript::Init(
    /* [in] */ ArrayOf<String> *scriptFileNames,
    /* [in] */ Int64 throttle,
    /* [in] */ Boolean randomizeThrottle,
    /* [in] */ IRandom *random,
    /* [in] */ Int64 profileWaitTime,
    /* [in] */ Int64 deviceSleepTime,
    /* [in] */ Boolean randomizeScript)
{
    return Init(String(), scriptFileNames, throttle, randomizeScript, random, profileWaitTime,
        deviceSleepTime, randomizeScript);
}

CARAPI MonkeySourceRandomScript::GetNextEvent(
        /* [out] */ IMonkeyEvent **event)
{
    VALIDATE_NOT_NULL(event);
    *event = NULL;

    if (mCurrentSource) {
        Int32 numSources = mScriptSources->GetSize();
        if(1 == numSources) {
            mCurrentSource = (*mScriptSources)[0];
        }
        else if (numSources > 1) {
            if (mRandomizeScript) {
                Int32 rdm;
                mRandom->NextInt32(numSources, &rdm);
                mCurrentSource = (*mScriptSources)[rdm];
            }
            else {
                mCurrentSource = (*mScriptSources)[mScriptCount % numSources];
                mScriptCount++;
            }
        }
    }

    if (mCurrentSource) {
        AutoPtr<IMonkeyEvent> nextEvent;
        mCurrentSource->GetNextEvent((IMonkeyEvent**)&nextEvent);
        if (NULL == nextEvent) {
            mCurrentSource = NULL;
        }

        *event = nextEvent;
        REFCOUNT_ADD(*event);
        return NOERROR;
    }

    return NOERROR;
}

CARAPI MonkeySourceRandomScript::SetVerbose(
        /* [in] */ Int32 verbose)
{
    mVerbose = verbose;

    if (mSetupSource) {
        mSetupSource->SetVerbose(verbose);
    }

    List<AutoPtr<IMonkeySourceScript> >::Iterator it = mScriptSources->Begin();
    for(; it != mScriptSources->End(); ++it) {
        (*it)->SetVerbose(verbose);
    }
    return NOERROR;
}

CARAPI MonkeySourceRandomScript::Validate(
    /* [out] */ Boolean *result)
{
    VALIDATE_NOT_NULL(result);
    Boolean temp;
    if (mSetupSource && (mSetupSource->Validate(&temp), temp == FALSE))  {
        *result = FALSE;
        return NOERROR;
    }

    List<AutoPtr<IMonkeySourceScript> >::Iterator it = mScriptSources->Begin();
    for(; it != mScriptSources->End(); ++it) {
        if ((*it)->Validate(&temp), temp ==FALSE) {
            *result = FALSE;
            return NOERROR;
        }
    }

    *result = TRUE;
    return NOERROR;
}

} // namespace Monkey
} // namespace Commands
} // namespace Droid
} // namespace Elastos
