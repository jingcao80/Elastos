#ifndef __TESTFILEPRINTER_H__
#define __TESTFILEPRINTER_H__

#include <elastos/droid/ext/frameworkdef.h>
#include "_Elastos.Droid.Test.h"
#include "_Eunit.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashSet.h>

using Elastos::Core::Object;
using Elastos::IO::IDataOutput;
using Elastos::IO::IFileOutputStream;
using Elastos::Utility::Etl::HashSet;
using Elastos::Utility::IList;
using Eunit::Framework::ITest;
using Eunit::Framework::ITestListener;
using Eunit::Framework::ITestFailure;

namespace Elastos {
namespace Droid {
namespace Test {

class TestFilePrinter
    : public Object
    , public ITestListener
    , public ITestRunnerListener
{
public:
    TestFilePrinter(
        /* [in] */ const String& tag,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean onlyFailures);

    ~TestFilePrinter();

    CAR_INTERFACE_DECL()

    CARAPI Started(
        /* [in] */ const String& className);

    CARAPI Finished(
        /* [in] */ const String& className);

    CARAPI Performance(
        /* [in] */ const String& className,
        /* [in] */ Int64 itemTimeNS,
        /* [in] */ Int32 iterations,
        /* [in] */ IList* itermediates);

    CARAPI Passed(
        /* [in] */ const String& className);

    CARAPI Failed(
        /* [in] */ const String& className,
        /* [in] */ ITestFailure* failure);

    CARAPI AddError(
        /* [in] */ ITest* test,
        /* [in] */ ECode ec);

    CARAPI AddFailure(
        /* [in] */ ITest* test,
        /* [in] */ ITestFailure* failure);

    CARAPI EndTest(
        /* [in] */ ITest* test);

    CARAPI StartTest(
        /* [in] */ ITest* test);

private:
    CARAPI_(void) Failed(
        /* [in] */ ITest* test,
        /* [in] */ ITestFailure* failure);

private:
    static const String PATH;
    String mTag;
    String mPrefix;
    AutoPtr<IFileOutputStream> mFileOut;
    AutoPtr<IDataOutput> mOutput;
    Boolean mOnlyFailures;
    HashSet<String> mFailedTests;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__TESTFILEPRINTER_H__
