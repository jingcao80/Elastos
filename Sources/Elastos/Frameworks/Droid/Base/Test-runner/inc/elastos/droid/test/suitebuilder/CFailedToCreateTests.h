
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
