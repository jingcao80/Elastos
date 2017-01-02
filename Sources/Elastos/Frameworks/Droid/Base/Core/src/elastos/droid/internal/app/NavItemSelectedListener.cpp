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

#include "elastos/droid/internal/app/NavItemSelectedListener.h"

using Elastos::Droid::Widget::EIID_IAdapterViewOnItemSelectedListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

CAR_INTERFACE_IMPL(NavItemSelectedListener, Object, IAdapterViewOnItemSelectedListener)

ECode  NavItemSelectedListener::constructor(
    /* [in] */ IActionBarOnNavigationListener* listener)
{
    mListener = listener;
    return NOERROR;
}

ECode  NavItemSelectedListener::OnItemSelected(
    /* [in] */ IAdapterView* parent,
    /* [in] */ IView* view,
    /* [in] */ Int32 position,
    /* [in] */ Int64 id)
{
    if (mListener != NULL) {
        Boolean res;
        mListener->OnNavigationItemSelected(position, id, &res);
    }
    return NOERROR;
}

ECode  NavItemSelectedListener::OnNothingSelected(
    /* [in] */ IAdapterView* parent)
{
    // Do nothing
    return NOERROR;
}

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos
