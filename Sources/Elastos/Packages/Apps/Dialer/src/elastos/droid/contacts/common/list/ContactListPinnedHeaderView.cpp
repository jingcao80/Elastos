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

#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.View.h"
#include "elastos/droid/contacts/common/list/ContactListPinnedHeaderView.h"
#include "elastos/droid/contacts/common/util/ViewUtil.h"
#include "elastos/droid/text/TextUtils.h"
#include "R.h"

using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Contacts::Common::List::EIID_IContactListPinnedHeaderView;
using Elastos::Droid::Contacts::Common::Util::ViewUtil;
using Elastos::Droid::Graphics::IColor;
using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::View::IViewGroupLayoutParams;
using Elastos::Droid::View::CViewGroupLayoutParams;
using Elastos::Droid::View::IGravity;
using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace List {

ContactListPinnedHeaderView::ContactListPinnedHeaderView(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ IView* parent)
{
    TextView::constructor(context, attrs);

    AutoPtr<IContext> ctx;
    GetContext((IContext**)&ctx);
    AutoPtr<ArrayOf<Int32> > params = TO_ATTRS_ARRAYOF(Elastos::Droid::Dialer::R::styleable::ContactListItemView);
    AutoPtr<ITypedArray> a;
    ctx->ObtainStyledAttributes(attrs, params, (ITypedArray**)&a);
    Int32 backgroundColor;
    a->GetColor(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_background_color,
            IColor::WHITE, &backgroundColor);
    Int32 textOffsetTop;
    a->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_text_offset_top,
            0, &textOffsetTop);
    Int32 paddingStartOffset;
    a->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::styleable::ContactListItemView_list_item_padding_left,
            0, &paddingStartOffset);
    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    Int32 textWidth;
    res->GetDimensionPixelSize(
            Elastos::Droid::Dialer::R::dimen::contact_list_section_header_width, &textWidth);
    Int32 widthIncludingPadding = paddingStartOffset + textWidth;
    a->Recycle();

    SetBackgroundColor(backgroundColor);
    SetTextAppearance(ctx, Elastos::Droid::Dialer::R::style::SectionHeaderStyle);
    AutoPtr<IViewGroupLayoutParams> lp;
    CViewGroupLayoutParams::New(widthIncludingPadding, IViewGroupLayoutParams::WRAP_CONTENT,
            (IViewGroupLayoutParams**)&lp);
    SetLayoutParams(lp);
    Int32 direction;
    parent->GetLayoutDirection(&direction);
    SetLayoutDirection(direction);
    SetGravity(IGravity::CENTER_VERTICAL |
            (ViewUtil::IsViewLayoutRtl(IView::Probe(this)) ? IGravity::RIGHT : IGravity::LEFT));

    // Apply text top offset. Multiply by two, because we are implementing this by padding for a
    // vertically centered view, rather than adjusting the position directly via a layout.
    Int32 start, top, end, bottom;
    GetPaddingStart(&start);
    GetPaddingTop(&top);
    GetPaddingEnd(&end);
    GetPaddingBottom(&bottom);
    SetPaddingRelative(
            start + paddingStartOffset,
            top + (textOffsetTop * 2),
            end,
            bottom);
}

CAR_INTERFACE_IMPL(ContactListPinnedHeaderView, TextView, IContactListPinnedHeaderView)

void ContactListPinnedHeaderView::SetSectionHeaderTitle(
    /* [in] */ const String& title)
{
    if (!TextUtils::IsEmpty(title)) {
        AutoPtr<ICharSequence> cs;
        CString::New(title, (ICharSequence**)&cs);
        SetText(cs);
        SetVisibility(IView::VISIBLE);
    }
    else {
        SetVisibility(IView::GONE);
    }
}

} // List
} // Common
} // Contacts
} // Droid
} // Elastos
