
#include "elastos/droid/test/TestCaseUtil.h"

using Elastos::Utility::CHashSet;
using Elastos::Utility::ICollection;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::IEnumeration;
using Eunit::Framework::ITestSuite;
using Eunit::Framework::ITestCase;

namespace Elastos {
namespace Droid {
namespace Test {

AutoPtr<IList> TestCaseUtil::GetTests(
    /* [in] */ ITest* test,
    /* [in] */ Boolean flatten)
{
    AutoPtr<ISet> seen;
    CHashSet::New((ISet**)&seen);
    return GetTests(test, flatten, seen);
}

AutoPtr<IList> TestCaseUtil::GetTests(
    /* [in] */ ITest* test,
    /* [in] */ Boolean flatten,
    /* [in] */ ISet* seen)
{
    AutoPtr<IArrayList> testCases;
    CArrayList::New((IArrayList**)&testCases);
    if (test != NULL) {

        AutoPtr<ITest> workingTest;
        /*
         * If we want to run a single TestCase method only, we must not
         * invoke the suite() method, because we will run all test methods
         * of the class then.
         */
        // TODO:
        // if (test instanceof TestCase &&
        //         ((TestCase)test).getName() == null) {
        //     workingTest = invokeSuiteMethodIfPossible(test.getClass(),
        //             seen);
        // }
        if (workingTest == NULL) {
            workingTest = test;
        }

        if (ITestSuite::Probe(workingTest) != NULL) {
            AutoPtr<ITestSuite> testSuite = ITestSuite::Probe(workingTest);
            AutoPtr<IEnumeration> enumeration;
            testSuite->Tests((IEnumeration**)&enumeration);
            Boolean hasMore;
            while (enumeration->HasMoreElements(&hasMore), hasMore) {
                AutoPtr<IInterface> obj;
                enumeration->GetNextElement((IInterface**)&obj);
                AutoPtr<ITest> childTest = ITest::Probe(obj);
                if (flatten) {
                    AutoPtr<IList> tests = GetTests(childTest, flatten, seen);
                    Boolean result;
                    testCases->AddAll(ICollection::Probe(tests), &result);
                }
                else {
                    Boolean result;
                    testCases->Add(childTest, &result);
                }
            }
        }
        else {
            Boolean result;
            testCases->Add(workingTest, &result);
        }
    }
    return IList::Probe(testCases);
}

String TestCaseUtil::GetTestName(
    /* [in] */ ITest* test)
{
    if (ITestCase::Probe(test) != NULL) {
        ITestCase* testCase = ITestCase::Probe(test);
        String name;
        testCase->GetName(&name);
        return name;
    }
    else if (ITestSuite::Probe(test) != NULL) {
        ITestSuite* testSuite = ITestSuite::Probe(test);
        String name;
        testSuite->GetName(&name);
        if (!name.IsNull()) {
            Int32 index = name.LastIndexOf(".");
            if (index > -1) {
                return name.Substring(index + 1);
            }
            else {
                return name;
            }
        }
    }
    return String("");
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
