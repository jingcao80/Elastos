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

#include "elastos/droid/systemui/statusbar/CLatestItemView.h"

using Elastos::Droid::View::EIID_View;
using Elastos::Droid::View::EIID_ViewGroup;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace StatusBar {


IVIEW_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWGROUP_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWPARENT_METHODS_IMPL(CLatestItemView, LatestItemView)
IVIEWMANAGER_METHODS_IMPL(CLatestItemView, LatestItemView)
IDRAWABLECALLBACK_METHODS_IMPL(CLatestItemView, LatestItemView)
IKEYEVENTCALLBACK_METHODS_IMPL(CLatestItemView, LatestItemView)
IACCESSIBILITYEVENTSOURCE_METHODS_IMPL(CLatestItemView, LatestItemView)
IFRAMELAYOUT_METHODS_IMPL(CLatestItemView, LatestItemView)

PInterface CLatestItemView::Probe(
    /* [in] */ REIID riid)
{
    if (riid == EIID_View) {
        return reinterpret_cast<PInterface>((View*)this);
    }
    else if (riid == EIID_ViewGroup) {
        return reinterpret_cast<PInterface>((ViewGroup*)this);
    }
    return _CLatestItemView::Probe(riid);
}

ECode CLatestItemView::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return LatestItemView::Init(context, attrs);
}


}// namespace StatusBar
}// namespace SystemUI
}// namespace Droid
}// namespace Elastos
