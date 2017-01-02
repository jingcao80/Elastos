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

#ifndef __ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSPREFERENCE_H__

#include "_Elastos_Droid_Settings_Applications_CProcessStatsPreference.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::Drawable::IColorDrawable;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Preference::IPreference;
using Elastos::Droid::Text::Format::IFormatter;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IProgressBar;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Settings::Utils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Applications {

CarClass(CProcessStatsPreference)
    , public Preference
{
public:
    CAR_OBJECT_DECL()

    CProcessStatsPreference();

    ~CProcessStatsPreference();

    CARAPI constructor(
        /* [in] */ IContext* context);
    {
        return constructor(context, NULL);
    }

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);
    {
        return constructor(context, attrs, 0);
    }

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);
    {
        return constructor(context, attrs, defStyleAttr, 0);
    }

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);
    {
        Preference::constructor(context, attrs, defStyleAttr, defStyleRes);
        SetLayoutResource(R::layout::preference_app_percentage);
        return NOERROR;
    }

    CARAPI Init(
        /* [in] */ IDrawable* _icon,
        /* [in] */ ProcStatsEntry* entry)
    {
        mEntry = entry;
        AutoPtr<IDrawable> icon = _icon;
        if (icon == NULL) {
            CColorDrawable::New(0, (IDrawable**)&icon);
        }
        SetIcon(icon);
        return NOERROR;
    }

    public AutoPtr<ProcStatsEntry> GetEntry()
    {
        return mEntry;
    }

    CARAPI SetPercent(
        /* [in] */ Double percentOfWeight,
        /* [in] */ Double percentOfTime)
    {
        mProgress = (Int32) Math::Ceil(percentOfWeight);
        mProgressText = Utils::FormatPercentage((Int32) percentOfTime);
        NotifyChanged();
        return NOERROR;
    }

    //@Override
    protected CARAPI OnBindView(
        /* [in] */ IView* view)
    {
        Preference::OnBindView(view);

        AutoPtr<IView> progressTmp;
        view->FindViewById(Elastos::Droid::R::id::progress, (IView**)&progressTmp);
        IProgressBar* progress = IProgressBar::Probe(progressTmp);
        progress->SetProgress(mProgress);

        AutoPtr<IView> text1Tmp;
        view->FindViewById(Elastos::Droid::R::id::text1, (IView**)&text1Tmp);
        ITextView* text1 = ITextView::Probe(text1Tmp);
        text1->SetText(mProgressText);
        return NOERROR;
    }

private:
    AutoPtr<ProcStatsEntry> mEntry;
    Int32 mProgress;
    AutoPtr<ICharSequence> mProgressText;
};

} // namespace Applications
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_APPLICATIONS_CPROCESSSTATSPREFERENCE_H__