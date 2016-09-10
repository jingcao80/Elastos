
#include "elastos/droid/settings/PinnedHeaderListFragment.h"

using Elastos::Droid::Widget::IListView;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;

namespace Elastos {
namespace Droid {
namespace Settings {

PinnedHeaderListFragment::PinnedHeaderListFragment()
{}

PinnedHeaderListFragment::~PinnedHeaderListFragment()
{}

ECode PinnedHeaderListFragment::constructor()
{
    return ListFragment::constructor();
}

ECode PinnedHeaderListFragment::SetPinnedHeaderView(
    /* [in] */ IView* pinnedHeaderView)
{
    AutoPtr<IListView> listview;
    GetListView((IListView**)&listview);

    AutoPtr<IViewParent> parent;
    IView::Probe(listview)->GetParent((IViewParent**)&parent);

    return IViewGroup::Probe(parent)->AddView(pinnedHeaderView, 0);
}

ECode PinnedHeaderListFragment::ClearPinnedHeaderView()
{
    AutoPtr<IListView> listview;
    GetListView((IListView**)&listview);

    AutoPtr<IViewParent> parent;
    IView::Probe(listview)->GetParent((IViewParent**)&parent);

    return IViewGroup::Probe(parent)->RemoveViewAt(0);
}

} // namespace Settings
} // namespace Droid
} // namespace Elastos
