
#ifndef __EUNIT_FRAMEWORK_TESTCASE_H__
#define __EUNIT_FRAMEWORK_TESTCASE_H__

#include <elastos/coredef.h>
#include <_Eunit.h>
#include "eunit/framework/Assert.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/List.h>

using Elastos::Core::Object;
using Elastos::Utility::Etl::List;

namespace Eunit {
namespace Framework {

/**
 * A test case defines the fixture to run multiple tests. To define a test case<br/>
 * <ol>
 *   <li>implement a subclass of <code>TestCase</code></li>
 *   <li>define instance variables that store the state of the fixture</li>
 *   <li>initialize the fixture state by overriding {@link #setUp()}</li>
 *   <li>clean-up after a test by overriding {@link #tearDown()}.</li>
 * </ol>
 * Each test runs in its own fixture so there
 * can be no side effects among test runs.
 * Here is an example:
 * <pre>
 * public class MathTest extends TestCase {
 *    protected double fValue1;
 *    protected double fValue2;
 *
 *    protected void setUp() {
 *       fValue1= 2.0;
 *       fValue2= 3.0;
 *    }
 * }
 * </pre>
 *
 * For each test implement a method which interacts
 * with the fixture. Verify the expected results with assertions specified
 * by calling {@link junit.framework.Assert#assertTrue(String, boolean)} with a boolean.
 * <pre>
 *    public void testAdd() {
 *       double result= fValue1 + fValue2;
 *       assertTrue(result == 5.0);
 *    }
 * </pre>
 *
 * Once the methods are defined you can run them. The framework supports
 * both a static type safe and more dynamic way to run a test.
 * In the static way you override the runTest method and define the method to
 * be invoked. A convenient way to do so is with an anonymous inner class.
 * <pre>
 * TestCase test= new MathTest("add") {
 *    public void runTest() {
 *       testAdd();
 *    }
 * };
 * test.run();
 * </pre>
 *
 * The dynamic way uses reflection to implement {@link #runTest()}. It dynamically finds
 * and invokes a method.
 * In this case the name of the test case has to correspond to the test method
 * to be run.
 * <pre>
 * TestCase test= new MathTest("testAdd");
 * test.run();
 * </pre>
 *
 * The tests to be run can be collected into a TestSuite. JUnit provides
 * different <i>test runners</i> which can run a test suite and collect the results.
 * A test runner either expects a static method <code>suite</code> as the entry
 * point to get a test to run or it will extract the suite automatically.
 * <pre>
 * public static Test suite() {
 *    suite.addTest(new MathTest("testAdd"));
 *    suite.addTest(new MathTest("testDivideByZero"));
 *    return suite;
 * }
 * </pre>
 * @see TestResult
 * @see TestSuite
 */
class ECO_PUBLIC TestCase
    : public Object
    , public Assert
    , public ITest
    , public ITestCase
{
public:
    /**
     * No-arg constructor to enable serialization. This method
     * is not intended to be used by mere mortals without calling setName().
     */
    TestCase();

    /**
     * Constructs a test case with the given name.
     */
    TestCase(
        /* [in] */ const String& name);

    virtual CARAPI Initialize();

    CARAPI_(PInterface) Probe(
        /* [in] */ REIID riid);

    CARAPI_(UInt32) AddRef();

    CARAPI_(UInt32) Release();

    CARAPI GetInterfaceID(
        /* [in] */ IInterface *pObject,
        /* [out] */ InterfaceID *pIID);

    CARAPI Aggregate(
        /* [in] */ AggrType aggrType,
        /* [in] */ PInterface pObject);

    CARAPI GetDomain(
        /* [out] */ PInterface *ppObject);

    CARAPI Equals(
        /* [in] */ IInterface* other,
        /* [out] */ Boolean * result);

    CARAPI GetHashCode(
        /* [out] */ Int32* hash);

    /**
     * Counts the number of test cases executed by run(TestResult result).
     */
    CARAPI CountTestCases(
        /* [out] */ Int32* number);

    /**
     * A convenience method to run this test, collecting the results with a
     * default TestResult object.
     *
     * @see TestResult
     */
    CARAPI Run(
        /* [out] */ ITestResult** result);

    /**
     * Runs the test case and collects the results in TestResult.
     */
    CARAPI Run(
        /* [in] */ ITestResult* result);

    CARAPI IsFailed(
        /* [out] */ Boolean* result);

    CARAPI AddFailure(
        /* [in] */ ECode ec,
        /* [in] */ const String& message,
        /* [in] */ const String& backtrace);

    CARAPI GetFailures(
        /* [out, callee] */ ArrayOf<ITestFailure*>** failures);

    /**
     * Runs the bare test sequence.
     * @throws Throwable if any exception is thrown
     */
    CARAPI RunBare();

    /**
     * Returns a string representation of the test case
     */
    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Gets the name of a TestCase
     * @return the name of the TestCase
     */
    CARAPI GetName(
        /* [out] */ String* name);

    /**
     * Sets the name of a TestCase
     * @param name the name to set
     */
    CARAPI SetName(
        /* [in] */ const String& name);

protected:
    /**
     * Creates a default TestResult object
     *
     * @see TestResult
     */
    virtual CARAPI_(AutoPtr<ITestResult>) CreateResult();

    /**
     * Override to run the test and assert its state.
     * @throws Throwable if any exception is thrown
     */
    virtual CARAPI RunTest();

    /**
     * Sets up the fixture, for example, open a network connection.
     * This method is called before a test is executed.
     */
    virtual CARAPI SetUp();

    /**
     * Tears down the fixture, for example, close a network connection.
     * This method is called after a test is executed.
     */
    virtual CARAPI TearDown();

private:
    /**
     * the name of the test case
     */
    String mName;
    List< AutoPtr<ITestFailure> > mFailures;
};

} // namespace Framework
} // namespace Eunit

#endif //__EUNIT_FRAMEWORK_TESTCASE_H__
