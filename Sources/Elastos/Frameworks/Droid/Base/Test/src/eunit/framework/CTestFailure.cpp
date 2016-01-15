
#include <elastos/coredef.h>
#include "eunit/framework/CTestFailure.h"

namespace Eunit {
namespace Framework {

CAR_INTERFACE_IMPL(CTestFailure, Object, ITestFailure)

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
