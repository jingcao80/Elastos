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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SCHEDULINGUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SCHEDULINGUTILS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::Res::IResources;
using Elastos::Core::Runnable;
using Elastos::Droid::Widget::IListView;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Util {

/** Static methods that are useful for scheduling actions to occur at a later time. */
class SchedulingUtils
{
public:
    /** Runs a piece of code after the next layout run */
    static CARAPI_(void) DoAfterLayout(
        /* [in] */ IView* view,
        /* [in] */ IRunnable* runnable);

    /** Runs a piece of code just before the next draw, after layout and measurement */
    static CARAPI_(void) DoOnPreDraw(
        /* [in] */ IView* view,
        /* [in] */ Boolean drawNextFrame,
        /* [in] */ IRunnable* runnable);
};

} // namespace Util
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_UTIL_SCHEDULINGUTILS_H__
