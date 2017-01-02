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

#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/test/TestPrinter.h"
#include <elastos/utility/logging/Logger.h>

using Eunit::Framework::EIID_ITestListener;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Test {

TestPrinter::TestPrinter(
    /* [in] */ const String& tag,
    /* [in] */ Boolean onlyFailures)
    : mTag(tag)
    , mOnlyFailures(onlyFailures)
{}

CAR_INTERFACE_IMPL_2(TestPrinter, Object, ITestListener, ITestRunnerListener)

ECode TestPrinter::Started(
    /* [in] */ const String& className)
{
    if (!mOnlyFailures) {
        Logger::I(mTag, "started: %s", className.string());
    }
    return NOERROR;
}

ECode TestPrinter::Finished(
    /* [in] */ const String& className)
{
    if (!mOnlyFailures) {
        Logger::I(mTag, "finished: %s", className.string());
    }
    return NOERROR;
}

ECode TestPrinter::Performance(
    /* [in] */ const String& className,
    /* [in] */ Int64 itemTimeNS,
    /* [in] */ Int32 iterations,
    /* [in] */ IList* itermediates)
{
    Logger::I(mTag, "perf: %s = %lldns/op (done %d times)", className.string(), itemTimeNS, iterations);
    if (itermediates != NULL) {
        Int32 N;
        itermediates->GetSize(&N);
        for (Int32 i = 0; i < N; i++) {
            assert(0);
            // TestRunner.IntermediateTime time = intermediates.get(i);
            // Log.i(mTag, "  intermediate: " + time.name + " = "
            //         + time.timeInNS + "ns");
        }
    }
    return NOERROR;
}

ECode TestPrinter::Passed(
    /* [in] */ const String& className)
{
    if (!mOnlyFailures) {
        Logger::I(mTag, "passed: %s", className.string());
    }
    return NOERROR;
}

ECode TestPrinter::Failed(
    /* [in] */ const String& className,
    /* [in] */ ITestFailure* failure)
{
    Logger::I(mTag, "failed: %s", className.string());
    String message;
    failure->ExceptionMessage(&message);
    Logger::I(mTag, message.string());
    Logger::I(mTag, "----- begin exception -----");
    String trace;
    failure->Trace(&trace);
    Logger::I(mTag, "%s", trace.string());
    Logger::I(mTag, "----- end exception -----");
    return NOERROR;
}

void TestPrinter::Failed(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    mFailedTests.Insert(testStr);
    Failed(testStr, failure);
}

ECode TestPrinter::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    assert(0);
    // Failed(test, ec);
    return NOERROR;
}

ECode TestPrinter::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ITestFailure* failure)
{
    Failed(test, failure);
    return NOERROR;
}

ECode TestPrinter::EndTest(
    /* [in] */ ITest* test)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    Finished(testStr);
    if (mFailedTests.Find(testStr) == mFailedTests.End()) {
        Passed(testStr);
    }
    mFailedTests.Erase(testStr);
    return NOERROR;
}

ECode TestPrinter::StartTest(
    /* [in] */ ITest* test)
{
    String testStr;
    IObject::Probe(test)->ToString(&testStr);
    return Started(testStr);
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
