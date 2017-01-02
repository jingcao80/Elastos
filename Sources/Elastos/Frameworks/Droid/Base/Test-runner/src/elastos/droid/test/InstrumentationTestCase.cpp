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

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/test/InstrumentationTestCase.h"
#include <elastos/core/StringUtils.h>
#include <elastos/utility/logging/Logger.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;
using Elastos::Core::StringUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Os::CBundle;
using Eunit::Framework::EIID_ITest;
using Eunit::Framework::ITestAnnotation;

namespace Elastos {
namespace Droid {
namespace Test {

ECode InstrumentationTestCase::_Runnable::Run()
{
    ECode ec = mRunnable->Run();
    if (FAILED(ec)) (*mExceptions)[0] = ec;
    return NOERROR;
}


ECode InstrumentationTestCase::_RunnableInRunTest::Run()
{
    *mEC = mHost->RunMethod(mTestMethod, mTolerance, mIsRepetitive);
    return *mEC;
}

CAR_INTERFACE_IMPL(InstrumentationTestCase, TestCase, IInstrumentationTestCase)

ECode InstrumentationTestCase::InjectInstrumentation(
    /* [in] */ IInstrumentation* instrumentation)
{
    mInstrumentation = instrumentation;
    return NOERROR;
}

ECode InstrumentationTestCase::GetInstrumentation(
    /* [out] */ IInstrumentation** instrumentation)
{
    VALIDATE_NOT_NULL(instrumentation);
    *instrumentation = mInstrumentation;
    REFCOUNT_ADD(*instrumentation);
    return NOERROR;
}

ECode InstrumentationTestCase::LaunchActivity(
    /* [in] */ const String& pkg,
    /* [in] */ IClassInfo* activityCls,
    /* [in] */ IBundle* extras,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    AutoPtr<IIntent> intent;
    CIntent::New(IIntent::ACTION_MAIN, (IIntent**)&intent);
    if (extras != NULL) {
        intent->PutExtras(extras);
    }
    return LaunchActivityWithIntent(pkg, activityCls, intent, activity);
}

ECode InstrumentationTestCase::LaunchActivityWithIntent(
    /* [in] */ const String& pkg,
    /* [in] */ IClassInfo* activityCls,
    /* [in] */ IIntent* intent,
    /* [out] */ IActivity** activity)
{
    VALIDATE_NOT_NULL(activity);
    String name;
    activityCls->GetName(&name);
    intent->SetClassName(pkg, pkg + "." + name);
    intent->AddFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);
    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);
    FAIL_RETURN(instrumentation->StartActivitySync(intent, activity));
    return instrumentation->WaitForIdleSync();
}

ECode InstrumentationTestCase::RunTestOnUiThread(
    /* [in] */ IRunnable* r)
{
    AutoPtr< ArrayOf<ECode> > exceptions = ArrayOf<ECode>::Alloc(1);
    AutoPtr<_Runnable> _runnable = new _Runnable(r, exceptions);
    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);
    instrumentation->RunOnMainSync(_runnable.Get());
    ECode ec = (*exceptions)[0];
    return ec;
}

ECode InstrumentationTestCase::RunTest()
{
    String name;
    TestCase::GetName(&name);
    AutoPtr<ICharSequence> nameCsq;
    CString::New(name, (ICharSequence**)&nameCsq);
    FAIL_RETURN(TestCase::AssertNotNull(nameCsq));
    AutoPtr<IMethodInfo> method;
    // try {
    // use getMethod to get all public inherited
    // methods. getDeclaredMethods returns all
    // methods of this class but excludes the
    // inherited ones.
    AutoPtr<IClassInfo> clazz;
    ECode ec = _CObject_ReflectClassInfo(TO_IINTERFACE(this), (IClassInfo**)&clazz);
    if (FAILED(ec)) {
        return TestCase::Fail(String("ClassInfo not found"));
    }

    //TODO:
    ec = clazz->GetMethodInfo(name, String(""), (IMethodInfo**)&method);
    if (FAILED(ec)) {
        return TestCase::Fail(String("Method \"") + name + String("\" not found"));
    }

    // if (!Modifier.isPublic(method.getModifiers())) {
    //     fail("Method \""+fName+"\" should be public");
    // }

    Int32 runCount = 1;
    Boolean isRepetitive = FALSE;
    AutoPtr<ITestAnnotation> annotation;
    ec = GetTestAnnotation((ITestAnnotation**)&annotation);
    // if (method.isAnnotationPresent(FlakyTest.class)) {
    //     runCount = method.getAnnotation(FlakyTest.class).tolerance();
    // } else if (method.isAnnotationPresent(RepetitiveTest.class)) {
    //     runCount = method.getAnnotation(RepetitiveTest.class).numIterations();
    //     isRepetitive = true;
    // }
    Boolean isPresent;
    if (SUCCEEDED(ec) && annotation != NULL && (annotation->IsAnnotationPresent(name, String("UiThreadTest"), &isPresent), isPresent)) {
        Int32 tolerance = runCount;
        Boolean repetitive = isRepetitive;
        AutoPtr<IMethodInfo> testMethod = method;
        AutoPtr<IInstrumentation> instrumentation;
        GetInstrumentation((IInstrumentation**)&instrumentation);
        AutoPtr<IRunnable> r = (IRunnable*)new _RunnableInRunTest(testMethod, tolerance, repetitive, &ec, this);
        instrumentation->RunOnMainSync(r);
    }
    else {
        ec = RunMethod(method, runCount, isRepetitive);
    }
    return ec;
}

