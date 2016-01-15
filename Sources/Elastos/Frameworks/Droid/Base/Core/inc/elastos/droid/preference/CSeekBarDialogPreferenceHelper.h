
#ifndef __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCEHELPER_H__
#define __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCEHELPER_H__

#include "_Elastos_Droid_Preference_CSeekBarDialogPreferenceHelper.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ISeekBar;

namespace Elastos {
namespace Droid {
namespace Preference {

CarClass(CSeekBarDialogPreferenceHelper)
    , public Singleton
    , public ISeekBarDialogPreferenceHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI GetSeekBar(
        /* [in] */ IView* dialogView,
        /* [out] */ ISeekBar** bar);
};

}
}
}
#endif // __ELASTOS_DROID_PREFERENCE_CSEEKBARPREFERENCEHELPER_H__