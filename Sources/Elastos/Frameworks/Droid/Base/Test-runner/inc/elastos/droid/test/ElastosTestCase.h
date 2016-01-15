
#ifndef __ELASTOSTESTCASE_H__
#define __ELASTOSTESTCASE_H__

#include <elastos/droid/ext/frameworkdef.h>
#include <_Elastos.Droid.Test.h>
#include <eunit/framework/TestCase.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Eunit::Framework::TestCase;

namespace Elastos {
namespace Droid {
namespace Test {

class ECO_PUBLIC ElastosTestCase
    : public TestCase
    , public IElastosTestCase
{
public:
    CAR_INTERFACE_DECL()

    CARAPI TestElastosTestCaseSetupProperly();

    CARAPI SetContext(
        /* [in] */ IContext* context);

    CARAPI GetContext(
        /* [out] */ IContext** context);

    /**
     * Test context can be used to access resources from the test's own package
     * as opposed to the resources from the test target package. Access to the
     * latter is provided by the context set with the {@link #setContext}
     * method.
     *
     * @hide
     */
    CARAPI SetTestContext(
        /* [in] */ IContext* context);

    /**
     * @hide
     */
    CARAPI GetTestContext(
        /* [out] */ IContext** context);

    /**
     * Asserts that launching a given activity is protected by a particular permission by
     * attempting to start the activity and validating that a {@link SecurityException}
     * is thrown that mentions the permission in its error message.
     *
     * Note that an instrumentation isn't needed because all we are looking for is a security error
     * and we don't need to wait for the activity to launch and get a handle to the activity.
     *
     * @param packageName The package name of the activity to launch.
     * @param className The class of the activity to launch.
     * @param permission The name of the permission.
     */
    CARAPI AssertActivityRequiresPermission(
        /* [in] */ const String& packageName,
        /* [in] */ const String& className,
        /* [in] */ const String& permission);

    /**
     * Asserts that reading from the content uri requires a particular permission by querying the
     * uri and ensuring a {@link SecurityException} is thrown mentioning the particular permission.
     *
     * @param uri The uri that requires a permission to query.
     * @param permission The permission that should be required.
     */
    CARAPI AssertReadingContentUriRequiresPermission(
        /* [in] */ IUri* uri,
        /* [in] */ const String& permission);

    /**
     * Asserts that writing to the content uri requires a particular permission by inserting into
     * the uri and ensuring a {@link SecurityException} is thrown mentioning the particular
     * permission.
     *
     * @param uri The uri that requires a permission to query.
     * @param permission The permission that should be required.
     */
    CARAPI AssertWritingContentUriRequiresPermission(
        /* [in] */ IUri* uri,
        /* [in] */ const String& permission);

protected:
    /**
     * This function is called by various TestCase implementations, at tearDown() time, in order
     * to scrub out any class variables.  This protects against memory leaks in the case where a
     * test case creates a non-static inner class (thus referencing the test case) and gives it to
     * someone else to hold onto.
     *
     * @param testCaseClass The class of the derived TestCase implementation.
     *
     * @throws IllegalAccessException
     */
    CARAPI_(void) ScrubClass(
        /* [in] */ IClassInfo* testCaseClass);


protected:
    CARAPI SetUp();

    CARAPI TearDown();

protected:
    AutoPtr<IContext> mContext;

private:
    AutoPtr<IContext> mTestContext;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOSTESTCASE_H__
