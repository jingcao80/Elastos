
#ifndef __CTESTSUITEBUILDER_H__
#define __CTESTSUITEBUILDER_H__

#include "Elastos.Droid.Content.h"
#include "_Eunit.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_Test_SuiteBuilder_CTestSuiteBuilder.h"
#include "TestGrouping.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashSet.h>

using Eunit::Framework::ITestSuite;
using Eunit::Framework::ITest;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Internal::Utility::IPredicate;
using Elastos::Core::Object;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

CarClass(CTestSuiteBuilder)
    , public Object
    , public ITestSuiteBuilder
{
public:
    CTestSuiteBuilder();

    CAR_INTERFACE_DECL()

    /**
     * The given name is automatically prefixed with the package containing the tests to be run.
     * If more than one package is specified, the first is used.
     *
     * @param clazz Use the class from your .apk. Use the class name for the test suite name.
     *              Use the class' classloader in order to load classes for testing.
     *              This is needed when running in the emulator.
     */
    CARAPI constructor(
        /* [in] */ IClassInfo* clazz);

    CARAPI constructor(
        /* [in] */ const String& name,
        /* [in] */ IClassLoader* classLoader);

    CARAPI AddTestClassByName(
        /* [in] */ const String& testPackageName,
        /* [in] */ const String& testClassName,
        /* [in] */ const String& testMethodName,
        /* [in] */ IContext* context);

    CARAPI AddTestSuite(
        /* [in] */ ITestSuite* testSuite);

    /**
     * Include all tests that satisfy the requirements in the given packages and all sub-packages,
     * unless otherwise specified.
     *
     * @param packageNames Names of packages to add.
     * @return The builder for method chaining.
     */
    CARAPI IncludePackages(
        /* [in] */ ArrayOf<String>* packageNames);

    /**
     * Exclude all tests in the given packages and all sub-packages, unless otherwise specified.
     *
     * @param packageNames Names of packages to remove.
     * @return The builder for method chaining.
     */
    CARAPI ExcludePackages(
        /* [in] */ ArrayOf<String>* packageNames);

    /**
     * Exclude tests that fail to satisfy all of the given predicates.
     *
     * @param predicates Predicates to add to the list of requirements.
     * @return The builder for method chaining.
     */
    CARAPI AddRequirements(
        /* [in] */ IList* predicates);

    /**
     * Include all junit tests that satisfy the requirements in the calling class' package and all
     * sub-packages.
     *
     * @return The builder for method chaining.
     */
    CARAPI IncludeAllPackagesUnderHere();

    /**
     * Override the default name for the suite being built. This should generally be called if you
     * call {@link #addRequirements(com.android.internal.util.Predicate[])} to make it clear which
     * tests will be included. The name you specify is automatically prefixed with the package
     * containing the tests to be run. If more than one package is specified, the first is used.
     *
     * @param newSuiteName Prefix of name to give the suite being built.
     * @return The builder for method chaining.
     */
    CARAPI Named(
        /* [in] */ const String& newSuiteName);

    /**
     * Call this method once you've configured your builder as desired.
     *
     * @return The suite containing the requested tests.
     */
    CARAPI Build(
        /* [out] */ ITestSuite** suite);

    /**
     * Exclude tests that fail to satisfy all of the given predicates. If you call this method, you
     * probably also want to call {@link #named(String)} to override the default suite name.
     *
     * @param predicates Predicates to add to the list of requirements.
     * @return The builder for method chaining.
     */
    CARAPI AddRequirements(
        /* [in] */ ArrayOf<IPredicate*>* predicates);

protected:
    /**
     * Subclasses use this method to determine the name of the suite.
     *
     * @return The package and suite name combined.
     */
    CARAPI_(String) GetSuiteName();

private:
    CARAPI_(Boolean) SatisfiesAllPredicates(
        /* [in] */ ITestMethod* test);

    CARAPI AddTest(
        /* [in] */ ITestMethod* testMethod);

    CARAPI AddTest(
        /* [in] */ ITest* test);

    CARAPI_(void) AddSuiteIfNecessary(
        /* [in] */ const String& parentClassname);

private:
    AutoPtr<IContext> mContext;
    AutoPtr<TestGrouping> mTestGrouping;
    AutoPtr<ISet> mPredicates;
    AutoPtr<IList> mTestCases;
    AutoPtr<ITestSuite> mRootSuite;
    AutoPtr<ITestSuite> mSuiteForCurrentClass;
    String mCurrentClassname;
    String mSuiteName;
};

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__CTESTSUITEBUILDER_H__
