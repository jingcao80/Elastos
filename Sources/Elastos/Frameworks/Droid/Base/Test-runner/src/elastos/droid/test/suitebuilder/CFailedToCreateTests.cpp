
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
