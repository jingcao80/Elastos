#include "elastos/droid/media/CMediaDescription.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;

namespace Elastos {
namespace Droid {
namespace Media {

CAR_INTERFACE_IMPL_2(CMediaDescription, Object, IMediaDescription, IParcelable)

CAR_OBJECT_IMPL(CMediaDescription)

CMediaDescription::CMediaDescription()
{
}

CMediaDescription::~CMediaDescription()
{
}

ECode CMediaDescription::constructor()
{
    return NOERROR;
}

ECode CMediaDescription::constructor(
    /* [in] */ const String& mediaId,
    /* [in] */ ICharSequence* title,
    /* [in] */ ICharSequence* subtitle,
    /* [in] */ ICharSequence* description,
    /* [in] */ IBitmap* icon,
    /* [in] */ IUri* iconUri,
    /* [in] */ IBundle* extras)
{
    mMediaId = mediaId;
    mTitle = title;
    mSubtitle = subtitle;
    mDescription = description;
    mIcon = icon;
    mIconUri = iconUri;
    mExtras = extras;
    return NOERROR;
}

ECode CMediaDescription::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadString(&mMediaId);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((Handle32*)&obj);
    mTitle = ICharSequence::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mSubtitle = ICharSequence::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mDescription = ICharSequence::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mIcon = IBitmap::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mIconUri = IUri::Probe(obj);
    obj = NULL;
    source->ReadInterfacePtr((Handle32*)&obj);
    mExtras = IBundle::Probe(obj);
    return NOERROR;
}

ECode CMediaDescription::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteString(mMediaId);
    dest->WriteInterfacePtr(mTitle.Get());
    dest->WriteInterfacePtr(mSubtitle.Get());
    dest->WriteInterfacePtr(mDescription.Get());
    dest->WriteInterfacePtr(mIcon.Get());
    dest->WriteInterfacePtr(mIconUri.Get());
    dest->WriteInterfacePtr(mExtras.Get());
    return NOERROR;
}

ECode CMediaDescription::GetMediaId(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    *result = mMediaId;
    return NOERROR;
}

ECode CMediaDescription::GetTitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mTitle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::GetSubtitle(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mSubtitle;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::GetDescription(
    /* [out] */ ICharSequence** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mDescription;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::GetIconBitmap(
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIcon;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::GetIconUri(
    /* [out] */ IUri** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mIconUri;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::GetExtras(
    /* [out] */ IBundle** result)
{
    VALIDATE_NOT_NULL(result)
    *result = mExtras;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode CMediaDescription::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result)
    String str;
    mTitle->ToString(&str);
    StringBuilder sb(str);
    sb.Append(", ");
    sb.Append(mSubtitle);
    sb.Append(", ");
    sb.Append(mDescription);
    return sb.ToString(result);
}

} // namespace Media
} // namepsace Droid
} // namespace Elastos
