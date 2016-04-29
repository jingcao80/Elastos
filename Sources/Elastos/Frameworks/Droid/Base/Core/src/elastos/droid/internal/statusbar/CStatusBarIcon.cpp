#include "elastos/droid/internal/statusbar/CStatusBarIcon.h"
#include "elastos/droid/os/CUserHandle.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::StringBuilder;
using Elastos::Core::CString;
using Elastos::Droid::Os::CUserHandle;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace StatusBar {

CAR_OBJECT_IMPL(CStatusBarIcon);
CAR_INTERFACE_IMPL_2(CStatusBarIcon, Object, IStatusBarIcon, IParcelable);
CStatusBarIcon::CStatusBarIcon()
    : mIconId(0)
    , mIconLevel(0)
    , mVisible(TRUE)
    , mNumber(0)
{
}

ECode CStatusBarIcon::constructor()
{
    return NOERROR;
}

ECode CStatusBarIcon::constructor(
    /* [in] */ const String& iconPackage,
    /* [in] */ IUserHandle* user,
    /* [in] */ Int32 iconId,
    /* [in] */ Int32 iconLevel,
    /* [in] */ Int32 number,
    /* [in] */ ICharSequence* contentDescription)
{
    mIconPackage = iconPackage;
    mUser = user;
    mIconId = iconId;
    mIconLevel = iconLevel;
    mNumber = number;
    mContentDescription = contentDescription;
    return NOERROR;
}

ECode CStatusBarIcon::ToString(
    /* [out] */ String* result)
{
    VALIDATE_NOT_NULL(result);
    StringBuilder sb("StatusBarIcon(pkg=");
    sb += mIconPackage;
    sb += "content=";
    if (mContentDescription != NULL) {
        String tmp;
        mContentDescription->ToString(&tmp);
        sb += tmp;
    }
    else {
        sb += "NULL";
    }
    sb += ", user=";
    Int32 id = 0;
    mUser->GetIdentifier(&id);
    sb += id;
    sb += ", id=0x";
    sb += mIconId;
    sb += ", level=";
    sb += mIconLevel;
    sb += ", visible=";
    sb += mVisible;
    sb += ", num=";
    sb += mNumber;
    sb += " )";
    *result = sb.ToString();
    return NOERROR;
}

ECode CStatusBarIcon::Clone(
    /* [out] */ IStatusBarIcon** object)
{
    VALIDATE_NOT_NULL(object)
    AutoPtr<CStatusBarIcon> that;
    CStatusBarIcon::NewByFriend(mIconPackage, mUser, mIconId,
        mIconLevel, mNumber, mContentDescription, (CStatusBarIcon**)&that);
    that->mVisible = mVisible;
    *object = that;
    REFCOUNT_ADD(*object)
    return NOERROR;
}

ECode CStatusBarIcon::ReadFromParcel(
    /* [in] */ IParcel* in)
{
    Int32 value;
    in->ReadString(&mIconPackage);

    mUser = NULL;
    in->ReadInt32(&value);
    if (value != 0) {
        CUserHandle::New((IUserHandle**)&mUser);
        IParcelable::Probe(mUser)->ReadFromParcel(in);
    }

    in->ReadInt32(&mIconId);
    in->ReadInt32(&mIconLevel);
    in->ReadBoolean(&mVisible);
    in->ReadInt32(&mNumber);

    mContentDescription = NULL;
    in->ReadInt32(&value);
    if (value != 0) {
        String cd;
        in->ReadString(&cd);
        CString::New(cd, (ICharSequence**)&mContentDescription);
    }

    return NOERROR;
}

ECode CStatusBarIcon::WriteToParcel(
    /* [in] */ IParcel* out)
{
    out->WriteString(mIconPackage);

    if (mUser == NULL) {
        out->WriteInt32(0);
    }
    else {
        out->WriteInt32(1);
        IParcelable::Probe(mUser)->WriteToParcel(out);
    }

    out->WriteInt32(mIconId);
    out->WriteInt32(mIconLevel);
    out->WriteBoolean(mVisible);
    out->WriteInt32(mNumber);

    if (mContentDescription == NULL) {
        out->WriteInt32(0);
    }
    else {
        out->WriteInt32(1);
        String cd;
        mContentDescription->ToString(&cd);
        out->WriteString(cd);
    }
    return NOERROR;
}

ECode CStatusBarIcon::GetVisible(
    /* [out] */ Boolean* visible)
{
    VALIDATE_NOT_NULL(visible);
    *visible = mVisible;
    return NOERROR;
}

ECode CStatusBarIcon::SetVisible(
    /* [in] */ Boolean visible)
{
    mVisible = visible;
    return NOERROR;
}

ECode CStatusBarIcon::GetIconPackage(
    /* [out] */ String* pkg)
{
    VALIDATE_NOT_NULL(pkg);
    *pkg = mIconPackage;
    return NOERROR;
}

ECode CStatusBarIcon::SetIconPackage(
    /* [in] */ const String& pkg)
{
    mIconPackage = pkg;
    return NOERROR;
}

ECode CStatusBarIcon::GetIconId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mIconId;
    return NOERROR;
}

ECode CStatusBarIcon::SetIconId(
    /* [in] */ Int32 id)
{
    mIconId = id;
    return NOERROR;
}

ECode CStatusBarIcon::GetIconLevel(
    /* [out] */ Int32* level)
{
    VALIDATE_NOT_NULL(level);
    *level = mIconLevel;
    return NOERROR;
}

ECode CStatusBarIcon::SetIconLevel(
    /* [in] */ Int32 level)
{
    mIconLevel = level;
    return NOERROR;
}

ECode CStatusBarIcon::GetNumber(
    /* [out] */ Int32* number)
{
    VALIDATE_NOT_NULL(number);
    *number = mNumber;
    return NOERROR;
}

ECode CStatusBarIcon::SetNumber(
    /* [in] */ Int32 number)
{
    mNumber = number;
    return NOERROR;
}

ECode CStatusBarIcon::GetContentDescription(
    /* [out] */ ICharSequence** seq)
{
    VALIDATE_NOT_NULL(seq);
    *seq = mContentDescription;
    REFCOUNT_ADD(*seq);
    return NOERROR;
}

ECode CStatusBarIcon::SetContentDescription(
    /* [in] */ ICharSequence* seq)
{
    mContentDescription = seq;
    return NOERROR;
}

ECode CStatusBarIcon::SetUser(
    /* [in] */ IUserHandle* user)
{
    mUser = user;
    return NOERROR;
}

ECode CStatusBarIcon::GetUser(
    /* [out] */ IUserHandle** user)
{
    VALIDATE_NOT_NULL(user);
    *user = mUser;
    REFCOUNT_ADD(*user);
    return NOERROR;
}

} //namespace StatusBar
} //namespace Internal
} //namespace Droid
} //namespace Elastos
