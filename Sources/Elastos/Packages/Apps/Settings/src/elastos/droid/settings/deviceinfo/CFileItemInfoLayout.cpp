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

#include "Elastos.CoreLibrary.IO.h"
#include "elastos/droid/settings/deviceinfo/CFileItemInfoLayout.h"
#include "elastos/droid/os/UserHandle.h"
#include "../R.h"
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::IUserEnvironment;
using Elastos::Droid::Os::CUserEnvironment;
using Elastos::Droid::Os::UserHandle;
using Elastos::Droid::Widget::EIID_ICheckable;
using Elastos::IO::IFile;
using Elastos::Core::CoreUtils;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Deviceinfo {

static Int32 InitsLengthExternalStorageDirPrefix()
{
    AutoPtr<IUserEnvironment> ue;
    CUserEnvironment::New(UserHandle::GetMyUserId(), (IUserEnvironment**)&ue);
    AutoPtr<IFile> file;
    ue->GetExternalStorageDirectory((IFile**)&file);
    String path;
    file->GetAbsolutePath(&path);
    return path.GetLength() + 1;
}

const Int32 CFileItemInfoLayout::sLengthExternalStorageDirPrefix = InitsLengthExternalStorageDirPrefix();

CAR_INTERFACE_IMPL(CFileItemInfoLayout, RelativeLayout, ICheckable)

CAR_OBJECT_IMPL(CFileItemInfoLayout)

CFileItemInfoLayout::CFileItemInfoLayout()
{}

CFileItemInfoLayout::~CFileItemInfoLayout()
{}

ECode CFileItemInfoLayout::constructor(
    /* [in] */ IContext* context)
{
    return constructor(context, NULL);
}

ECode CFileItemInfoLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    return constructor(context, attrs, 0);
}

ECode CFileItemInfoLayout::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs,
    /* [in] */ Int32 defStyle)
{
    return RelativeLayout::constructor(context, attrs, defStyle);
}

ECode CFileItemInfoLayout::Toggle()
{
    Boolean res;
    ICheckable::Probe(mCheckbox)->IsChecked(&res);
    return SetChecked(!res);
}

ECode CFileItemInfoLayout::OnFinishInflate()
{
    RelativeLayout::OnFinishInflate();
    AutoPtr<IView> tmp;
    FindViewById(R::id::misc_filename, (IView**)&tmp);
    mFileNameView = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::misc_filesize, (IView**)&tmp);
    mFileSizeView = ITextView::Probe(tmp);
    tmp = NULL;
    FindViewById(R::id::misc_checkbox, (IView**)&tmp);
    mCheckbox = ICheckBox::Probe(tmp);
    return NOERROR;
}

ECode CFileItemInfoLayout::SetFileName(
    /* [in] */ const String& fileName)
{
    return mFileNameView->SetText(CoreUtils::Convert(fileName.Substring(sLengthExternalStorageDirPrefix)));
}

ECode CFileItemInfoLayout::SetFileSize(
    /* [in] */ const String& filesize)
{
    mFileSizeView->SetText(CoreUtils::Convert(filesize));
    return NOERROR;
}

ECode CFileItemInfoLayout::IsChecked(
    /* [out] */ Boolean* result)
{
    return ICheckable::Probe(mCheckbox)->IsChecked(result);
}

AutoPtr<ICheckBox> CFileItemInfoLayout::GetCheckBox()
{
    return mCheckbox;
}

ECode CFileItemInfoLayout::SetChecked(
    /* [in] */ Boolean checked)
{
    ICheckable::Probe(mCheckbox)->SetChecked(checked);
    return NOERROR;
}

} // namespace Deviceinfo
} // namespace Settings
} // namespace Droid
} // namespace Elastos