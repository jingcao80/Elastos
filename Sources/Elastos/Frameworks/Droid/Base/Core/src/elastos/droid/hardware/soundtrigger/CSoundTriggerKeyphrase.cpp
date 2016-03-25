
#include "elastos/droid/hardware/soundtrigger/CSoundTriggerKeyphrase.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/Arrays.h>

using Elastos::Core::StringBuilder;
using Elastos::Utility::Arrays;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Soundtrigger {

CAR_INTERFACE_IMPL_2(CSoundTriggerKeyphrase, Object, ISoundTriggerKeyphrase, IParcelable)

CAR_OBJECT_IMPL(CSoundTriggerKeyphrase)

CSoundTriggerKeyphrase::CSoundTriggerKeyphrase()
    : mId(0)
    , mRecognitionModes(0)
    , mLocale(NULL)
    , mText(NULL)
    , mUsers(NULL)
{
}

ECode CSoundTriggerKeyphrase::constructor()
{
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::constructor(
    /* [in] */ Int32 id,
    /* [in] */ Int32 recognitionModes,
    /* [in] */ const String& locale,
    /* [in] */ const String& text,
    /* [in] */ ArrayOf<Int32>* users)
{
    mId = id;
    mRecognitionModes = recognitionModes;
    mLocale = locale;
    mText = text;
    mUsers = users;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::FromParcel(
    /* [in] */ IParcel* source,
    /* [out] */ ISoundTriggerKeyphrase** result)
{
    VALIDATE_NOT_NULL(result);

    Int32 id;
    FAIL_RETURN(source->ReadInt32(&id))
    Int32 recognitionModes;
    FAIL_RETURN(source->ReadInt32(&recognitionModes))
    String locale;
    FAIL_RETURN(source->ReadString(&locale))
    String text;
    FAIL_RETURN(source->ReadString(&text))
    AutoPtr<ArrayOf<Int32> > users;
    Int32 numUsers;
    FAIL_RETURN(source->ReadInt32(&numUsers))
    if (numUsers >= 0) {
        users = ArrayOf<Int32>::Alloc(numUsers);
        FAIL_RETURN(source->ReadArrayOf((Handle32*)&users))
    }

    AutoPtr<CSoundTriggerKeyphrase> newResult;
    CSoundTriggerKeyphrase::NewByFriend(id, recognitionModes, locale, text,
            users, (CSoundTriggerKeyphrase**)&newResult);
    *result = newResult;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    FAIL_RETURN(dest->WriteInt32(mId))
    FAIL_RETURN(dest->WriteInt32(mRecognitionModes))
    FAIL_RETURN(dest->WriteString(mLocale))
    FAIL_RETURN(dest->WriteString(mText))
    if (mUsers != NULL) {
        FAIL_RETURN(dest->WriteInt32(mUsers->GetLength()))
        FAIL_RETURN(dest->WriteArrayOf((Handle32)mUsers.Get()))
    } else {
        FAIL_RETURN(dest->WriteInt32(-1))
    }
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    FAIL_RETURN(source->ReadInt32(&mId))
    FAIL_RETURN(source->ReadInt32(&mRecognitionModes))
    FAIL_RETURN(source->ReadString(&mLocale))
    FAIL_RETURN(source->ReadString(&mText))

    AutoPtr<ArrayOf<Int32> > users;
    Int32 numUsers;
    FAIL_RETURN(source->ReadInt32(&numUsers))
    if (numUsers >= 0) {
        users = ArrayOf<Int32>::Alloc(numUsers);
        FAIL_RETURN(source->ReadArrayOf((Handle32*)&users))
    }
    mUsers = users;

    return NOERROR;
}

ECode CSoundTriggerKeyphrase::Equals(
    /* [in] */ IInterface *obj,
    /* [out] */ Boolean *equal)
{
    VALIDATE_NOT_NULL(equal);

    if (TO_IINTERFACE(this) == TO_IINTERFACE(obj)) {
        *equal = TRUE;
        return NOERROR;
    }
    if (obj == NULL) {
        *equal = FALSE;
        return NOERROR;
    }

    ClassID clsid;
    IObject::Probe(obj)->GetClassID(&clsid);
    if (ECLSID_CSoundTriggerKeyphrase != clsid) {
        *equal = FALSE;
        return NOERROR;
    }

    const AutoPtr<CSoundTriggerKeyphrase> other = (CSoundTriggerKeyphrase*)ISoundTriggerKeyphrase::Probe(obj);
    if (mText.IsNull()) {
        if (!(other->mText).IsNull()) {
            *equal = FALSE;
            return NOERROR;
        }
    }
    else if (!mText.Equals(other->mText)) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mId != other->mId) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mLocale.IsNull()) {
        if (!(other->mLocale).IsNull()) {
            *equal = FALSE;
            return NOERROR;
        }
    }
    else if (!mLocale.Equals(other->mLocale)) {
        *equal = FALSE;
        return NOERROR;
    }
    if (mRecognitionModes != other->mRecognitionModes) {
        *equal = FALSE;
        return NOERROR;
    }
    if (!Arrays::Equals(mUsers, other->mUsers)) {
        *equal = FALSE;
        return NOERROR;
    }
    *equal = TRUE;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    VALIDATE_NOT_NULL(hashCode);

    Int32 prime = 31;
    Int32 result = 1;
    if (!mText.IsNull()) {
        Int32 testHashCode = mText.GetHashCode();
        result = prime * result + testHashCode;
    }
    result = prime * result + mId;
    if (!mLocale.IsNull()) {
        Int32 localeHashCode = mLocale.GetHashCode();
        result = prime * result + localeHashCode;
    }
    result = prime * result + mRecognitionModes;
    Int32 usersHashCode = Arrays::GetHashCode(mUsers);
    result = prime * result + usersHashCode;
    *hashCode = result;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);

    StringBuilder sb;
    sb += "Keyphrase [id=";
    sb += mId;
    sb += ", recognitionModes=";
    sb += mRecognitionModes;
    sb += ", locale=";
    sb += mLocale;
    sb += ", text=";
    sb += mText;
    sb += ", users=";
    sb += Arrays::ToString(mUsers);
    sb += "]";

    return sb.ToString(str);
}

ECode CSoundTriggerKeyphrase::GetId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);

    *id = mId;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::GetRecognitionModes(
    /* [out] */ Int32* modes)
{
    VALIDATE_NOT_NULL(modes);

    *modes = mRecognitionModes;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::GetLocale(
    /* [out] */ String* locale)
{
    VALIDATE_NOT_NULL(locale);

    *locale = mLocale;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::GetText(
    /* [out] */ String* text)
{
    VALIDATE_NOT_NULL(text);

    *text = mText;
    return NOERROR;
}

ECode CSoundTriggerKeyphrase::GetUsers(
    /* [out, callee] */ ArrayOf<Int32>** users)
{
    VALIDATE_NOT_NULL(users);

    *users = mUsers;
    REFCOUNT_ADD(*users);
    return NOERROR;
}

} //Soundtrigger
} //Hardware
} //Droid
} //Elastos
