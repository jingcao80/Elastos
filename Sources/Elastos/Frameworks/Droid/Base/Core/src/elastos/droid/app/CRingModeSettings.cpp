
#include "Elastos.CoreLibrary.External.h"
#include "_Elastos.Droid.Content.h"
#include "_Elastos.Droid.Media.h"
#include "elastos/droid/app/CRingModeSettings.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Core::StringUtils;

namespace Elastos {
namespace Droid {
namespace App {

const String CRingModeSettings::RING_MODE_NORMAL("normal");
const String CRingModeSettings::RING_MODE_VIBRATE("vibrate");
const String CRingModeSettings::RING_MODE_MUTE("mute");

CAR_INTERFACE_IMPL_2(CRingModeSettings, Object, IRingModeSettings, IParcelable)

CAR_OBJECT_IMPL(CRingModeSettings)

CRingModeSettings::CRingModeSettings()
    : mOverride(FALSE)
    , mDirty(FALSE)
{}

ECode CRingModeSettings::constructor()
{
    return constructor(RING_MODE_NORMAL, FALSE);
}

ECode CRingModeSettings::constructor(
    /* [in] */ const String& value,
    /* [in] */ Boolean override)
{
    mValue = value;
    mOverride = override;
    mDirty = FALSE;
    return NOERROR;
}

ECode CRingModeSettings::GetValue(
    /* [out] */ String* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CRingModeSettings::SetValue(
    /* [in] */ const String& value)
{
    mValue = value;
    mDirty = TRUE;
    return NOERROR;
}

ECode CRingModeSettings::SetOverride(
    /* [in] */ Boolean override)
{
    mOverride = override;
    mDirty = TRUE;
    return NOERROR;
}

ECode CRingModeSettings::IsOverride(
    /* [out] */ Boolean* override)
{
    VALIDATE_NOT_NULL(override);
    *override = mOverride;
    return NOERROR;
}

/** @hide */
ECode CRingModeSettings::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

ECode CRingModeSettings::ProcessOverride(
    /* [in] */ IContext* context)
{
    Boolean override;
    if (IsOverride(&override), override) {
        Int32 ringerMode = IAudioManager::RINGER_MODE_NORMAL;
        if (mValue.Equals(RING_MODE_MUTE)) {
            ringerMode = IAudioManager::RINGER_MODE_SILENT;
        }
        else if (mValue.Equals(RING_MODE_VIBRATE)) {
            ringerMode = IAudioManager::RINGER_MODE_VIBRATE;
        }
        AutoPtr<IInterface> obj;
        context->GetSystemService(IContext::AUDIO_SERVICE, (IInterface**)&obj);
        IAudioManager* amgr = IAudioManager::Probe(obj);
        amgr->SetRingerMode(ringerMode);
    }
    return NOERROR;
}

ECode CRingModeSettings::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IRingModeSettings** obj)
{
    VALIDATE_NOT_NULL(obj);
    Int32 event;
    xpp->Next(&event);
    AutoPtr<CRingModeSettings> ringModeDescriptor;
    CRingModeSettings::NewByFriend((CRingModeSettings**)&ringModeDescriptor);
    String name;
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("ringModeDescriptor"))) {
        if (event == IXmlPullParser::START_TAG) {
            xpp->GetName(&name);
            if (name.Equals("value")) {
                String next;
                xpp->NextText(&next);
                ringModeDescriptor->mValue = next;
            }
            else if (name.Equals("override")) {
                String next;
                xpp->NextText(&next);
                ringModeDescriptor->mOverride = StringUtils::ParseBoolean(next);
            }
        }
        xpp->Next(&event);
    }
    *obj = (IRingModeSettings*)ringModeDescriptor.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

/** @hide */
ECode CRingModeSettings::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<ringModeDescriptor>\n<value>"));
    builder->Append(mValue);
    builder->Append(String("</value>\n<override>"));
    builder->Append(mOverride);
    builder->Append(String("</override>\n</ringModeDescriptor>\n"));
    return NOERROR;
}

ECode CRingModeSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteBoolean(mOverride);
    dest->WriteString(mValue);
    dest->WriteBoolean(mDirty);
    return NOERROR;
}

ECode CRingModeSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadBoolean(&mOverride);
    source->ReadString(&mValue);
    source->ReadBoolean(&mDirty);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
