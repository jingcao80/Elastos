
#ifndef __TESTCASEUTIL_H__
#define __TESTCASEUTIL_H__

#include "_Eunit.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Eunit::Framework::ITest;

namespace Elastos {
namespace Droid {
namespace Test {

/**
 * @hide - This is part of a framework that is under development and should not be used for
 * active development.
 */
class TestCaseUtil
{
public:
    static CARAPI_(AutoPtr<IList>) GetTests(
        /* [in] */ ITest* test,
        /* [in] */ Boolean flatten);

    static CARAPI_(String) GetTestName(
        /* [in] */ ITest* test);

private:
    static CARAPI_(AutoPtr<IList>) GetTests(
        /* [in] */ ITest* test,
        /* [in] */ Boolean flatten,
        /* [in] */ ISet* seen);

private:
    TestCaseUtil() {}
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__TESTCASEUTIL_H__
