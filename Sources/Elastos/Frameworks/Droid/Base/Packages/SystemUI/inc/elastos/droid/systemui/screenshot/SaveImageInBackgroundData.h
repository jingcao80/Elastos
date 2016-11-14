
#ifndef __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__
#define __ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/app/Service.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>
#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"

using Elastos::Droid::App::Service;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IMessenger;
using Elastos::Core::Runnable;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

/**
 * POD used in the AsyncTask which saves an image in the background.
 */
class SaveImageInBackgroundData
    : public Object
{
public:
    SaveImageInBackgroundData();

    CARAPI_(void) ClearImage();

    CARAPI_(void) ClearContext();

public:
    AutoPtr<IContext> mContext;
    AutoPtr<IBitmap> mImage;
    AutoPtr<IUri> mImageUri;
    AutoPtr<IRunnable> mFinisher;
    Int32 mIconSize;
    Int32 mResult;
    Int32 mPreviewWidth;
    Int32 mPreviewheight;
};

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

template <>
struct Conversion<Elastos::Droid::SystemUI::Screenshot::SaveImageInBackgroundData*, IInterface*>
{
    enum { exists = TRUE, exists2Way = FALSE, sameType = FALSE };
};

#endif //__ELASTOS_DROID_SYSTEMUI_SCREENSHOT_SAVEIMAGEINBACKGROUNDDATA_H__