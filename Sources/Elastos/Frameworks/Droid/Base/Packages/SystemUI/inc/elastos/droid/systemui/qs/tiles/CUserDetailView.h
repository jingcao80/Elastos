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

#ifndef __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILVIEW_H__
#define __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILVIEW_H__

#include "_Elastos_Droid_SystemUI_Qs_Tiles_CUserDetailView.h"
#include "elastos/droid/systemui/qs/PseudoGridView.h"
#include "elastos/droid/systemui/statusbar/policy/UserSwitcherController.h"

using Elastos::Droid::SystemUI::StatusBar::Policy::IUserSwitcherController;
using Elastos::Droid::SystemUI::StatusBar::Policy::UserSwitcherController;
using Elastos::Droid::View::IViewOnClickListener;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

/**
 * Quick settings detail view for user switching.
 */
CarClass(CUserDetailView)
    , public PseudoGridView
    , public IUserDetailView
{
public:
    class Adapter
        : public UserSwitcherController::BaseUserAdapter
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        Adapter(
            /* [in] */ IContext* context,
            /* [in] */ IUserSwitcherController* controller);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);

        // @Override
        CARAPI OnClick(
            /* [in] */ IView* view);

    private:
        IContext* mContext;
    };

public:
    CAR_OBJECT_DECL()

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    static CARAPI_(AutoPtr<IUserDetailView>) Inflate(
        /* [in] */ IContext* context,
        /* [in] */ IViewGroup* parent,
        /* [in] */ Boolean attach);

    CARAPI CreateAndSetAdapter(
        /* [in] */ IUserSwitcherController* controller);

private:
    AutoPtr<Adapter> mAdapter;
};

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_QS_TILES_CUSERDETAILVIEW_H__
