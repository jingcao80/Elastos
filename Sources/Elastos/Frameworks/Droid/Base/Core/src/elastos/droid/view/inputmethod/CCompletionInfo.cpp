
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/view/inputmethod/CCompletionInfo.h"

using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CCompletionInfo::
//========================================================================================
CAR_INTERFACE_IMPL_2(CCompletionInfo, Object, ICompletionInfo, IParcelable)

CAR_OBJECT_IMPL(CCompletionInfo)

CCompletionInfo::CCompletionInfo()
    : mId(0)
    , mPosition(0)
{}

ECode CCompletionInfo::constructor()
{
    return NOERROR;
}

ECode CCompletionInfo::constructor(
    /* [in] */ Int64 id,
    /* [in] */ Int32 index,
    /* [in] */ ICharSequence* text)
{
    mId = id;
    mPosition = index;
    mText = text;
    mLabel = NULL;
    return NOERROR;
}

ECode CCompletionInfo::constructor(
    /* [in] */ Int64 id,
    /* [in] */ Int32 index,
    /* [in] */ ICharSequence* text,
    /* [in] */ ICharSequence* label)
{
    mId = id;
    mPosition = index;
    mText = text;
    mLabel = label;
    return NOERROR;
}

ECode CCompletionInfo::GetId(
    /* [out] */ Int64* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mId;
    return NOERROR;
}

ECode CCompletionInfo::GetPosition(
    /* [out] */ Int32* pos)
{
    VALIDATE_NOT_NULL(pos);
    *pos = mPosition;
    return NOERROR;
}

ECode CCompletionInfo::GetText(
    /* [out] */ ICharSequence** text)
{
    VALIDATE_NOT_NULL(text);
    *text = mText;
    REFCOUNT_ADD(*text);

    return NOERROR;
}

ECode CCompletionInfo::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CCompletionInfo::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    source->ReadInt64(&mId);
    source->ReadInt32(&mPosition);
    String strText, strLable;
    source->ReadString(&strText);
    source->ReadString(&strLable);

    mText = NULL;
    mLabel = NULL;

    if (!strText.IsNull()) {
        CString::New(strText, (ICharSequence**)&mText);
    }

    if (!strLable.IsNull()) {
        CString::New(strLable, (ICharSequence**)&mLabel);
    }

    return NOERROR;
}

ECode CCompletionInfo::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteInt64(mId);
    dest->WriteInt32(mPosition);
    String strText, strLable;
    if (mText)
        mText->ToString(&strText);
    if (mLabel)
        mLabel->ToString(&strLable);
    dest->WriteString(strText);
    dest->WriteString(strLable);

    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
