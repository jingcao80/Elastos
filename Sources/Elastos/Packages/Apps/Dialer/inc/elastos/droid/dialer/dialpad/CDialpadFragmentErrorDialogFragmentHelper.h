//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
#define __ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__

#include "_Elastos_Droid_Dialer_Dialpad_CDialpadFragmentErrorDialogFragmentHelper.h"
#include <elastos/core/Singleton.h>

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CarClass(CDialpadFragmentErrorDialogFragmentHelper)
    , public Singleton
    , public IDialpadFragmentErrorDialogFragmentHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI NewInstance(
        /* [in] */ Int32 messageResId,
        /* [out] */ IDialpadFragmentErrorDialogFragment** fragment);

    CARAPI NewInstance(
        /* [in] */ Int32 titleResId,
        /* [in] */ Int32 messageResId,
        /* [out] */ IDialpadFragmentErrorDialogFragment** fragment);

};

} // Dialpad
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_DIALPAD_CDIALPADFRAGMENTERRORDIALOGFRAGMENTHELPER_H__
