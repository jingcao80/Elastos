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

#ifndef  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__
#define  __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__

#include "_Elastos.Droid.Launcher2.h"
#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewGroupOnHierarchyChangeListener;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class HideFromAccessibilityHelper
    : public Object
    , public IViewGroupOnHierarchyChangeListener
{
public:
    CAR_INTERFACE_DECL();

    HideFromAccessibilityHelper();

    CARAPI SetImportantForAccessibilityToNo(
        /* [in] */ IView* v,
        /* [in] */ Boolean onlyAllApps);

    CARAPI RestoreImportantForAccessibility(
        /* [in] */ IView* v);

    CARAPI OnChildViewAdded(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

    CARAPI OnChildViewRemoved(
        /* [in] */ IView* parent,
        /* [in] */ IView* child);

private:
    CARAPI_(void) SetImportantForAccessibilityToNoHelper(
        /* [in] */ IView* v);

    CARAPI_(void) RestoreImportantForAccessibilityHelper(
        /* [in] */ IView* v);

    CARAPI_(Boolean) IncludeView(
        /* [in] */ IView* v);

    CARAPI_(Boolean) HasAncestorOfType(
        /* [in] */ IView* v,
        /* [in] */ ClassID c);

protected:
    Boolean mHide;
    Boolean mOnlyAllApps;

private:
    AutoPtr<IHashMap> mPreviousValues;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_HIDEFROMACCESSIBILITYHELPER_H__