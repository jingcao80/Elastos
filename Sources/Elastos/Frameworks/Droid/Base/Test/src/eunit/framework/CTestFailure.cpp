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

#include <elastos/coredef.h>
#include "eunit/framework/CTestFailure.h"

namespace Eunit {
namespace Framework {

CAR_INTERFACE_IMPL(CTestFailure, Object, ITestFailure)

CAR_OBJECT_IMPL(CTestFailure)

ECode CTestFailure::constructor(
    /* [in] */ ITest* failedTest,
    /* [in] */ ECode ec,
    /* [in] */ const String& message,
    /* [in] */ const String& backtrace)
{
    mFailedTest = IWeakReference::Probe(failedTest);
    mErrorCode = ec;
    mMessage = message;
    mBacktrace = backtrace;
    return NOERROR;
}

/**
 * Gets the failed test.
 */
ECode CTestFailure::FailedTest(
    /* [out] */ ITest** test)
{
    VALIDATE_NOT_NULL(test);
    AutoPtr<ITest> t;
    mFailedTest->Resolve(EIID_ITest, (IInterface**)&t);
    *test = t;
    REFCOUNT_ADD(*test);
    return NOERROR;
}

/**
 * Gets the thrown exception.
 */
ECode CTestFailure::ThrownException(
    /* [out] */ ECode* ec)
{
    VALIDATE_NOT_NULL(ec);
    *ec = mErrorCode;
    return NOERROR;
}

ECode CTestFailure::Trace(
    /* [out] */ String* traceStr)
{
    VALIDATE_NOT_NULL(traceStr);
    *traceStr = mBacktrace;
    return NOERROR;
}

ECode CTestFailure::ExceptionMessage(
    /* [out] */ String* message)
{
    VALIDATE_NOT_NULL(message);
    *message = mMessage;
    return NOERROR;
}

ECode CTestFailure::IsFailure(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    *result = mErrorCode == (ECode)E_ASSERTION_FAILED_ERROR;
    return NOERROR;
}

} // namespace Framework
} // namespace Eunit
