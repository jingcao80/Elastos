
#ifndef __TESTGROUPING_H__
#define __TESTGROUPING_H__

#include <elastos/droid/ext/frameworkdef.h>
#include <Elastos.Droid.Internal.h>
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Core::IComparator;
using Elastos::Core::IClassLoader;
using Elastos::Utility::IList;
using Elastos::Utility::ISortedSet;
using Elastos::Utility::ISet;
using Elastos::Utility::ICollection;
using Elastos::Droid::Internal::Utility::IPredicate;

namespace Elastos {
namespace Droid {
namespace Test {
namespace SuiteBuilder {

class TestGrouping : public Object
{
private:
    /**
     * Sort classes by their simple names (i.e. without the package prefix), using
     * their packages to sort classes with the same name.
     */
    class SortBySimpleName
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

    /**
     * Sort classes by their fully qualified names (i.e. with the package
     * prefix).
     */
    class SortByFullyQualifiedName
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);
    };

    class TestCasePredicate
        : public Object
        , public IPredicate
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Apply(
            /* [in] */ IInterface* t,
            /* [out] */ Boolean* result);

    private:
        CARAPI_(Boolean) HasValidConstructor(
            /* [in] */ IClassInfo* aClass);
    };

    class TestMethodPredicate
        : public Object
        , public IPredicate
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI Apply(
            /* [in] */ IInterface* t,
            /* [out] */ Boolean* result);
    };

public:
    TestGrouping(
        /* [in] */ IComparator* comparator);

    /**
     * @return A list of all tests in the package, including small, medium, large,
     *         flaky, and suppressed tests. Includes sub-packages recursively.
     */
    CARAPI_(AutoPtr<IList>) GetTests();

    CARAPI_(AutoPtr<ISortedSet>) GetTestCaseClasses();

    CARAPI_(Boolean) Equals(
        /* [in] */ IInterface* o);

    CARAPI_(Int32) GetHashCode();

    /**
     * Include all tests in the given packages and all their sub-packages, unless otherwise
     * specified. Each of the given packages must contain at least one test class, either directly
     * or in a sub-package.
     *
     * @param packageNames Names of packages to add.
     * @return The {@link TestGrouping} for method chaining.
     */
    CARAPI_(AutoPtr<TestGrouping>) AddPackagesRecursive(
        /* [in] */ ArrayOf<String>* packageNames);

    /**
     * Exclude all tests in the given packages and all their sub-packages, unless otherwise
     * specified.
     *
     * @param packageNames Names of packages to remove.
     * @return The {@link TestGrouping} for method chaining.
     */
    CARAPI_(AutoPtr<TestGrouping>) RemovePackagesRecursive(
        /* [in] */ ArrayOf<String>* packageNames);

    /**
     * @return The first package name passed to {@link #addPackagesRecursive(String[])}, or null
     *         if that method was never called.
     */
    CARAPI_(String) GetFirstIncludedPackage();

    CARAPI_(void) SetClassLoader(
        /* [in] */ IClassLoader* classLoader);

protected:
    CARAPI_(AutoPtr<IList>) GetTestMethods(
        /* [in] */ IClassInfo* testCaseClass);

private:
    CARAPI_(AutoPtr<IList>) TestCaseClassesInPackage(
        /* [in] */ const String& packageName);

    CARAPI_(AutoPtr<IList>) SelectTestClasses(
        /* [in] */ ISet* allClasses);

    CARAPI_(AutoPtr<IList>) Select(
        /* [in] */ ICollection* items,
        /* [in] */ IPredicate* predicate);

public:
    AutoPtr<ISortedSet> mTestCaseClasses;

    static AutoPtr<IComparator> SORT_BY_SIMPLE_NAME;

    static AutoPtr<IComparator> SORT_BY_FULLY_QUALIFIED_NAME;

protected:
    String mFirstIncludedPackage;

private:
    static const String TAG;
    AutoPtr<IClassLoader> mClassLoader;
};

} // namespace SuiteBuilder
} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__TESTGROUPING_H__
