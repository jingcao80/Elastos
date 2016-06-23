
#include "Elastos.Droid.Content.h"
#include "elastos/droid/app/CActionKeyInfo.h"
#include "elastos/droid/R.h"

using Elastos::Droid::R;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace App {

CAR_INTERFACE_IMPL_2(CActionKeyInfo, Object, IActionKeyInfo, IParcelable)

CAR_OBJECT_IMPL(CActionKeyInfo)

CActionKeyInfo::CActionKeyInfo()
    : mKeyCode(0)
{}

ECode CActionKeyInfo::constructor()
{
    return NOERROR;
}

ECode CActionKeyInfo::constructor(
    /* [in] */ IContext* activityContext,
    /* [in] */ IAttributeSet* attr)
{
    AutoPtr<ArrayOf<Int32> > attrIds = TO_ATTRS_ARRAYOF(R::styleable::SearchableActionKey);
    AutoPtr<ITypedArray>  a;
    activityContext->ObtainStyledAttributes(attr, attrIds, (ITypedArray**)&a);

    a->GetInt32(R::styleable::SearchableActionKey_keycode, 0, &mKeyCode);
    a->GetString(R::styleable::SearchableActionKey_queryActionMsg, &mQueryActionMsg);
    a->GetString(R::styleable::SearchableActionKey_suggestActionMsg, &mSuggestActionMsg);
    a->GetString(R::styleable::SearchableActionKey_suggestActionMsgColumn, &mSuggestActionMsgColumn);
    a->Recycle();

    //sanity check
    if (mKeyCode == 0) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    else if((mQueryActionMsg == NULL)
        && (mSuggestActionMsg == NULL)
        && (mSuggestActionMsgColumn == NULL)) {
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }
    return NOERROR;
}

ECode CActionKeyInfo::GetKeyCode(
    /* [out] */ Int32* keyCode)
{
    VALIDATE_NOT_NULL(keyCode)
    *keyCode = mKeyCode;
    return NOERROR;
}

ECode CActionKeyInfo::GetQueryActionMsg(
    /* [out] */ String* actionMsg)
{
    VALIDATE_NOT_NULL(actionMsg)
    *actionMsg = mQueryActionMsg;
    return NOERROR;
}

ECode CActionKeyInfo::GetSuggestActionMsg(
    /* [out] */ String* actionMsg)
{
    VALIDATE_NOT_NULL(actionMsg)
    *actionMsg = mSuggestActionMsg;
    return NOERROR;
}

ECode CActionKeyInfo::GetSuggestActionMsgColumn(
    /* [out] */ String* column)
{
    VALIDATE_NOT_NULL(column)
    *column = mSuggestActionMsgColumn;
    return NOERROR;
}

ECode CActionKeyInfo::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    source->ReadInt32(&mKeyCode);
    source->ReadString(&mQueryActionMsg);
    source->ReadString(&mSuggestActionMsg);
    source->ReadString(&mSuggestActionMsgColumn);
    return NOERROR;
}

ECode CActionKeyInfo::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteInt32(mKeyCode);
    dest->WriteString(mQueryActionMsg);
    dest->WriteString(mSuggestActionMsg);
    dest->WriteString(mSuggestActionMsgColumn);
    return NOERROR;
}

}
}
}