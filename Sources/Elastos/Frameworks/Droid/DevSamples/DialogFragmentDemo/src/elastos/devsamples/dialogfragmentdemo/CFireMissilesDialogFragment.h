
#ifndef __Elastos_DevSamples_DialogFragmentDemo_CFireMissilesDialogFragment_H__
#define __Elastos_DevSamples_DialogFragmentDemo_CFireMissilesDialogFragment_H__

#include <elastos/droid/app/DialogFragment.h>
#include "_Elastos_DevSamples_DialogFragmentDemo_CFireMissilesDialogFragment.h"

using Elastos::Droid::App::DialogFragment;

namespace Elastos {
namespace DevSamples {
namespace DialogFragmentDemo {

CarClass(CFireMissilesDialogFragment)
    , public DialogFragment
{
public:
    CAR_OBJECT_DECL()

    CARAPI constructor();

    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);
};

} // namespace DialogFragmentDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_DialogFragmentDemo_CFireMissilesDialogFragment_H__
