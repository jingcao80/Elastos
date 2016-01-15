
#include "CInstrumentationWatcher.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::App::IInstrumentation;

namespace Elastos {
namespace Droid {
namespace Commands {
namespace Am {

CInstrumentationWatcher::CInstrumentationWatcher()
    : mFinished(FALSE)
    , mRawMode(FALSE)
    , mHost(NULL)
{
}

ECode CInstrumentationWatcher::InstrumentationStatus(
    /* [in] */ IComponentName * name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle * results)
{
    AutoLock lock(mLock);
    String pretty;
    if (!mRawMode && results != NULL) {
        results->GetString(IInstrumentation::REPORT_KEY_STREAMRESULT, &pretty);
    }

    if (!pretty.IsNull()) {
        PFL_EX("%s", pretty.string());
    }
    else {
        if (results != NULL) {
            AutoPtr<IObjectContainer> keySet;
            results->KeySet((IObjectContainer**)&keySet);
            AutoPtr<IObjectEnumerator> enumerator;
            keySet->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext = FALSE;

            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<ICharSequence> element;
                enumerator->Current((IInterface**)&element);
                String objString;
                element->ToString(&objString);
                AutoPtr<IInterface> value;
                results->Get(objString, (IInterface**)&value);
                //System.out.println(
                //                    "INSTRUMENTATION_STATUS: " + key + "=" + results.get(key));
                //Todo: value type contains many, need to judge perspective?
                PFL_EX("INSTRUMENTATION_STATUS: %s=<todo>", objString.string());
            }
        }
        PFL_EX("INSTRUMENTATION_STATUS_CODE: %d", resultCode);
    }

    mLock.NotifyAll();
    return NOERROR;
}

ECode CInstrumentationWatcher::InstrumentationFinished(
    /* [in] */ IComponentName * name,
    /* [in] */ Int32 resultCode,
    /* [in] */ IBundle * results)
{
    AutoLock lock(mLock);
    String pretty;
    if (!mRawMode && results != NULL) {
        results->GetString(IInstrumentation::REPORT_KEY_STREAMRESULT, &pretty);
    }

    if (!pretty.IsNull()) {
        PFL_EX("%s", pretty.string());
    }
    else {
        if (results != NULL) {
            AutoPtr<IObjectContainer> keySet;
            results->KeySet((IObjectContainer**)&keySet);
            AutoPtr<IObjectEnumerator> enumerator;
            keySet->GetObjectEnumerator((IObjectEnumerator**)&enumerator);
            Boolean hasNext = FALSE;
            while (enumerator->MoveNext(&hasNext), hasNext) {
                AutoPtr<ICharSequence> element;
                enumerator->Current((IInterface**)&element);
                String objString;
                AutoPtr<IInterface> value;
                element->ToString(&objString);
                results->Get(objString, (IInterface**)&value);
                //System.out.println(
                //                    "INSTRUMENTATION_STATUS: " + key + "=" + results.get(key));
                //Todo: value type contains many, need to judge perspective?
                PFL_EX("INSTRUMENTATION_RESULT: %s=<todo>", objString.string());
            }
        }
        PFL_EX("INSTRUMENTATION_CODE: %d", resultCode);
    }

    mFinished = TRUE;
    mLock.NotifyAll();
    return NOERROR;
}

ECode CInstrumentationWatcher::SetRawOutput(
        /* [in] */ Boolean rawMode)
{
    mRawMode = rawMode;
    return NOERROR;
}

ECode CInstrumentationWatcher::WaitForFinish(
    /* [out] */ Boolean* result)
{
/*synchronized (this) {
    while (!mFinished) {
        try {
            if (!mAm.asBinder().pingBinder()) {
                return false;
            }
            wait(1000);
        } catch (InterruptedException e) {
            throw new IllegalStateException(e);
        }
    }
}
return true;*/

//Because interface named PingBinder is not implemented, now just comment following code temperarily
/*
    VALIDATE_NOT_NULL(result);
    AutoLock lock(mLock);
    ECode ec = NOERROR;
    while (!mFinished) {
        Boolean result;
        ECode ec = ((IBinder*)IBinder::Probe(mHost->GetAm()))->PingBinder(&result);
        if (FAILED(ec))
            return ec;
        if (!result) {
            *result = FALSE;
            return ec;
        }
        ec = mLock.Wait(1000);
        if (FAILED(ec))
            return ec;
    }
    *result = TRUE;
    return ec;
*/
    return NOERROR;
}

ECode CInstrumentationWatcher::constructor(
    /* [in] */ Handle32 am)
{
    mHost = (CAm*)am;
    return NOERROR;
}

}
}
}
}

