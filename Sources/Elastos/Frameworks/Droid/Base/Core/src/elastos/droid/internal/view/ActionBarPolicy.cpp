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

#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/internal/view/ActionBarPolicy.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/R.h"

using Elastos::Droid::Content::Res::ITypedArray;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Pm::IApplicationInfo;
using Elastos::Droid::Os::Build;
using Elastos::Droid::Utility::IDisplayMetrics;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace View {

CAR_INTERFACE_IMPL(ActionBarPolicy, Object, IActionBarPolicy)

ActionBarPolicy::ActionBarPolicy(
    /* [in] */ IContext* ctx)
    : mContext(ctx)
{
}

AutoPtr<IActionBarPolicy> ActionBarPolicy::Get(
    /* [in] */ IContext* context)
{
    return new ActionBarPolicy(context);
}

ECode ActionBarPolicy::GetMaxActionButtons(
        /* [out] */ Int32* rst)
{
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);
    return rs->GetInteger(R::integer::max_action_buttons, rst);
}

ECode ActionBarPolicy::ShowsOverflowMenuButton(
        /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    *rst = TRUE;
    return NOERROR;
}

ECode ActionBarPolicy::GetEmbeddedMenuWidthLimit(
        /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);
    AutoPtr<IDisplayMetrics> dm;
    rs->GetDisplayMetrics((IDisplayMetrics**)&dm);
    Int32 widthPixels;
    dm->GetWidthPixels(&widthPixels);
    *rst = widthPixels/2;
    return NOERROR;
}

ECode ActionBarPolicy::HasEmbeddedTabs(
        /* [out] */ Boolean* rst)
{
    VALIDATE_NOT_NULL(rst)
    AutoPtr<IApplicationInfo> appInfo;
    mContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 targetSdk;
    appInfo->GetTargetSdkVersion(&targetSdk);
    AutoPtr<IResources> rs;
    mContext->GetResources((IResources**)&rs);
    if(targetSdk >= Build::VERSION_CODES::JELLY_BEAN) {
        return rs->GetBoolean(R::bool_::action_bar_embed_tabs, rst);
    }

    // The embedded tabs policy changed in Jellybean; give older apps the old policy
    // so they get what they expect.
    return rs->GetBoolean(R::bool_::action_bar_embed_tabs_pre_jb, rst);
}

ECode ActionBarPolicy::GetTabContainerHeight(
        /* [out] */ Int32* rst)
{
    VALIDATE_NOT_NULL(rst)
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::ActionBar);
    AutoPtr<ITypedArray> a;
    mContext->ObtainStyledAttributes(NULL, attrIds, R::attr::actionBarStyle, 0, (ITypedArray**)&a);

    Int32 height;
    a->GetLayoutDimension(R::styleable::ActionBar_height, 0, &height);
    Boolean isHas;
    if(HasEmbeddedTabs(&isHas), !isHas) {
        AutoPtr<IResources> r;
        mContext->GetResources((IResources**)&r);
        Int32 pixelSize;
        r->GetDimensionPixelSize(R::dimen::action_bar_stacked_max_height, &pixelSize);
        height = (height > pixelSize ? pixelSize : height);
    }
    a->Recycle();
    *rst = height;
    return NOERROR;
}

ECode ActionBarPolicy::EnableHomeButtonByDefault(
        /* [out] */ Boolean* rst)
{
    AutoPtr<IApplicationInfo> appInfo;
    mContext->GetApplicationInfo((IApplicationInfo**)&appInfo);
    Int32 targetSdk;
    appInfo->GetTargetSdkVersion(&targetSdk);
    *rst = (targetSdk < Build::VERSION_CODES::ICE_CREAM_SANDWICH);
    return NOERROR;
}

ECode ActionBarPolicy::GetStackedTabMaxWidth(
        /* [out] */ Int32* rst)
{
    AutoPtr<IResources> r;
    mContext->GetResources((IResources**)&r);
    r->GetDimensionPixelSize(R::dimen::action_bar_stacked_tab_max_width, rst);
    return NOERROR;
}

} // namespace View
} // namespace Internal
} // namepsace Droid
} // namespace Elastos
