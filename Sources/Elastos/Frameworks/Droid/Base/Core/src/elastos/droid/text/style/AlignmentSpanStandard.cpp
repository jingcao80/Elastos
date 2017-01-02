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

#include "elastos/droid/text/style/AlignmentSpanStandard.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Style {

CAR_INTERFACE_IMPL_5(AlignmentSpanStandard, Object, IAlignmentSpanStandard, IAlignmentSpan, IParagraphStyle, IParcelableSpan, IParcelable)

AlignmentSpanStandard::AlignmentSpanStandard()
    : mAlignment(0)
{
}

AlignmentSpanStandard::~AlignmentSpanStandard()
{}

ECode AlignmentSpanStandard::constructor()
{
    return NOERROR;
}

ECode AlignmentSpanStandard::constructor(
    /* [in] */ LayoutAlignment align)
{
    mAlignment = align;
    return NOERROR;
}

ECode AlignmentSpanStandard::GetAlignment(
    /* [out] */ LayoutAlignment* align)
{
    VALIDATE_NOT_NULL(align)
    *align = mAlignment;
    return NOERROR;
}

ECode AlignmentSpanStandard::GetSpanTypeId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id)
    *id = ITextUtils::ALIGNMENT_SPAN;
    return NOERROR;
}

ECode AlignmentSpanStandard::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    Int32 ival;
    source->ReadInt32(&ival);
    mAlignment = (LayoutAlignment)ival;
    return NOERROR;
}

ECode AlignmentSpanStandard::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    dest->WriteInt32((Int32)mAlignment);
    return NOERROR;
}

} // namespace Style
} // namespace Text
} // namepsace Droid
} // namespace Elastos