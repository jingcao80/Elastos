
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/app/CStreamSettings.h"

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL(CStreamSettings, Object, IStreamSettings, IParcelable)

CAR_OBJECT_IMPL(CStreamSettings)

ECode CStreamSettings::constructor(
    /* [in] */ Int32 streamId)
{
    return constructor(streamId, 0, FALSE);
}

ECode CStreamSettings::constructor(
    /* [in] */ Int32 streamId,
    /* [in] */ Int32 value,
    /* [in] */ Boolean override)
{
    mStreamId = streamId;
    mValue = value;
    mOverride = override;
    mDirty = FALSE;
    return NOERROR;
}

ECode CStreamSettings::GetStreamId(
    /* [out] */ Int32* streamId)
{
    VALIDATE_NOT_NULL(streamId);
    *streamId = mStreamId;
    return NOERROR;
}

ECode CStreamSettings::GetValue(
    /* [out] */ Int32* value)
{
    VALIDATE_NOT_NULL(value);
    *value = mValue;
    return NOERROR;
}

ECode CStreamSettings::SetValue(
    /* [in] */ Int32 value)
{
    mValue = value;
    mDirty = TRUE;
    return NOERROR;
}

ECode CStreamSettings::SetOverride(
    /* [in] */ Boolean override)
{
    mOverride = override;
    mDirty = TRUE;
    return NOERROR;
}

ECode CStreamSettings::IsOverride(
    /* [out] */ Boolean* override)
{
    VALIDATE_NOT_NULL(override);
    *override = mOverride;
    return NOERROR;
}

ECode CStreamSettings::IsDirty(
    /* [out] */ Boolean* dirty)
{
    VALIDATE_NOT_NULL(dirty);
    *dirty = mDirty;
    return NOERROR;
}

ECode CStreamSettings::FromXml(
    /* [in] */ IXmlPullParser* xpp,
    /* [in] */ IContext* context,
    /* [out] */ IStreamSettings** obj)
{
    VALIDATE_NOT_NULL(obj);
    Int32 event;
    xpp->Next(&event);
    AutoPtr<CStreamSettings> streamDescriptor;
    CStreamSettings::NewByFriend(0, (CStreamSettings**)&streamDescriptor);
    String name;
    while (event != IXmlPullParser::END_TAG || (xpp->GetName(&name), !name.Equals("streamDescriptor"))) {
        if (event == IXmlPullParser::START_TAG) {
            xpp->GetName(&name);
            if (name.Equals("streamId")) {
                String text;
                xpp->NextText(&text);
                streamDescriptor->mStreamId = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("value")) {
                String text;
                xpp->NextText(&text);
                streamDescriptor->mValue = StringUtils::ParseInt32(text);
            }
            else if (name.Equals("override")) {
                String text;
                xpp->NextText(&text);
                streamDescriptor->mOverride = StringUtils::ParseInt32(text);
            }
        }
        xpp->Next(&event);
    }
    *obj = (IStreamSettings*)streamDescriptor.Get();
    REFCOUNT_ADD(*obj);
    return NOERROR;
}

ECode CStreamSettings::GetXmlString(
    /* [in] */ IStringBuilder* builder,
    /* [in] */ IContext* context)
{
    builder->Append(String("<streamDescriptor>\n<streamId>"));
    builder->Append(mStreamId);
    builder->Append(String("</streamId>\n<value>"));
    builder->Append(mValue);
    builder->Append(String("</value>\n<override>"));
    builder->Append(mOverride);
    builder->Append(String("</override>\n</streamDescriptor>\n"));
    mDirty = FALSE;
    return NOERROR;
}

ECode CStreamSettings::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInt32(&mStreamId);
    source->ReadBoolean(&mOverride);
    source->ReadInt32(&mValue);
    source->ReadBoolean(&mDirty);
    return NOERROR;
}

ECode CStreamSettings::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32(mStreamId);
    dest->WriteBoolean(mOverride);
    dest->WriteInt32(mValue);
    dest->WriteBoolean(mDirty);
    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
