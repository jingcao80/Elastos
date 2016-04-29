
#ifndef __ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__
#define __ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/droid/internal/app/AlertActivity.h>

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Media::Projection::IIMediaProjectionManager;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Widget::ICompoundButton;
using Elastos::Droid::Widget::ICompoundButtonOnCheckedChangeListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Media {

class MediaProjectionPermissionActivity
    : public AlertActivity
    , public IDialogInterfaceOnClickListener
    , public ICompoundButtonOnCheckedChangeListener
{
public:
    CAR_INTERFACE_DECL()

    MediaProjectionPermissionActivity();

    CARAPI constructor();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    // @Override
    CARAPI OnCheckedChanged(
        /* [in] */ ICompoundButton* buttonView,
        /* [in] */ Boolean isChecked);

private:
    CARAPI_(AutoPtr<IIntent>) GetMediaProjectionIntent(
        /* [in] */ Int32 uid,
        /* [in] */ const String& packageName,
        /* [in] */ Boolean permanentGrant);

private:
    const static String TAG;

    Boolean mPermanentGrant;
    String mPackageName;
    Int32 mUid;
    AutoPtr<IIMediaProjectionManager> mService;
};

} // namespace Media
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_MEDIA_MEDIAPROJECTIONPERMISSIONACTIVITY_H__

