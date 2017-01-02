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

#ifndef __CFAILEDTOCREATETESTS_H__
#define __CFAILEDTOCREATETESTS_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos_Droid_Test_SuiteBuilder_CFailedToCreateTests.h"
#include <eunit/framework/TestCase.h>

using Eunit::Framework::ITestResult;
using Eunit::Framework::ITestAnnotation;
using Eunit::Framework::TestCase;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

CarClass(CFailedToCreateTests)
    , public TestCase
    , public IFailedToCreateTests
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ECode ec);

    CARAPI TestSuiteConstructionFailed();

    CARAPI CountTestCases(
        /* [out] */ Int32* number);

    CARAPI Run(
        /* [in] */ ITestResult* result);

    CARAPI GetTestAnnotation(
        /* [out] */ ITestAnnotation** annotation);

    CARAPI Run(
        /* [out] */ ITestResult** result);

    CARAPI RunBare();

    CARAPI GetName(
        /* [out] */ String* name);

    CARAPI SetName(
        /* [in] */ const String& name);

private:
    ECode mException;
};

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CFAILEDTOCREATETESTS_H__
