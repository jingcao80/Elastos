
#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDTASK_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDTASK_H__

#include "_SystemUI.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/systemui/screenshot/SaveImageInBackgroundData.h"

using Elastos::Droid::App::INotificationBigPictureStyle;
using Elastos::Droid::App::INotificationBuilder;
using Elastos::Droid::App::INotificationManager;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::IO::IFile;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

/**
 * An AsyncTask that saves an image to the media store in the background.
 */
class SaveImageInBackgroundTask
    : public AsyncTask
{
public:
    SaveImageInBackgroundTask(
        /* [in] */ IContext* context,
        /* [in] */ SaveImageInBackgroundData* data,
        /* [in] */ INotificationManager* nManager,
        /* [in] */ Int32 nId);

protected:
    // @Override
    CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result);

    // @Override
    CARAPI OnPostExecute(
        /* [in] */ IInterface* params);

private:
    const static String TAG;

    const static String SCREENSHOTS_DIR_NAME;
    const static String SCREENSHOT_FILE_NAME_TEMPLATE;
    const static String SCREENSHOT_SHARE_SUBJECT_TEMPLATE;

    Int32 mNotificationId;
    AutoPtr<INotificationManager> mNotificationManager;
    AutoPtr<INotificationBuilder> mNotificationBuilder, mPublicNotificationBuilder;
    AutoPtr<IFile> mScreenshotDir;
    String mImageFileName;
    String mImageFilePath;
    Int64 mImageTime;
    AutoPtr<INotificationBigPictureStyle> mNotificationStyle;
    Int32 mImageWidth;
    Int32 mImageHeight;

    // WORKAROUND: We want the same notification across screenshots that we update so that we don't
    // spam a user's notification drawer.  However, we only show the ticker for the saving state
    // and if the ticker text is the same as the previous notification, then it will not show. So
    // for now, we just add and remove a space from the ticker text to trigger the animation when
    // necessary.
    static Boolean sTickerAddSpace;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDTASK_H__