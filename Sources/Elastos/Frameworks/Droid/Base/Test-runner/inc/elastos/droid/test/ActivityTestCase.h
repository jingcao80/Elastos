
#ifndef __ACTIVITYTESTCASE_H__
#define __ACTIVITYTESTCASE_H__

#include <elastos/droid/ext/frameworkdef.h>
#include <elastos/droid/test/InstrumentationTestCase.h>

namespace Elastos {
namespace Droid {
namespace Test {

/**
 * This is common code used to support Activity test cases.  For more useful classes, please see
 * {@link android.test.ActivityUnitTestCase} and
 * {@link android.test.ActivityInstrumentationTestCase}.
 */

class ECO_PUBLIC ActivityTestCase : public InstrumentationTestCase
{
protected:
    /**
     * @return Returns the activity under test.
     */
    virtual CARAPI GetActivity(
        /* [out] */ IActivity** activity);

    /**
     * Set the activity under test.
     * @param testActivity The activity under test
     */
    CARAPI_(void) SetActivity(
        /* [in] */ IActivity* testActivity);

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
    CARAPI ScrubClass(
        /* [in] */ IClassInfo* testCaseClass);

private:
    /**
     * The activity that will be set up for use in each test method.
     */
    AutoPtr<IActivity> mActivity;
};

} // namespace Test
} // namespace Droid
} // namespace Elastos

#endif //__ACTIVITYTESTCASE_H__
