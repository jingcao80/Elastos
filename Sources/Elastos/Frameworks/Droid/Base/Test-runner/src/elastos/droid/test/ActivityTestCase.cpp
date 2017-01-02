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

#include "Elastos.Droid.App.h"
#include "elastos/droid/test/ActivityTestCase.h"

namespace Elastos {
namespace Droid {
namespace Test {

ECode ActivityTestCase::GetActivity(
    /* [out] */ IActivity** activity)
{
    assert(activity != NULL);
    *activity = mActivity;
    REFCOUNT_ADD(*activity);
    return NOERROR;
}

void ActivityTestCase::SetActivity(
    /* [in] */ IActivity* testActivity)
{
    mActivity = testActivity;
}

ECode ActivityTestCase::ScrubClass(
    /* [in] */ IClassInfo* testCaseClass)
{
    assert(0);
    // final Field[] fields = getClass().getDeclaredFields();
    // for (Field field : fields) {
    //     final Class<?> fieldClass = field.getDeclaringClass();
    //     if (testCaseClass.isAssignableFrom(fieldClass) && !field.getType().isPrimitive()
    //             && (field.getModifiers() & Modifier.FINAL) == 0) {
    //         try {
    //             field.setAccessible(true);
    //             field.set(this, null);
    //         } catch (Exception e) {
    //             android.util.Log.d("TestCase", "Error: Could not nullify field!");
    //         }

    //         if (field.get(this) != null) {
    //             android.util.Log.d("TestCase", "Error: Could not nullify field!");
    //         }
    //     }
    // }
    return NOERROR;
}

} // namespace Test
} // namespace Droid
} // namespace Elastos
