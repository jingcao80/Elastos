
#ifndef __Elastos_DevSamples_DialogFragmentDemo_CLoginDialogFragment_H__
#define __Elastos_DevSamples_DialogFragmentDemo_CLoginDialogFragment_H__

#include <elastos/droid/app/DialogFragment.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.Droid.Content.h>
#include "_Elastos_DevSamples_DialogFragmentDemo_CLoginDialogFragment.h"

using Elastos::Droid::App::DialogFragment;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

CarClass(CLoginDialogFragment)
    , public DialogFragment
{
public:
    class LoginButtonOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        LoginButtonOnClickListener(
            /* [in] */ CLoginDialogFragment* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 id);

    private:
        CLoginDialogFragment* mHost;
    };

public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

private:
    friend class LoginButtonOnClickListener;

    AutoPtr<ITextView> mUsernameExitText;
    AutoPtr<ITextView> mPasswordExitText;
};

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DialogFragmentDemo_CLoginDialogFragment_H__
