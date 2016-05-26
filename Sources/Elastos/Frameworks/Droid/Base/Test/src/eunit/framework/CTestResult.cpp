
#include <Elastos.CoreLibrary.Utility.h>
#include "eunit/framework/CTestResult.h"
#include <elastos/core/AutoLock.h>

using Elastos::Core::AutoLock;
using Elastos::Utility::CVector;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IIterator;
using Elastos::Utility::ICollection;

namespace Eunit {
namespace Framework {

CAR_INTERFACE_IMPL(CTestResult::_Protectable, Object, IProtectable)

ECode CTestResult::_Protectable::Protect()
{
    return mTest->RunBare();
}


CAR_INTERFACE_IMPL(CTestResult, Object, ITestResult)

CAR_OBJECT_IMPL(CTestResult)

CTestResult::CTestResult()
    : mRunTests(0)
    , mStop(FALSE)
{
    CVector::New((IVector**)&mFailures);
    CVector::New((IVector**)&mErrors);
    CVector::New((IVector**)&mListeners);
}

ECode CTestResult::constructor()
{
    return NOERROR;
}

/**
 * Adds an error to the list of errors. The passed in exception
 * caused the error.
 */
ECode CTestResult::AddError(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    // fErrors.add(new TestFailure(test, t));
    //     for (TestListener each : cloneListeners())
    //         each.addError(test, t);
    assert(0);
    return NOERROR;
}

/**
 * Adds a failure to the list of failures. The passed in exception
 * caused the failure.
 */
ECode CTestResult::AddFailure(
    /* [in] */ ITest* test,
    /* [in] */ ECode ec)
{
    AutoPtr< ArrayOf<ITestFailure*> > failures;
    test->GetFailures((ArrayOf<ITestFailure*>**)&failures);
    if (failures == NULL) return NOERROR;
    for (Int32 i = 0; i < failures->GetLength(); ++i) {
        mFailures->AddElement((*failures)[i]);
    }
    AutoPtr<IList> listeners = CloneListeners();
    AutoPtr<IIterator> it;
    listeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> each;
        it->GetNext((IInterface**)&each);
        ITestListener* listener = ITestListener::Probe(each);
        for (Int32 i = 0; i < failures->GetLength(); ++i) {
            listener->AddFailure(test, (*failures)[i]);
        }
    }
    return NOERROR;
}

/**
 * Registers a TestListener
 */
ECode CTestResult::AddListener(
    /* [in] */ ITestListener* listener)
{
    AutoLock lock(this);
    Boolean result;
    return mListeners->Add(listener, &result);
}

/**
 * Unregisters a TestListener
 */
ECode CTestResult::RemoveListener(
    /* [in] */ ITestListener* listener)
{
    AutoLock lock(this);
    Boolean result;
    return mListeners->Remove(listener, &result);
}

AutoPtr<IList> CTestResult::CloneListeners()
{
    AutoLock lock(this);
    AutoPtr<IArrayList> result;
    CArrayList::New((IArrayList**)&result);
    Boolean succeeded;
    result->AddAll(ICollection::Probe(mListeners), &succeeded);
    return IList::Probe(result);
}

/**
 * Informs the result that a test was completed.
 */
ECode CTestResult::EndTest(
    /* [in] */ ITest* test)
{
    AutoPtr<IList> listeners = CloneListeners();
    AutoPtr<IIterator> it;
    listeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> each;
        it->GetNext((IInterface**)&each);
        ITestListener::Probe(each)->EndTest(test);
    }
    return NOERROR;
}

/**
 * Gets the number of detected errors.
 */
ECode CTestResult::ErrorCount(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    mErrors->GetSize(number);
    return NOERROR;
}

/**
 * Returns an Enumeration for the errors
 */
ECode CTestResult::Errors(
    /* [out] */ IEnumeration** errors)
{
    assert(0);
    return NOERROR;
}

/**
 * Gets the number of detected failures.
 */
ECode CTestResult::FailureCount(
    /*  [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    mFailures->GetSize(number);
    return NOERROR;
}

/**
 * Returns an Enumeration for the failures
 */
ECode CTestResult::Failures(
    /* [out] */ IEnumeration** failures)
{
    assert(0);
    return NOERROR;
}

/**
 * Runs a TestCase.
 */
ECode CTestResult::Run(
    /* [in] */ ITestCase* test)
{
    StartTest(ITest::Probe(test));
    AutoPtr<_Protectable> p = new _Protectable(test);
    RunProtected(ITest::Probe(test), p);

    EndTest(ITest::Probe(test));
    return NOERROR;
}

/**
 * Gets the number of run tests.
 */
ECode CTestResult::RunCount(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);

    AutoLock lock(this);
    *number = mRunTests;
    return NOERROR;
}

/**
 * Runs a TestCase.
 */
ECode CTestResult::RunProtected(
    /* [in] */ ITest* test,
    /* [in] */ IProtectable* p)
{
    ECode ec = p->Protect();
    Boolean failed;
    test->IsFailed(&failed);
    if (ec == (ECode)E_ASSERTION_FAILED_ERROR || failed) {
        AddFailure(test, ec);
    }
    // catch (ThreadDeath e) { // don't catch ThreadDeath by accident
    //     throw e;
    // }
    else if (FAILED(ec)) AddError(test, ec);
    return NOERROR;
}

/**
 * Checks whether the test run should stop
 */
ECode CTestResult::ShouldStop(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(this);
    *result = mStop;
    return NOERROR;
}

/**
 * Informs the result that a test will be started.
 */
ECode CTestResult::StartTest(
    /* [in] */ ITest* test)
{
    Int32 count;
    test->CountTestCases(&count);
    {
        AutoLock lock(this);
        mRunTests += count;
    }
    AutoPtr<IList> listeners = CloneListeners();
    AutoPtr<IIterator> it;
    listeners->GetIterator((IIterator**)&it);
    Boolean hasNext;
    while (it->HasNext(&hasNext), hasNext) {
        AutoPtr<IInterface> each;
        it->GetNext((IInterface**)&each);
        ITestListener::Probe(each)->StartTest(test);
    }
    return NOERROR;
}

/**
 * Marks that the test run should stop.
 */
ECode CTestResult::Stop()
{
    AutoLock lock(this);
    mStop = TRUE;
    return NOERROR;
}

/**
 * Returns whether the entire test was successful or not.
 */
ECode CTestResult::WasSuccessful(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);
    AutoLock lock(this);
    Int32 fCount, eCount;
    FailureCount(&fCount);
    ErrorCount(&eCount);
    *result = fCount == 0 && eCount == 0;
    return NOERROR;
}

} // namespace Framework
} // namespace Eunit
