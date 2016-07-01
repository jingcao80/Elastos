#ifndef __ELASTOS_DROID_DIALER_UTIL_CDIALERUTILS_H__
#define __ELASTOS_DROID_DIALER_UTIL_CDIALERUTILS_H__

#include "_Elastos_Droid_Dialer_Util_CDialerUtils.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::View::IView;
using Elastos::Core::ICharSequence;
using Elastos::Utility::IIterable;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Util {

CarClass(CDialerUtils)
    , public Singleton
    , public IDialerUtils
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    /**
     * Attempts to start an activity and displays a toast with the default error message if the
     * activity is not found, instead of throwing an exception.
     *
     * @param context to start the activity with.
     * @param intent to start the activity with.
     */
    CARAPI StartActivityWithErrorToast(
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
    CARAPI StartActivityWithErrorToast(
        /* [in] */ IContext* context,
        /* [in] */ Intent* intent,
        /* [in] */ Int32 msgId);

    /**
     * Returns the component name to use in order to send an SMS using the default SMS application,
     * or null if none exists.
     */
    CARAPI GetSmsComponent(
        /* [in] */ IContext* context,
        /* [out] */ IComponentName** component);

    /**
     * Sets the image asset and text for an empty list view (see empty_list_view.xml).
     *
     * @param emptyListView The empty list view.
     * @param imageResId The resource id for the drawable to set as the image.
     * @param strResId The resource id for the string to set as the message.
     * @param res The resources to obtain the image and string from.
     */
    CARAPI ConfigureEmptyListView(
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
    // CARAPI CloseQuietly(
    //     /* [in] */ IAutoCloseable* closeable);

    /**
     * Joins a list of {@link CharSequence} into a single {@link CharSequence} seperated by a
     * localized delimiter such as ", ".
     *
     * @param resources Resources used to get list delimiter.
     * @param list List of char sequences to join.
     * @return Joined char sequences.
     */
    CARAPI Join(
        /* [in] */ IResources* resources,
        /* [in] */ IIterable* list,
        /* [out] */ ICharSequence** result);

    /**
     * @return True if the application is currently in RTL mode.
     */
    CARAPI IsRtl(
        /* [out] */ Boolean* result);

    CARAPI ShowInputMethod(
        /* [in] */ IView* view);

    CARAPI HideInputMethod(
        /* [in] */ IView* view);
};

} // Util
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_UTIL_CDIALERUTILS_H__
