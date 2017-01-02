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

#include "elastos/droid/settings/location/DimmableIconPreference.h"

using Elastos::Droid::Graphics::Drawable::IDrawable;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Location {

const Int32 DimmableIconPreference::ICON_ALPHA_ENABLED = 255;
const Int32 DimmableIconPreference::ICON_ALPHA_DISABLED = 102;

DimmableIconPreference::DimmableIconPreference()
{}

DimmableIconPreference::~DimmableIconPreference()
{}

ECode DimmableIconPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return Preference::constructor(context, attrs, defStyle);
}

ECode DimmableIconPreference::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return Preference::constructor(context, attrs);
}

ECode DimmableIconPreference::constructor(
    /* [in] */ IContext* context)
{
    return Preference::constructor(context);
}

void DimmableIconPreference::DimIcon(
    /* [in] */ Boolean dimmed)
{
    AutoPtr<IDrawable> icon;
    GetIcon((IDrawable**)&icon);
    if (icon != NULL) {
        icon->Mutate();
        icon->SetAlpha(dimmed ? ICON_ALPHA_DISABLED : ICON_ALPHA_ENABLED);
        SetIcon(icon);
    }
}

ECode DimmableIconPreference::OnParentChanged(
    /* [in] */ IPreference* parent,
    /* [in] */ Boolean disableChild)
{
    DimIcon(disableChild);
    return Preference::OnParentChanged(parent, disableChild);
}

ECode DimmableIconPreference::SetEnabled(
    /* [in] */ Boolean enabled)
{
    DimIcon(!enabled);
    return Preference::SetEnabled(enabled);
}

} // namespace Location
} // namespace Settings
} // namespace Droid
} // namespace Elastos