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

#ifndef _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_
#define _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_

#include "_Elastos_Droid_View_CViewAnimationUtilsHelper.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Animation::IAnimator;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewAnimationUtilsHelper;

namespace Elastos {
namespace Droid {
namespace View {

CarClass(CViewAnimationUtilsHelper)
    , public Singleton
    , public IViewAnimationUtilsHelper
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    CARAPI CreateCircularReveal(
    	/* [in] */ IView* view,
        /* [in] */ Int32 centerX,
        /* [in] */ Int32 centerY,
        /* [in] */ Float startRadius,
        /* [in] */ Float endRadius,
        /* [out] */ IAnimator** result);
};

} // namespace View
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_VIEW_CVIEWANIMATIONUTILSHELPER_H_

