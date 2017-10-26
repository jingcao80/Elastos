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

#ifndef __ELASTOS_APPS_PACKAGEINSTALLER_CCAFFEINATEDSCROLLVIEW_H__
#define __ELASTOS_APPS_PACKAGEINSTALLER_CCAFFEINATEDSCROLLVIEW_H__

#include "_Elastos_Droid_PackageInstaller_CCaffeinatedScrollView.h"
#include <elastos/droid/widget/ScrollView.h>

using Elastos::Droid::Widget::ScrollView;

namespace Elastos {
namespace Droid {
namespace PackageInstaller {

/**
  * It's a ScrollView that knows how to stay awake.
  */
CarClass(CCaffeinatedScrollView)
    , public ScrollView
    , public ICaffeinatedScrollView
{
public:
    CCaffeinatedScrollView();

    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
      * Make this visible so we can call it
      */
    // @Override
    CARAPI_(Boolean) AwakenScrollBars();

    CARAPI SetFullScrollAction(
        /* [in] */ IRunnable* action);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI OnScrollChanged(
        /* [in] */ Int32 l,
        /* [in] */ Int32 t,
        /* [in] */ Int32 oldl,
        /* [in] */ Int32 oldt);

private:
    CARAPI_(void) CheckFullScrollAction();

private:
    AutoPtr<IRunnable> mFullScrollAction;
    Int32 mBottomSlop;
};

} // namespace PackageInstaller
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_APPS_PACKAGEINSTALLER_CCAFFEINATEDSCROLLVIEW_H__

