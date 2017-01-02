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

#include "elastos/droid/dialer/dialpad/DialpadFragment.h"
#include "elastos/droid/dialer/dialpad/CDialpadFragmentErrorDialogFragmentHelper.h"

namespace Elastos {
namespace Droid {
namespace Dialer {
namespace Dialpad {

CAR_INTERFACE_IMPL(CDialpadFragmentErrorDialogFragmentHelper, Singleton, IDialpadFragmentErrorDialogFragmentHelper);

CAR_SINGLETON_IMPL(CDialpadFragmentErrorDialogFragmentHelper);

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialpadFragment::ErrorDialogFragment::NewInstance(messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

ECode CDialpadFragmentErrorDialogFragmentHelper::NewInstance(
    /* [in] */ Int32 titleResId,
    /* [in] */ Int32 messageResId,
    /* [out] */ IDialpadFragmentErrorDialogFragment** fragment)
{
    VALIDATE_NOT_NULL(fragment);
    AutoPtr<IDialpadFragmentErrorDialogFragment> result =
            DialpadFragment::ErrorDialogFragment::NewInstance(titleResId, messageResId);
    *fragment = result;
    REFCOUNT_ADD(*fragment);

    return NOERROR;
}

} // Dialpad
} // Dialer
} // Droid
} // Elastos