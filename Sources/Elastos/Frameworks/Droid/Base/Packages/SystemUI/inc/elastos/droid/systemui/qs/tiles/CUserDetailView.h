
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
        AutoPtr<IContext> mContext;
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
