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
