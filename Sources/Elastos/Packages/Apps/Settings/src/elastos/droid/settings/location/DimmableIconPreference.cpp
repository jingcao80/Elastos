
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