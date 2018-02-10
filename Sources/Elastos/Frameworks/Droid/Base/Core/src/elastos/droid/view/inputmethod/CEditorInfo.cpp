//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Location.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/droid/view/inputmethod/CEditorInfo.h"
#include "elastos/droid/os/Build.h"
#include "elastos/droid/text/TextUtils.h"

using Elastos::Droid::Os::Build;
using Elastos::Droid::Text::EIID_IInputType;
using Elastos::Droid::Text::TextUtils;

namespace Elastos {
namespace Droid {
namespace View {
namespace InputMethod {

//========================================================================================
//              CEditorInfo::
//========================================================================================
CAR_INTERFACE_IMPL_3(CEditorInfo, Object, IEditorInfo, IInputType, IParcelable)

CAR_OBJECT_IMPL(CEditorInfo)

CEditorInfo::CEditorInfo()
    : mInputType(IInputType::TYPE_NULL)
    , mImeOptions(IEditorInfo::IME_NULL)
    , mActionId(0)
    , mInitialSelStart(-1)
    , mInitialSelEnd(-1)
    , mInitialCapsMode(0)
    , mFieldId(0)
{}

CEditorInfo::~CEditorInfo()
{
    // TODO: detete, for debug
    mActionLabel = NULL;
    mHintText = NULL;
    mLabel = NULL;
    mExtras = NULL;
}

ECode CEditorInfo::constructor()
{
    return NOERROR;
}

ECode CEditorInfo::ReadFromParcel(
    /* [in] */ IParcel *source)
{
    source->ReadInt32(&mInputType);
    source->ReadInt32(&mImeOptions);
    source->ReadString(&mPrivateImeOptions);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mActionLabel);
    source->ReadInt32(&mActionId);
    source->ReadInt32(&mInitialSelStart);
    source->ReadInt32(&mInitialSelEnd);
    source->ReadInt32(&mInitialCapsMode);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mHintText);
    TextUtils::CHAR_SEQUENCE_CREATOR::CreateFromParcel(source, (ICharSequence**)&mLabel);
    source->ReadString(&mPackageName);
    source->ReadInt32(&mFieldId);
    source->ReadString(&mFieldName);
    AutoPtr<IInterface> obj;
    source->ReadInterfacePtr((HANDLE*)&obj);
    mExtras = IBundle::Probe(obj);
    return NOERROR;
}

ECode CEditorInfo::WriteToParcel(
    /* [in] */ IParcel *dest)
{
    dest->WriteInt32(mInputType);
    dest->WriteInt32(mImeOptions);
    dest->WriteString(mPrivateImeOptions);
    TextUtils::WriteToParcel(mActionLabel, dest);
    dest->WriteInt32(mActionId);
    dest->WriteInt32(mInitialSelStart);
    dest->WriteInt32(mInitialSelEnd);
    dest->WriteInt32(mInitialCapsMode);
    TextUtils::WriteToParcel(mHintText, dest);
    TextUtils::WriteToParcel(mLabel, dest);
    dest->WriteString(mPackageName);
    dest->WriteInt32(mFieldId);
    dest->WriteString(mFieldName);
    dest->WriteInterfacePtr(mExtras);
    return NOERROR;
}

ECode CEditorInfo::GetInputType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mInputType;
    return NOERROR;
}

ECode CEditorInfo::SetInputType(
    /* [in] */ Int32 type)
{
    mInputType = type;
    return NOERROR;
}

ECode CEditorInfo::GetImeOptions(
    /* [out] */ Int32* options)
{
    VALIDATE_NOT_NULL(options);
    *options = mImeOptions;
    return NOERROR;
}

ECode CEditorInfo::SetImeOptions(
    /* [in] */ Int32 options)
{
    mImeOptions = options;
    return NOERROR;
}

ECode CEditorInfo::GetPrivateImeOptions(
    /* [out] */ String* options)
{
    VALIDATE_NOT_NULL(options);
    *options = mPrivateImeOptions;
    return NOERROR;
}

ECode CEditorInfo::SetPrivateImeOptions(
    /* [in] */ const String& options)
{
    mPrivateImeOptions = options;
    return NOERROR;
}

ECode CEditorInfo::GetActionLabel(
    /* [out] */ ICharSequence** actionLabel)
{
    VALIDATE_NOT_NULL(actionLabel);
    *actionLabel = mActionLabel;
    REFCOUNT_ADD(*actionLabel);
    return NOERROR;
}

ECode CEditorInfo::SetActionLabel(
    /*[in]  */ ICharSequence* actionLabel)
{
    mActionLabel = actionLabel;
    return NOERROR;
}

ECode CEditorInfo::GetActionId(
    /* [out] */ Int32* actionId)
{
    VALIDATE_NOT_NULL(actionId);
    *actionId = mActionId;
    return NOERROR;
}

ECode CEditorInfo::SetActionId(
    /* [in] */ Int32 actionId)
{
    mActionId = actionId;
    return NOERROR;
}

ECode CEditorInfo::GetInitialSelStart(
    /* [out] */ Int32* initialSelStart)
{
    VALIDATE_NOT_NULL(initialSelStart);
    *initialSelStart = mInitialSelStart;
    return NOERROR;
}

ECode CEditorInfo::SetInitialSelStart(
    /* [in] */ Int32 initialSelStart)
{
    mInitialSelStart = initialSelStart;
    return NOERROR;
}

ECode CEditorInfo::GetInitialSelEnd(
    /* [out] */ Int32* initialSelEnd)
{
    VALIDATE_NOT_NULL(initialSelEnd);
    *initialSelEnd = mInitialSelEnd;
    return NOERROR;
}

ECode CEditorInfo::SetInitialSelEnd(
    /* [in] */ Int32 initialSelEnd)
{
    mInitialSelEnd = initialSelEnd;
    return NOERROR;
}

ECode CEditorInfo::GetInitialCapsMode(
    /* [out] */ Int32* initialCapsMode)
{
    VALIDATE_NOT_NULL(initialCapsMode);
    *initialCapsMode = mInitialCapsMode;
    return NOERROR;
}

ECode CEditorInfo::SetInitialCapsMode(
    /* [in] */ Int32 initialCapsMode)
{
    mInitialCapsMode = initialCapsMode;
    return NOERROR;
}

ECode CEditorInfo::GetHintText(
    /* [out] */ ICharSequence** hintText)
{
    VALIDATE_NOT_NULL(hintText);
    *hintText = mHintText;
    REFCOUNT_ADD(*hintText);
    return NOERROR;
}

ECode CEditorInfo::SetHintText(
    /* [in] */ ICharSequence* hintText)
{
    mHintText = hintText;
    return NOERROR;
}

ECode CEditorInfo::GetLabel(
    /* [out] */ ICharSequence** label)
{
    VALIDATE_NOT_NULL(label);
    *label = mLabel;
    REFCOUNT_ADD(*label);
    return NOERROR;
}

ECode CEditorInfo::SetLabel(
    /* [in] */ ICharSequence* label)
{
    mLabel = label;
    return NOERROR;
}

ECode CEditorInfo::GetPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName);
    *packageName = mPackageName;
    return NOERROR;
}

