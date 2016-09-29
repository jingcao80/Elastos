#ifndef __ELASTOS_DROID_SETTINGS_CBUGREPORTPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_CBUGREPORTPREFERENCE_H__

#include "_Elastos_Droid_Settings_CBugreportPreference.h"
#include "elastos/droid/preference/DialogPreference.h"

using Elastos::Droid::App::IAlertDialogBuilder;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Preference::DialogPreference;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Settings {

CarClass(CBugreportPreference)
    , public DialogPreference
{
public:
    CAR_OBJECT_DECL()

    CBugreportPreference();

    ~CBugreportPreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    //@Override
    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

protected:
    //@Override
    CARAPI OnPrepareDialogBuilder(
        /* [in] */ IAlertDialogBuilder* builder);

    //@Override
    CARAPI ShowDialog(
        /* [in] */ IBundle* state);

    //@Override
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    //@Override
    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CBUGREPORTPREFERENCE_H__