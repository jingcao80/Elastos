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

#ifndef __CTESTRESULT_H__
#define __CTESTRESULT_H__

#include "_Eunit_Framework_CTestResult.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <elastos/coredef.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IEnumeration;
using Elastos::Utility::IList;
using Elastos::Utility::IVector;

namespace Eunit {
namespace Framework {

CarClass(CTestResult)
    , public Object
    , public ITestResult
{
private:
    class _Protectable
        : public Object
        , public IProtectable
    {
    public:
        _Protectable(
            /* [in] */ ITestCase* test)
            : mTest(test)
        {}

        CAR_INTERFACE_DECL()

        CARAPI Protect();

    private:
        AutoPtr<ITestCase> mTest;
    };

public:
    CTestResult();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor();

    /**
     * Adds an error to the list of errors. The passed in exception
     * caused the error.
     */
    CARAPI AddError(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    /**
     * Adds a failure to the list of failures. The passed in exception
     * caused the failure.
     */
    CARAPI AddFailure(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    /**
     * Registers a TestListener
     */
    CARAPI AddListener(
        /* [in] */ ITestListener* listener);

    /**
     * Unregisters a TestListener
     */
    CARAPI RemoveListener(
        /* [in] */ ITestListener* listener);

    /**
     * Informs the result that a test was completed.
     */
    CARAPI EndTest(
        /* [in] */ ITest* test);

    /**
     * Gets the number of detected errors.
     */
    CARAPI ErrorCount(
        /* [out] */ Int32* number);

    /**
     * Returns an Enumeration for the errors
     */
    CARAPI Errors(
        /* [out] */ IEnumeration** errors);

    /**
     * Gets the number of detected failures.
     */
    CARAPI FailureCount(
        /*  [out] */ Int32* number);

    /**
     * Returns an Enumeration for the failures
     */
    CARAPI Failures(
        /* [out] */ IEnumeration** failures);

    /**
     * Runs a TestCase.
     */
    CARAPI Run(
        /* [in] */ ITestCase* test);

    /**
     * Gets the number of run tests.
     */
    CARAPI RunCount(
        /* [out] */ Int32* number);

    /**
     * Runs a TestCase.
     */
    CARAPI RunProtected(
        /* [in] */ ITest* test,
        /* [in] */ IProtectable* p);

    /**
     * Checks whether the test run should stop
     */
    CARAPI ShouldStop(
        /* [out] */ Boolean* result);

    /**
     * Informs the result that a test will be started.
     */
    CARAPI StartTest(
        /* [in] */ ITest* test);

    /**
     * Marks that the test run should stop.
     */
    CARAPI Stop();

    /**
     * Returns whether the entire test was successful or not.
     */
    CARAPI WasSuccessful(
        /* [out] */ Boolean* result);

private:
    /**
     * Returns a copy of the listeners.
     */
    CARAPI_(AutoPtr<IList>) CloneListeners();

public:
    // BEGIN android-changed changed types from List<> to Vector<> for API compatibility
    AutoPtr<IVector> mFailures;
    AutoPtr<IVector> mErrors;
    AutoPtr<IVector> mListeners;
    // END android-changed
    Int32 mRunTests;

private:
    Boolean mStop;
};

} // namespace Framework
} // namespace Eunit

#endif //__CTESTRESULT_H__
