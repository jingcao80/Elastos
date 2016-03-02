
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CBrightnessSettings.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/provider/CSettingsSystem.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettingsSystem;
using Elastos::Droid::Provider::CSettingsSystem;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CBrightnessSettings, Object, IBrightnessSettings)

CAR_OBJECT_IMPL(CBrightnessSettings)

ECode CBrightnessSettings::constructor()
{
    return constructor(0, false);
}

ECode CBrightnessSettings::constructor(
    /* [in] */ Int32 value,
    /* [in] */ Boolean override)
{
    mValue = value;
    mOverride = override;
    mDirty = FALSE;
    return NOERROR;
}

ECode CBrightnessSettings::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CBrightnessSettings::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    mDirty = TRUE;
    return NOERROR;
}

ECode CBrightnessSettings::SetOverride(
    /* [in] */ Boolean override)
{
    mOverride = override;
    mDirty = TRUE;
    return NOERROR;
}

ECode CBrightnessSettings::IsOverride(
    /* [out] */ Boolean* override)
{
    VALIDATE_NOT_NULL(override);
    *override = mOverride;
    return NOERROR;
}

ECode CBrightnessSettings::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

ECode CBrightnessSettings::ProcessOverride(
    /* [in] */ IContext* context)
{
    Boolean override;
    if (IsOverride(&override), override) {
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsSystem> ss;
        CSettingsSystem::AcquireSingleton((ISettingsSystem**)&ss);
        Int32 value;
        ss->GetInt32(resolver, ISettingsSystem::SCREEN_BRIGHTNESS_MODE,
                ISettingsSystem::SCREEN_BRIGHTNESS_MODE_MANUAL, &value);
        Boolean automatic = value == ISettingsSystem::SCREEN_BRIGHTNESS_MODE_AUTOMATIC;
        if (automatic) {
            Float current;
            ss->GetFloat(resolver, ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, -2.0, &current);
            // Convert from [0, 255] to [-1, 1] for SCREEN_AUTO_BRIGHTNESS_ADJ
            Float adj = mValue / (255 / 2.0) - 1;
            if (current != adj) {
                Boolean result;
                ss->PutFloat(resolver, ISettingsSystem::SCREEN_AUTO_BRIGHTNESS_ADJ, adj, &result);
            }
        }
        else {
            Int32 current;
            ss->GetInt32(resolver, ISettingsSystem::SCREEN_BRIGHTNESS, -1, &current);
            if (current != mValue) {
                Boolean result;
                ss->PutInt32(resolver, ISettingsSystem::SCREEN_BRIGHTNESS, mValue, &result);
            }
        }
    }
    return NOERROR;
}

ECode CBrightnessSettings::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IBrightnessSettings** obj)
{
    VALIDATE_NOT_NULL(obj);

    Int32 event;
    xpp->Next(&event);
    AutoPtr<CBrightnessSettings> brightnessDescriptor;
    CBrightnessSettings::NewByFriend((CBrightnessSettings**)&brightnessDescriptor);
    String name;
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("brightnessDescriptor"))) {
        if (event == IXmlPullParser::START_TAG) {
            if (name.Equals("value")) {
                String text;
                xpp->NextText(&text);
                brightnessDescriptor->mValue = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("override")) {
                String text;
                xpp->NextText(&text);
                brightnessDescriptor->mOverride = StringUtils::ParseBoolean(text);
            }
        }
        xpp->Next(&event);
    }
    *obj = (IBrightnessSettings*)brightnessDescriptor.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CBrightnessSettings::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<brightnessDescriptor>\n<value>"));
    builder->Append(mValue);
    builder->Append(String("</value>\n<override>"));
    builder->Append(mOverride);
    builder->Append(String("</override>\n</brightnessDescriptor>\n"));
    return NOERROR;
}

ECode CBrightnessSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mOverride);
    source->ReadInt32(&mValue);
    source->ReadBoolean(&mDirty);
    return NOERROR;
}

ECode CBrightnessSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteBoolean(mOverride);
    dest->WriteInt32(mValue);
    dest->WriteBoolean(mDirty);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
