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

#ifndef __ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__

#include "_Elastos.Droid.Dialer.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/widget/LinearLayout.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Widget::LinearLayout;

namespace Elastos{
namespace Droid{
namespace Contacts {
namespace Common {
namespace List {

class ViewPagerTabStrip
    : public LinearLayout
{
public:
    ViewPagerTabStrip();

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    /**
     * Notifies this view that view pager has been scrolled. We save the tab index
     * and selection offset for interpolating the position and width of selection
     * underline.
     */
    CARAPI_(void) OnPageScrolled(
        /* [in] */ Int32 position,
        /* [in] */ Float positionOffset,
        /* [in] */ Int32 positionOffsetPixels);

protected:
    // @Override
    CARAPI_(void) OnDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(Boolean) IsRtl();

private:
    Int32 mSelectedUnderlineThickness;
    AutoPtr<IPaint> mSelectedUnderlinePaint;

    Int32 mIndexForSelection;
    Float mSelectionOffset;
};

} // List
} // Common
} // Contacts
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_LIST_VIEWPAGERTABSTRIP_H__
