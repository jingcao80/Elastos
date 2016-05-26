
#ifndef __CTESTFAILURE_H__
#define __CTESTFAILURE_H__

#include "_Eunit_Framework_CTestFailure.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;

namespace Eunit {
namespace Framework {

CarClass(CTestFailure)
    , public Object
    , public ITestFailure
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ ITest* failedTest,
        /* [in] */ ECode ec,
        /* [in] */ const String& message,
        /* [in] */ const String& backtrace);

    /**
     * Gets the failed test.
     */
    CARAPI FailedTest(
        /* [out] */ ITest** test);

    /**
     * Gets the thrown exception.
     */
    CARAPI ThrownException(
        /* [out] */ ECode* ec);

    CARAPI Trace(
        /* [out] */ String* traceStr);

    CARAPI ExceptionMessage(
        /* [out] */ String* message);

    CARAPI IsFailure(
        /* [out] */ Boolean* result);

protected:
    AutoPtr<IWeakReference> mFailedTest;
    ECode mErrorCode;
    String mMessage;
    String mBacktrace;
};

} // namespace Framework
} // namespace Eunit

#endif //__CTESTFAILURE_H__
