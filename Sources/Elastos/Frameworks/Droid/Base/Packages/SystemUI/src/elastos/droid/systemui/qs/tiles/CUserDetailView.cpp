
#include "elastos/droid/systemui/qs/tiles/CUserDetailView.h"
#include "elastos/droid/systemui/qs/tiles/CUserDetailItemView.h"
#include "../../R.h"
#include <elastos/droid/view/LayoutInflater.h>

using Elastos::Droid::SystemUI::StatusBar::Policy::IUserSwitcherControllerUserRecord;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::LayoutInflater;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Qs {
namespace Tiles {

CAR_INTERFACE_IMPL(CUserDetailView::Adapter, BaseUserAdapter, IViewOnClickListener);
CUserDetailView::Adapter::Adapter(
    /* [in] */ IContext* context,
    /* [in] */ IUserSwitcherController* controller)
    : BaseUserAdapter((UserSwitcherController*)controller)
    , mContext(context)
{
}

ECode CUserDetailView::Adapter::GetView(
    /* [in] */ Int32 position,
    /* [in] */ IView* convertView,
    /* [in] */ IViewGroup* parent,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);
    AutoPtr<IInterface> obj;
    GetItem(position, (IInterface**)&obj);
    AutoPtr<IUserSwitcherControllerUserRecord> item = IUserSwitcherControllerUserRecord::Probe(obj);
    AutoPtr<IUserDetailItemView> v = CUserDetailItemView::ConvertOrInflate(
            mContext, convertView, parent);
    if (IView::Probe(v) != convertView) {
        IView::Probe(v)->SetOnClickListener(this);
    }
    String name;
    GetName(mContext, item, &name);
    UserSwitcherController::UserRecord* _item = (UserSwitcherController::UserRecord*)item.Get();
    if (_item->mPicture == NULL) {
        AutoPtr<IDrawable> d;
        GetDrawable(mContext, item, (IDrawable**)&d);
        v->Bind(name, d);
    }
    else {
        v->Bind(name, _item->mPicture);
    }
    IView::Probe(v)->SetActivated(_item->mIsCurrent);
    IView::Probe(v)->SetTag(item);
    *view = IView::Probe(v);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode CUserDetailView::Adapter::OnClick(
    /* [in] */ IView* view)
{
    AutoPtr<IInterface> o;
    view->GetTag((IInterface**)&o);
    AutoPtr<IUserSwitcherControllerUserRecord> tag = IUserSwitcherControllerUserRecord::Probe(o);
    SwitchTo(tag);
    return NOERROR;
}

CAR_OBJECT_IMPL(CUserDetailView);
CAR_INTERFACE_IMPL(CUserDetailView, PseudoGridView, IUserDetailView);
ECode CUserDetailView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return PseudoGridView::constructor(context, attrs);
}

AutoPtr<IUserDetailView> CUserDetailView::Inflate(
    /* [in] */ IContext* context,
    /* [in] */ IViewGroup* parent,
    /* [in] */ Boolean attach)
{
    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(R::layout::qs_user_detail, parent, attach, (IView**)&view);
    return IUserDetailView::Probe(view);
}

ECode CUserDetailView::CreateAndSetAdapter(
    /* [in] */ IUserSwitcherController* controller)
{
    mAdapter = new Adapter(mContext, controller);
    PseudoGridView::ViewGroupAdapterBridge::Link(this, mAdapter);
    return NOERROR;
}

} // namespace Tiles
} // namespace Qs
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos
