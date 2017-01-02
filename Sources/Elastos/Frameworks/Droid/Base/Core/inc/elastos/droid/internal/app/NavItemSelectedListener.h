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

#ifndef __ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__
#define __ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Widget.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IActionBarOnNavigationListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

/**
 * Wrapper to adapt the ActionBar.OnNavigationListener in an AdapterView.OnItemSelectedListener
 * for use in Spinner widgets. Used by action bar implementations.
 */
class NavItemSelectedListener
    : public Object
    , public IAdapterViewOnItemSelectedListener
{
public:
    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IActionBarOnNavigationListener* listener);

    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

private:
    AutoPtr<IActionBarOnNavigationListener> mListener;
};

} // namespace App
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_NAVITEMSELECTEDLISTENER_H__
