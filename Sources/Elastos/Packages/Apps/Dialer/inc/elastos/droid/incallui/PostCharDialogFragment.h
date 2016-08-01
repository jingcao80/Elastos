
#ifndef __ELASTOS_DROID_INCALLUI_POSTCHARDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_POSTCHARDIALOGFRAGMENT_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/app/DialogFragment.h"
#include "Elastos.Droid.App.h"

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace Droid {
namespace InCallUI {

/**
 * Pop up an alert dialog with OK and Cancel buttons to allow user to Accept or Reject the WAIT
 * inserted as part of the Dial string.
 */
class PostCharDialogFragment
    : public DialogFragment
{
public:
    PostCharDialogFragment(
        /* [in] */ const String& callId,
        /* [in] */ const String& postDialStr)
        : mCallId(callId)
        , mPostDialStr(postDialStr)
    {}

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

    // @Override
    CARAPI OnCancel(
        /* [in] */ IDialogInterface* dialog);

private:
    String mCallId;
    String mPostDialStr;
};

} // InCallUI
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_INCALLUI_POSTCHARDIALOGFRAGMENT_H__

