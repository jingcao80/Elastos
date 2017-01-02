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

#include "Elastos.Droid.App.h"
#include "elastos/droid/text/style/EasyEditSpan.h"
#include "elastos/droid/app/CPendingIntent.h"

using Elastos::Droid::App::CPendingIntent;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_3(EasyEditSpan, Object, IEasyEditSpan, IParcelableSpan, IParcelable)

EasyEditSpan::EasyEditSpan()
    : mDeleteEnabled(TRUE)
{
}

EasyEditSpan::~EasyEditSpan()
{}

ECode EasyEditSpan::constructor()
{
    return NOERROR;
}

ECode EasyEditSpan::constructor(
    /* [in] */ IPendingIntent* intent)
{
    mPendingIntent = intent;
    return NOERROR;
}

ECode EasyEditSpan::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    mPendingIntent = NULL;
    CPendingIntent::New((IPendingIntent**)&mPendingIntent);
    IParcelable::Probe(mPendingIntent)->ReadFromParcel(source);
    Byte b;
    source->ReadByte(&b);
    mDeleteEnabled = (b == 1);
    return NOERROR;
}

ECode EasyEditSpan::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    IParcelable::Probe(mPendingIntent)->WriteToParcel(dest);
    dest->WriteByte(mDeleteEnabled ? (Byte)1 : (Byte) 0);
    return NOERROR;
}

ECode EasyEditSpan::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::EASY_EDIT_SPAN;
    return NOERROR;
}

ECode EasyEditSpan::IsDeleteEnabled(
    /* [out] */ Boolean* enabled)
{
    VALIDATE_NOT_NULL(enabled)
    *enabled = mDeleteEnabled;
    return NOERROR;
}

ECode EasyEditSpan::SetDeleteEnabled(
    /* [in] */ Boolean value)
{
    mDeleteEnabled = value;
    return NOERROR;
}

ECode EasyEditSpan::GetPendingIntent(
    /* [out] */ IPendingIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mPendingIntent;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos
