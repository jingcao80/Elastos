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

#include "elastos/droid/dialer/interactions/CPhoneDisambiguationDialogFragmentHelper.h"
#include "elastos/droid/dialer/interactions/PhoneNumberInteraction.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Interactions {

CAR_INTERFACE_IMPL(CPhoneDisambiguationDialogFragmentHelper, Singleton, IPhoneDisambiguationDialogFragmentHelper);

CAR_SINGLETON_IMPL(CPhoneDisambiguationDialogFragmentHelper);

ECode CPhoneDisambiguationDialogFragmentHelper::Show(
    /* [in] */ IFragmentManager* fragmentManager,
    /* [in] */ IArrayList* phoneList,
    /* [in] */ Int32 interactionType,
    /* [in] */ const String& callOrigin)
{
    PhoneNumberInteraction::PhoneDisambiguationDialogFragment::Show(
            fragmentManager, phoneList, interactionType, callOrigin);
    return NOERROR;
}

} // Interactions
} // Dialer
} // Droid
} // Elastos