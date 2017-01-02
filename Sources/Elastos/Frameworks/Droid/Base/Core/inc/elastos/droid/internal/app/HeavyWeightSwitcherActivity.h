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

#ifndef __ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__
#define __ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__

#include "Elastos.Droid.Internal.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Core::ICharSequence;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntentSender;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace App {

class HeavyWeightSwitcherActivity
    : public Activity
    , public IHeavyWeightSwitcherActivity
{
private:
    class SwitchOldListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        SwitchOldListener(
            /* [in] */ HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

    class SwitchNewListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        SwitchNewListener(
            /* [in] */ HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

    class CancelListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        CancelListener(
            HeavyWeightSwitcherActivity* host)
            : mHost(host)
        {}

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        HeavyWeightSwitcherActivity* mHost;
    };

public:
    HeavyWeightSwitcherActivity();

    CAR_INTERFACE_DECL()

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI SetText(
        /* [in] */ Int32 id,
        /* [in] */ ICharSequence* text);

    CARAPI SetDrawable(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* dr);

    CARAPI SetIconAndText(
        /* [in] */ Int32 iconId,
        /* [in] */ Int32 actionId,
        /* [in] */ Int32 descriptionId,
        /* [in] */ const String& packageName,
        /* [in] */ Int32 actionStr,
        /* [in] */ Int32 descriptionStr);

public:
    AutoPtr<IIntentSender> mStartIntent;
    Boolean mHasResult;
    String mCurApp;
    Int32 mCurTask;
    String mNewApp;
    AutoPtr<SwitchOldListener> mSwitchOldListener;
    AutoPtr<SwitchNewListener> mSwitchNewListener;
    AutoPtr<CancelListener> mCancelListener;
};

} //namespace App
} //namespace Internal
} //namespace Droid
} //namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_APP_HEAVYWEIGHTSWITCHERACTIVITY_H__
