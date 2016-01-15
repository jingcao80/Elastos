
#include "Elastos.Droid.Content.h"
#include "elastos/droid/net/CUriBuilder.h"
#include "elastos/droid/service/notification/Condition.h"
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/Objects.h>

using Elastos::Droid::Net::CUriBuilder;
using Elastos::Core::CString;
using Elastos::Core::IString;
using Elastos::Core::StringBuilder;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::Objects;

namespace Elastos {
namespace Droid {
namespace Service {
namespace Notification {

const String Condition::TAG("Condition");

CAR_INTERFACE_IMPL_2(Condition, Object, ICondition, IParcelable)

Condition::Condition()
    : mIcon(0)
    , mState(0)
    , mFlags(0)
{
}

ECode Condition::constructor(
    /* [in] */ IUri* id,
    /* [in] */ const String& summary,
    /* [in] */ const String& line1,
    /* [in] */ const String& line2,
    /* [in] */ Int32 icon,
    /* [in] */ Int32 state,
    /* [in] */ Int32 flags)
{
    if (id == NULL) {
        Logger::E(TAG, "id is required");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (summary.IsNull()) {
        Logger::E(TAG, "summary is required");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (line1.IsNull()) {
        Logger::E(TAG, "line1 is required");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (line2.IsNull()) {
        Logger::E(TAG, "line2 is required");
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    if (!IsValidState(state)) {
        Logger::E(TAG, "state is invalid: %d", state);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    mId = id;
    mSummary = summary;
    mLine1 = line1;
    mLine2 = line2;
    mIcon= icon;
    mState = state;
    mFlags = flags;
    return NOERROR;
}

ECode Condition::constructor(
    /* [in] */ IParcelable* source)
{
    return NOERROR;
}

Boolean Condition::IsValidState(
    /* [in] */ Int32 state)
{
    return state >= ICondition::STATE_FALSE && state <= ICondition::STATE_ERROR;
}

ECode Condition::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    source->ReadInterfacePtr((Handle32*)((IUri**)&mId));
    source->ReadString(&mSummary);
    source->ReadString(&mLine1);
    source->ReadString(&mLine2);
    source->ReadInt32(&mIcon);
    source->ReadInt32(&mState);
    source->ReadInt32(&mFlags);
    return NOERROR;
}

ECode Condition::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInterfacePtr(mId.Get());
    dest->WriteString(mSummary);
    dest->WriteString(mLine1);
    dest->WriteString(mLine2);
    dest->WriteInt32(mIcon);
    dest->WriteInt32(mState);
    dest->WriteInt32(mFlags);
    return NOERROR;
}

ECode Condition::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    StringBuilder sb("Condition[id=");
    sb += mId;
    sb += ",summary=";
    sb += mSummary;
    sb += ",line1=";
    sb += mLine1;
    sb += ",line2=";
    sb += mLine2;
    sb += ",icon=";
    sb += mIcon;
    sb += ",state=";
    sb += mState;
    sb += ",flags=";
    sb += mFlags;
    sb += ']';
    *str = sb.ToString();
    return NOERROR;
}

ECode Condition::StateToString(
    /* [in] */ Int32 state,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    if (state == ICondition::STATE_FALSE) *str = "STATE_FALSE";
    if (state == ICondition::STATE_TRUE) *str = "STATE_TRUE";
    if (state == ICondition::STATE_UNKNOWN) *str = "STATE_UNKNOWN";
    if (state == ICondition::STATE_ERROR) *str = "STATE_ERROR";
    else {
        Logger::E(TAG, "state is invalid: %d", state);
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode Condition::RelevanceToString(
    /* [in] */ Int32 flags,
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str)
    const Boolean now = (flags & ICondition::FLAG_RELEVANT_NOW) != 0;
    const Boolean always = (flags & ICondition::FLAG_RELEVANT_ALWAYS) != 0;
    if (!now && !always) *str = "NONE";
    if (now && always) *str = "NOW, ALWAYS";
    else *str = now ? "NOW" : "ALWAYS";
    return NOERROR;
}

ECode Condition::Equals(
    /* [in] */ IInterface* o,
    /* [out] */ Boolean* isEquals)
{
    VALIDATE_NOT_NULL(isEquals)
    if (ICondition::Probe(o) == NULL) {
        *isEquals = FALSE;
        return NOERROR;
    }
    if (o == IInterface::Probe((IObject*)this)) {
        *isEquals = TRUE;
        return NOERROR;
    }
    const AutoPtr<ICondition> _other = ICondition::Probe(o);
    AutoPtr<Condition> other = (Condition*)_other.Get();
    AutoPtr<IString> str1, str2, str3, str4, str5, str6;
    CString::New(other->mSummary, (IString**)&str1);
    CString::New(mSummary, (IString**)&str2);
    CString::New(other->mLine1, (IString**)&str3);
    CString::New(mLine1, (IString**)&str4);
    CString::New(other->mLine2, (IString**)&str5);
    CString::New(mLine2, (IString**)&str6);
    *isEquals = Objects::Equals(other->mId, mId)
            && Objects::Equals(str1, str2)
            && Objects::Equals(str3, str4)
            && Objects::Equals(str5, str6)
            && other->mIcon == mIcon
            && other->mState == mState
            && other->mFlags == mFlags;
    return NOERROR;
}

ECode Condition::GetHashCode(
    /* [out] */ Int32* hashCode)
{
    //TODO
    // return Objects.hash(id, summary, line1, line2, icon, state, flags);
    return NOERROR;
}

ECode Condition::Copy(
    /* [out] */ ICondition** copy)
{
    //TODO
    // final Parcel parcel = Parcel.obtain();
    // try {
    //     writeToParcel(parcel, 0);
    //     parcel.setDataPosition(0);
    //     return new Condition(parcel);
    // } finally {
    //     parcel.recycle();
    // }
    return NOERROR;
}

ECode Condition::NewId(
    /* [in] */ IContext* context,
    /* [out] */ IUriBuilder** id)
{
    VALIDATE_NOT_NULL(id)
    AutoPtr<IUriBuilder> builder;
    CUriBuilder::New((IUriBuilder**)&builder);
    builder->Scheme(ICondition::SCHEME);
    String pn;
    context->GetPackageName(&pn);
    builder->Authority(pn);
    *id = builder.Get();
    REFCOUNT_ADD(*id)
    return NOERROR;
}

ECode Condition::IsValidId(
    /* [in] */ IUri* id,
    /* [in] */ const String& pkg,
    /* [out] */ Boolean* isValidId)
{
    VALIDATE_NOT_NULL(isValidId)
    String scheme, authority;
    id->GetScheme(&scheme);
    id->GetAuthority(&authority);
    *isValidId = id != NULL && scheme.Equals(ICondition::SCHEME) && authority.Equals(pkg);
    return NOERROR;
}

} // namespace Notification
} // namespace Service
} // namepsace Droid
} // namespace Elastos