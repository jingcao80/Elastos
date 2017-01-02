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

#include "elastos/droid/test/suitebuilder/CFailedToCreateTests.h"

using Eunit::Framework::EIID_ITest;
using Eunit::Framework::EIID_ITestCase;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

CAR_INTERFACE_IMPL(CFailedToCreateTests, TestCase, IFailedToCreateTests)

CAR_OBJECT_IMPL(CFailedToCreateTests)

ECode CFailedToCreateTests::constructor(
    /* [in] */ ECode ec)
{
    mException = ec;
    return NOERROR;
}

ECode CFailedToCreateTests::TestSuiteConstructionFailed()
{
    // throw new RuntimeException("Exception during suite construction", exception);
    return E_RUNTIME_EXCEPTION;
}

ECode CFailedToCreateTests::CountTestCases(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    return TestCase::CountTestCases(number);
}

ECode CFailedToCreateTests::Run(
    /* [in] */ ITestResult* result)
{
    return TestCase::Run(result);
}

ECode CFailedToCreateTests::GetTestAnnotation(
    /* [out] */ ITestAnnotation** annotation)
{
    VALIDATE_NOT_NULL(annotation);
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode CFailedToCreateTests::Run(
    /* [out] */ ITestResult** result)
{
    VALIDATE_NOT_NULL(result);
    return TestCase::Run(result);
}

ECode CFailedToCreateTests::RunBare()
{
    return TestCase::RunBare();
}

ECode CFailedToCreateTests::GetName(
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);
    return TestCase::GetName(name);
}

ECode CFailedToCreateTests::SetName(
    /* [in] */ const String& name)
{
    return TestCase::SetName(name);
}

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos
