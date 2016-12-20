#ifndef __ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSDIALOGFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include <elastos/droid/app/DialogFragment.h>

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace Droid {
namespace DialerBind {
namespace Analytics {

class AnalyticsDialogFragment
    : public DialogFragment
    , public IAnalyticsInterface
{
public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI SendHitEvent(
        /* [in] */ const String& categoryId,
        /* [in] */ const String& actionId,
        /* [in] */ const String& labelId);

    // @Override
    CARAPI SendScreenView();
};

} // Analytics
} // DialerBind
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALERBIND_ANALYTICS_ANALYTICSDIALOGFRAGMENT_H__
