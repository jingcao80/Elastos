#ifndef __ELASTOS_DROID_DIALER_UTIL_DIALERUTILS_H__
#define __ELASTOS_DROID_DIALER_UTIL_DIALERUTILS_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;
using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

/**
 * General purpose utility methods for the Dialer.
 */
class DialerUtils
{
public:
    /**
     * Attempts to start an activity and displays a toast with the default error message if the
     * activity is not found, instead of throwing an exception.
     *
     * @param context to start the activity with.
     * @param intent to start the activity with.
     */
    static CARAPI_(void) StartActivityWithErrorToast(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent);

    /**
     * Attempts to start an activity and displays a toast with a provided error message if the
     * activity is not found, instead of throwing an exception.
     *
     * @param context to start the activity with.
     * @param intent to start the activity with.
     * @param msgId Resource ID of the string to display in an error message if the activity is
     *              not found.
     */
    static CARAPI_(void) StartActivityWithErrorToast(
        /* [in] */ IContext* context,
        /* [in] */ IIntent* intent,
        /* [in] */ Int32 msgId);

    /**
     * Returns the component name to use in order to send an SMS using the default SMS application,
     * or null if none exists.
     */
    static CARAPI_(AutoPtr<IComponentName>) GetSmsComponent(
        /* [in] */ IContext* context);

    /**
     * Sets the image asset and text for an empty list view (see empty_list_view.xml).
     *
     * @param emptyListView The empty list view.
     * @param imageResId The resource id for the drawable to set as the image.
     * @param strResId The resource id for the string to set as the message.
     * @param res The resources to obtain the image and string from.
     */
    static CARAPI_(void) ConfigureEmptyListView(
        /* [in] */ IView* emptyListView,
        /* [in] */ Int32 imageResId,
        /* [in] */ Int32 strResId,
        /* [in] */ IResources* res);

    /**
     * Closes an {@link AutoCloseable}, silently ignoring any checked exceptions. Does nothing if
     * null.
     *
     * @param closeable to close.
     */
    // TODO:
    // static CARAPI CloseQuietly(
    //     /* [in] */ IAutoCloseable* closeable);

    /**
     * Joins a list of {@link CharSequence} into a single {@link CharSequence} seperated by a
     * localized delimiter such as ", ".
     *
     * @param resources Resources used to get list delimiter.
     * @param list List of char sequences to join.
     * @return Joined char sequences.
     */
    static CARAPI_(AutoPtr<ICharSequence>) Join(
        /* [in] */ IResources* resources,
        /* [in] */ IIterable* list);

    /**
     * @return True if the application is currently in RTL mode.
     */
    static CARAPI_(Boolean) IsRtl();

    static CARAPI_(void) ShowInputMethod(
        /* [in] */ IView* view);

    static CARAPI_(void) HideInputMethod(
        /* [in] */ IView* view);
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_DIALERUTILS_H__
