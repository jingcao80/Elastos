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

#ifndef __ELASTOS_DROID_SYSTEMUI_VOLUME_SEGMENTEDBUTTONS_H__
#define __ELASTOS_DROID_SYSTEMUI_VOLUME_SEGMENTEDBUTTONS_H__

#include "_Elastos.Droid.SystemUI.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Utility.h"
#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::SystemUI::Volume::IInteractionCallback;
using Elastos::Droid::SystemUI::Volume::ISegmentedButtonsCallback;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Volume {

class SegmentedButtons
    : public LinearLayout
    , public ISegmentedButtons
{
private:
    class MyClick
        : public Object
        , public IViewOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        MyClick(
            /* [in] */ SegmentedButtons* host);

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        SegmentedButtons* mHost;
    };

    class MyInteractionCallback
        : public Object
        , public IInteractionCallback
    {
    public:
        CAR_INTERFACE_DECL()

        MyInteractionCallback(
            /* [in] */ SegmentedButtons* host);

        //@Override
        CARAPI OnInteraction();

    private:
        SegmentedButtons* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SegmentedButtons();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetCallback(
        /* [in] */ ISegmentedButtonsCallback* callback);

    CARAPI GetSelectedValue(
        /* [out]] */ IInterface** sv);

    CARAPI SetSelectedValue(
        /* [in] */ IInterface* value);

    CARAPI AddButton(
        /* [in] */ Int32 labelResId,
        /* [in] */ IInterface* value);

    CARAPI UpdateLocale();

private:
    CARAPI_(void) FireOnSelected();

    CARAPI_(void) FireInteraction();

private:
    const static AutoPtr<ITypeface> MEDIUM;
    const static AutoPtr<ITypeface> BOLD;
    const static Int32 LABEL_RES_KEY;

    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<ISegmentedButtonsCallback> mCallback;
    AutoPtr<IInterface> mSelectedValue;
    AutoPtr<IViewOnClickListener> mClick;
};

} // namespace Volume
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SYSTEMUI_VOLUME_SEGMENTEDBUTTONS_H__