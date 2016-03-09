
#include "Elastos.CoreLibrary.External.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CAirplaneModeSettings.h"
#include "elastos/droid/content/CIntent.h"
#include "elastos/droid/provider/CSettingsGlobal.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Provider::ISettingsGlobal;
using Elastos::Droid::Provider::CSettingsGlobal;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CAirplaneModeSettings, Object, IAirplaneModeSettings)

CAR_OBJECT_IMPL(CAirplaneModeSettings)

ECode CAirplaneModeSettings::constructor()
{
    return constructor(0, FALSE);
}

ECode CAirplaneModeSettings::constructor(
    /* [in] */ Int32 value,
    /* [in] */ Boolean override)
{
    mValue = value;
    mOverride = override;
    mDirty = FALSE;
    return NOERROR;
}

ECode CAirplaneModeSettings::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CAirplaneModeSettings::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    mDirty = TRUE;
    return NOERROR;
}

ECode CAirplaneModeSettings::SetOverride(
    /* [in] */ Boolean override)
{
    mOverride = override;
    mDirty = TRUE;
    return NOERROR;
}

ECode CAirplaneModeSettings::IsOverride(
    /* [out] */ Boolean* override)
{
    VALIDATE_NOT_NULL(override);
    *override = mOverride;
    return NOERROR;
}

ECode CAirplaneModeSettings::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

ECode CAirplaneModeSettings::ProcessOverride(
    /* [in] */ IContext* context)
{
    Boolean override;
    if (IsOverride(&override), override) {
        AutoPtr<IContentResolver> resolver;
        context->GetContentResolver((IContentResolver**)&resolver);
        AutoPtr<ISettingsGlobal> sg;
        CSettingsGlobal::AcquireSingleton((ISettingsGlobal**)&sg);
        Int32 current;
        sg->GetInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, 0, &current);
        if (current != mValue) {
            Boolean result;
            sg->PutInt32(resolver, ISettingsGlobal::AIRPLANE_MODE_ON, mValue, &result);
            AutoPtr<IIntent> intent;
            CIntent::New(IIntent::ACTION_AIRPLANE_MODE_CHANGED, (IIntent**)&intent);
            intent->PutExtra(String("state"), mValue == 1);
            context->SendBroadcast(intent);
        }
    }
    return NOERROR;
}

ECode CAirplaneModeSettings::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IAirplaneModeSettings** obj)
{
    VALIDATE_NOT_NULL(obj);

    Int32 event;
    xpp->Next(&event);
    AutoPtr<CAirplaneModeSettings> airplaneModeDescriptor;
    CAirplaneModeSettings::NewByFriend((CAirplaneModeSettings**)&airplaneModeDescriptor);
    String name;
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("airplaneModeDescriptor"))) {
        xpp->GetName(&name);
        if (event == IXmlPullParser::START_TAG) {
            if (name.Equals("value")) {
                String text;
                xpp->NextText(&text);
                airplaneModeDescriptor->mValue = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("override")) {
                String text;
                xpp->NextText(&text);
                airplaneModeDescriptor->mOverride = StringUtils::ParseBoolean(text);
            }
        }
        xpp->Next(&event);
    }
    *obj = (IAirplaneModeSettings*)airplaneModeDescriptor.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CAirplaneModeSettings::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<airplaneModeDescriptor>\n<value>"));
    builder->Append(mValue);
    builder->Append(String("</value>\n<override>"));
    builder->Append(mOverride);
    builder->Append(String("</override>\n</airplaneModeDescriptor>\n"));
    return NOERROR;
}

ECode CAirplaneModeSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mOverride);
    source->ReadInt32(&mValue);
    source->ReadBoolean(&mDirty);
    return NOERROR;
}

ECode CAirplaneModeSettings::WriteToParcel(
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
