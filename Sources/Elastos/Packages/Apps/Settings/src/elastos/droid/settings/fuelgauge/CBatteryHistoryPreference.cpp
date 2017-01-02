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

#include "elastos/droid/settings/fuelgauge/CBatteryHistoryPreference.h"
#include "../R.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::View::IViewParent;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ITextView;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CAR_INTERFACE_IMPL(CBatteryHistoryPreference, Preference, IBatteryHistoryPreference)

CAR_OBJECT_IMPL(CBatteryHistoryPreference)

CBatteryHistoryPreference::CBatteryHistoryPreference()
    : mHideLabels(FALSE)
{}

ECode CBatteryHistoryPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IBatteryStats* stats,
    /* [in] */ IIntent* batteryBroadcast)
{
    Preference::constructor(context);
    SetLayoutResource(R::layout::preference_batteryhistory);
    mStats = stats;
    mBatteryBroadcast = batteryBroadcast;
    return NOERROR;
}

AutoPtr<IBatteryStats> CBatteryHistoryPreference::GetStats()
{
    return mStats;
}

ECode CBatteryHistoryPreference::SetHideLabels(
    /* [in] */ Boolean hide)
{
    if (mHideLabels != hide) {
        mHideLabels = hide;
        if (mLabelHeader != NULL) {
            mLabelHeader->SetVisibility(hide ? IView::GONE : IView::VISIBLE);
        }
    }
    return NOERROR;
}

ECode CBatteryHistoryPreference::OnBindView(
    /* [in] */ IView* view)
{
    Preference::OnBindView(view);

    AutoPtr<IView> tmp;
    view->FindViewById(R::id::battery_history_chart, (IView**)&tmp);
    CBatteryHistoryChart* chart = (CBatteryHistoryChart*)tmp.Get();
    if (mChart == NULL) {
        // First time: use and initialize this chart.
        chart->SetStats(mStats, mBatteryBroadcast);
        mChart = chart;
    }
    else {
        // All future times: forget the newly inflated chart, re-use the
        // already initialized chart from last time.
        AutoPtr<IViewParent> vp;
        chart->GetParent((IViewParent**)&vp);
        IViewGroup* parent = IViewGroup::Probe(vp);
        Int32 index;
        parent->IndexOfChild(chart, &index);
        parent->RemoveViewAt(index);
        AutoPtr<IViewParent> _vp;
        mChart->GetParent((IViewParent**)&_vp);
        if (_vp != NULL) {
            IViewGroup::Probe(_vp)->RemoveView(mChart);
        }
        parent->AddView(mChart, index);
    }
    view->FindViewById(R::id::labelsHeader, (IView**)&mLabelHeader);
    mLabelHeader->SetVisibility(mHideLabels ? IView::GONE : IView::VISIBLE);
    return NOERROR;
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos