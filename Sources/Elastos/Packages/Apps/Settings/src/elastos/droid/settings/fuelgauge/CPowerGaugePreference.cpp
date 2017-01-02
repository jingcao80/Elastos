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

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/settings/fuelgauge/CPowerGaugePreference.h"
#include "elastos/droid/settings/Utils.h"
#include "../R.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Graphics::Drawable::CColorDrawable;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Fuelgauge {

CAR_INTERFACE_IMPL(CPowerGaugePreference, Preference, IPowerGaugePreference)

CAR_OBJECT_IMPL(CPowerGaugePreference)

CPowerGaugePreference::CPowerGaugePreference()
    : mProgress(0)
{}

ECode CPowerGaugePreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IDrawable* icon,
    /* [in] */ ICharSequence* contentDescription,
    /* [in] */ IBatteryEntry* info)
{
    Preference::constructor(context);
    SetLayoutResource(R::layout::preference_app_percentage);
    AutoPtr<IDrawable> _icon;
    if (icon != NULL) {
        _icon = icon;
    }
    else {
        CColorDrawable::New(0, (IDrawable**)&_icon);
    }
    SetIcon(_icon);
    mInfo = (BatteryEntry*)info;
    mContentDescription = contentDescription;
    return NOERROR;
}

ECode CPowerGaugePreference::SetPercent(
    /* [in] */ Double percentOfMax,
    /* [in] */ Double percentOfTotal)
{
    mProgress = (Int32) Elastos::Core::Math::Ceil(percentOfMax);
    mProgressText = CoreUtils::Convert(Utils::FormatPercentage((Int32) (percentOfTotal + 0.5)));
    NotifyChanged();
    return NOERROR;
}

AutoPtr<BatteryEntry> CPowerGaugePreference::GetInfo()
{
    return mInfo;
}

ECode CPowerGaugePreference::OnBindView(
    /* [in] */ IView* view)
{
    Preference::OnBindView(view);

    AutoPtr<IView> tmp;
    view->FindViewById(Elastos::Droid::R::id::progress, (IView**)&tmp);
    IProgressBar* progress = IProgressBar::Probe(tmp);
    progress->SetProgress(mProgress);

    tmp = NULL;
    view->FindViewById(Elastos::Droid::R::id::text1, (IView**)&tmp);
    ITextView* text1 = ITextView::Probe(tmp);
    text1->SetText(mProgressText);

    if (mContentDescription != NULL) {
        AutoPtr<IView> titleView;
        view->FindViewById(Elastos::Droid::R::id::title, (IView**)&titleView);
        titleView->SetContentDescription(mContentDescription);
    }
    return NOERROR;
}

} // namespace Fuelgauge
} // namespace Settings
} // namespace Droid
} // namespace Elastos