
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SCHEDULINGUTILS_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_UTIL_SCHEDULINGUTILS_H__

#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::Runnable;
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
