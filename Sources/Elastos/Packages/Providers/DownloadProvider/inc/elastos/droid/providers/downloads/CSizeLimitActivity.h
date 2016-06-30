
#ifndef __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__
#define __ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__

#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Net.h"
#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Providers_Downloads_CSizeLimitActivity.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Utility::IQueue;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Downloads {

/**
 * Activity to show dialogs to the user when a download exceeds a limit on download sizes for
 * mobile networks.  This activity gets started by the background download service when a download's
 * size is discovered to be exceeded one of these thresholds.
 */
CarClass(CSizeLimitActivity)
    , public Activity
    , public IDialogInterfaceOnCancelListener
    , public IDialogInterfaceOnClickListener
    , public ISizeLimitActivity
{
public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CSizeLimitActivity();

    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    CARAPI OnNewIntent(
        /* [in] */ IIntent* intent);

    CARAPI OnResume();

private:
    CARAPI_(void) ShowNextDialog();

    CARAPI_(void) ShowDialog(
        /* [in] */ ICursor* cursor);

    CARAPI_(void) DialogClosed();

private:
    AutoPtr<IDialog> mDialog;
    AutoPtr<IQueue> mDownloadsToShow;
    AutoPtr<IUri> mCurrentUri;
    AutoPtr<IIntent> mCurrentIntent;
};

} // namespace Downloads
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_PROVIDERS_DOWNLOADS_CSIZELIMITACTIVITY_H__