ECode InstrumentationTestCase::RunMethod(
    /* [in] */ IMethodInfo* runMethod,
    /* [in] */ Int32 tolerance)
{
    return RunMethod(runMethod, tolerance, FALSE);
}

ECode InstrumentationTestCase::RunMethod(
    /* [in] */ IMethodInfo* runMethod,
    /* [in] */ Int32 tolerance,
    /* [in] */ Boolean isRepetitive)
{
    ECode ec = NOERROR;

    Int32 runCount = 0;
    do {
        ec = runMethod->Invoke(TO_IINTERFACE(this), NULL);
        runCount++;
        // Report current iteration number, if test is repetitive
        if (isRepetitive) {
            AutoPtr<IBundle> iterations;
            CBundle::New((IBundle**)&iterations);
            iterations->PutInt32(String("currentiterations"), runCount);
            AutoPtr<IInstrumentation> instrumentation;
            GetInstrumentation((IInstrumentation**)&instrumentation);
            instrumentation->SendStatus(2, iterations);
        }
    } while ((runCount < tolerance) && (isRepetitive || FAILED(ec)));

    return ec;
}

ECode InstrumentationTestCase::SendKeys(
    /* [in] */ const String& keysSequence)
{
    AutoPtr< ArrayOf<String> > keys;
    StringUtils::Split(keysSequence, String(" "), (ArrayOf<String>**)&keys);
    Int32 count = keys->GetLength();

    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);

    for (Int32 i = 0; i < count; i++) {
        String key = (*keys)[i];
        Int32 repeater = key.IndexOf('*');

        Int32 keyCount = 1;
        // try {
        if (repeater != -1) {
            ECode ec = StringUtils::Parse(key.Substring(0, repeater), &keyCount);
            if (FAILED(ec)) {
                Logger::W("ActivityTestCase", "Invalid repeat count: %s", key.string());
                continue;
            }
        }

        if (repeater != -1) {
            key = key.Substring(repeater + 1);
        }

        for (Int32 j = 0; j < keyCount; j++) {
            Int32 keyCode = KeyStringToKeyCode(key);
            if (keyCode == -1) {
                Logger::W("ActivityTestCase", "Unknown keycode: KEYCODE_%s", key.string());
                break;
            }
            ECode ec = instrumentation->SendKeyDownUpSync(keyCode);
            if (FAILED(ec)) {
                assert(ec == (ECode)E_SECURITY_EXCEPTION);
                // Ignore security exceptions that are now thrown
                // when trying to send to another app, to retain
                // compatibility with existing tests.
            }
        }
    }

    return instrumentation->WaitForIdleSync();
}

ECode InstrumentationTestCase::SendKeys(
    /* [in] */ ArrayOf<Int32>* keys)
{
    Int32 count = keys->GetLength();
    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);

    for (Int32 i = 0; i < count; i++) {
        // try {
        ECode ec = instrumentation->SendKeyDownUpSync((*keys)[i]);
        if (FAILED(ec)) {
            assert(ec == (ECode)E_SECURITY_EXCEPTION);
        }
        // } catch (SecurityException e) {
        //     // Ignore security exceptions that are now thrown
        //     // when trying to send to another app, to retain
        //     // compatibility with existing tests.
        // }
    }

    return instrumentation->WaitForIdleSync();
}

ECode InstrumentationTestCase::SendRepeatedKeys(
    /* [in] */ ArrayOf<Int32>* keys)
{
    Int32 count = keys->GetLength();
    if ((count & 0x1) == 0x1) {
        // throw new IllegalArgumentException("The size of the keys array must "
        //         + "be a multiple of 2");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    AutoPtr<IInstrumentation> instrumentation;
    GetInstrumentation((IInstrumentation**)&instrumentation);

    for (Int32 i = 0; i < count; i += 2) {
        Int32 keyCount = (*keys)[i];
        Int32 keyCode = (*keys)[i + 1];
        for (Int32 j = 0; j < keyCount; j++) {
            // try {
            ECode ec = instrumentation->SendKeyDownUpSync(keyCode);
            if (FAILED(ec)) {
                assert(ec == (ECode)E_SECURITY_EXCEPTION);
            }
            // } catch (SecurityException e) {
            //     // Ignore security exceptions that are now thrown
            //     // when trying to send to another app, to retain
            //     // compatibility with existing tests.
            // }
        }
    }

    return instrumentation->WaitForIdleSync();
}

ECode InstrumentationTestCase::TearDown()
{
    return NOERROR;
}

Int32 InstrumentationTestCase::KeyStringToKeyCode(
    /* [in] */ const String& key)
{
    assert(0);
    return -1;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
