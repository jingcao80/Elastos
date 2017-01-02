//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