ECode CEditorInfo::SetPackageName(
    /* [in] */ const String& packageName)
{
    mPackageName = packageName;
    return NOERROR;
}

ECode CEditorInfo::GetFieldId(
    /* [out] */ Int32* fieldId)
{
    VALIDATE_NOT_NULL(fieldId);
    *fieldId = mFieldId;
    return NOERROR;
}

ECode CEditorInfo::SetFieldId(
    /* [in] */ Int32 fieldId)
{
    mFieldId = fieldId;
    return NOERROR;
}

ECode CEditorInfo::GetFieldName(
    /* [out] */ String* fieldName)
{
    VALIDATE_NOT_NULL(fieldName);
    *fieldName = mFieldName;
    return NOERROR;
}

ECode CEditorInfo::SetFieldName(
    /* [in] */ const String& fieldName)
{
    mFieldName = fieldName;
    return NOERROR;
}

ECode CEditorInfo::GetExtras(
    /* [out] */ IBundle** extras)
{
    VALIDATE_NOT_NULL(extras);
    *extras = mExtras;
    REFCOUNT_ADD(*extras);
    return NOERROR;
}

ECode CEditorInfo::SetExtras(
    /* [in] */ IBundle* extras)
{
    mExtras = extras;
    return NOERROR;
}

ECode CEditorInfo::MakeCompatible(
    /*[in]  */ Int32 targetSdkVersion)
{
    if (targetSdkVersion < Build::VERSION_CODES::HONEYCOMB) {
        switch (mInputType & (IInputType::TYPE_MASK_CLASS | IInputType::TYPE_MASK_VARIATION)) {
            case IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_WEB_EMAIL_ADDRESS:
                mInputType = IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_EMAIL_ADDRESS
                        | (mInputType & IInputType::TYPE_MASK_FLAGS);
                break;
            case IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_WEB_PASSWORD:
                mInputType = IInputType::TYPE_CLASS_TEXT | IInputType::TYPE_TEXT_VARIATION_PASSWORD
                        | (mInputType & IInputType::TYPE_MASK_FLAGS);
                break;
            case IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_NORMAL:
            case IInputType::TYPE_CLASS_NUMBER | IInputType::TYPE_NUMBER_VARIATION_PASSWORD:
                mInputType = IInputType::TYPE_CLASS_NUMBER
                        | (mInputType & IInputType::TYPE_MASK_FLAGS);
                break;
        }
    }
    return NOERROR;
}

} // namespace InputMethod
} // namespace View
} // namespace Droid
} // namespace Elastos
