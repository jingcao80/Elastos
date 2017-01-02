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

#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__

#include "_Elastos_Droid_Dialer_Interactions_CPhoneDisambiguationDialogFragmentHelper.h"
#include <elastos/core/Singleton.h>
#include "Elastos.Droid.App.h"
#include "Elastos.CoreLibrary.Utility.h"

using Elastos::Droid::App::IFragmentManager;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CarClass(CPhoneDisambiguationDialogFragmentHelper)
    , public Singleton
    , public IPhoneDisambiguationDialogFragmentHelper
{
public:
    CAR_INTERFACE_DECL();

    CAR_SINGLETON_DECL();

    CARAPI Show(
        /* [in] */ IFragmentManager* fragmentManager,
        /* [in] */ IArrayList* phoneList,
        /* [in] */ Int32 interactionType,
        /* [in] */ const String& callOrigin);
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_CPHONEDISAMBIGUATIONDIALOGFRAGMENTHELPER_H__
