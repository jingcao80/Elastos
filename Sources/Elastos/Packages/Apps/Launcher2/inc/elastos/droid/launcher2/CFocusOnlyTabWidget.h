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

#ifndef  __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__
#define  __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__

#include "_Elastos_Droid_Launcher2_CFocusOnlyTabWidget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/widget/TabWidget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::TabWidget;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CarClass(CFocusOnlyTabWidget)
    , public TabWidget
    , public IFocusOnlyTabWidget
{
public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CARAPI GetSelectedTab(
        /* [out] */ IView** tab);

    CARAPI GetChildTabIndex(
        /* [in] */ IView* v,
        /* [out] */ Int32* index);

    CARAPI SetCurrentTabToFocusedTab();

    CARAPI SuperOnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);

    //@Override
    virtual CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_CFOCUSONLYTABWIDGET_H__