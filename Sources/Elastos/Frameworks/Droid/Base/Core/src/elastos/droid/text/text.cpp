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

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/text/CAlteredCharSequence.h"
#include "elastos/droid/text/CAnnotation.h"
#include "elastos/droid/text/CBoringLayout.h"
#include "elastos/droid/text/CDynamicLayout.h"
#include "elastos/droid/text/CStaticLayout.h"
#include "elastos/droid/text/CNoCopySpanConcrete.h"
#include "elastos/droid/text/CSpannableStringBuilder.h"
#include "elastos/droid/text/CTextPaint.h"

namespace Elastos {
namespace Droid {
namespace Text {

ECode CNoCopySpanConcrete::constructor()
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(CNoCopySpanConcrete, Object, INoCopySpan)
CAR_OBJECT_IMPL(CNoCopySpanConcrete)

CAR_OBJECT_IMPL(CAlteredCharSequence)
CAR_OBJECT_IMPL(CAnnotation)
CAR_OBJECT_IMPL(CBoringLayout)
CAR_OBJECT_IMPL(CDynamicLayout)
CAR_OBJECT_IMPL(CStaticLayout)
CAR_OBJECT_IMPL(CSpannableStringBuilder)
CAR_OBJECT_IMPL(CTextPaint)


} // namespace Text
} // namepsace Droid
} // namespace Elastos