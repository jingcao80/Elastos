
#ifndef __Elastos_DevSamples_DialogFragmentDemo_CEditNameDialogFragment_H__
#define __Elastos_DevSamples_DialogFragmentDemo_CEditNameDialogFragment_H__

#include <elastos/droid/app/DialogFragment.h>
#include "_Elastos_DevSamples_DialogFragmentDemo_CEditNameDialogFragment.h"

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

CarClass(CEditNameDialogFragment)
    , public DialogFragment
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);
};

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DialogFragmentDemo_CEditNameDialogFragment_H__
