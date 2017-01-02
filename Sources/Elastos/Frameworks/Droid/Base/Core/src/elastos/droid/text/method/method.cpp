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

#include "elastos/droid/text/method/CDateKeyListener.h"
#include "elastos/droid/text/method/CTextKeyListener.h"
#include "elastos/droid/text/method/CNullKeyListener.h"
#include "elastos/droid/text/method/CDateTimeKeyListener.h"
#include "elastos/droid/text/method/CQwertyKeyListener.h"
#include "elastos/droid/text/method/CDialerKeyListener.h"
#include "elastos/droid/text/method/CTimeKeyListener.h"
#include "elastos/droid/text/method/CDigitsKeyListener.h"
#include "elastos/droid/text/method/CMultiTapKeyListener.h"
#include "elastos/droid/text/method/CBaseMovementMethod.h"
#include "elastos/droid/text/method/CAllCapsTransformationMethod.h"
#include "elastos/droid/text/method/CArrowKeyMovementMethod.h"
#include "elastos/droid/text/method/CHideReturnsTransformationMethod.h"
#include "elastos/droid/text/method/CScrollingMovementMethod.h"
#include "elastos/droid/text/method/CSingleLineTransformationMethod.h"
#include "elastos/droid/text/method/CLinkMovementMethod.h"
#include "elastos/droid/text/method/CWordIterator.h"
#include "elastos/droid/text/method/CCharacterPickerDialog.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

CAR_OBJECT_IMPL(CDateKeyListener)
CAR_OBJECT_IMPL(CTextKeyListener)
CAR_OBJECT_IMPL(CNullKeyListener)
CAR_OBJECT_IMPL(CDateTimeKeyListener)
CAR_OBJECT_IMPL(CQwertyKeyListener)
CAR_OBJECT_IMPL(CDialerKeyListener)
CAR_OBJECT_IMPL(CTimeKeyListener)
CAR_OBJECT_IMPL(CDigitsKeyListener)
CAR_OBJECT_IMPL(CMultiTapKeyListener)
CAR_OBJECT_IMPL(CBaseMovementMethod)
CAR_OBJECT_IMPL(CAllCapsTransformationMethod)
CAR_OBJECT_IMPL(CArrowKeyMovementMethod)
CAR_OBJECT_IMPL(CHideReturnsTransformationMethod)
CAR_OBJECT_IMPL(CLinkMovementMethod)
CAR_OBJECT_IMPL(CScrollingMovementMethod)
CAR_OBJECT_IMPL(CSingleLineTransformationMethod)
CAR_OBJECT_IMPL(CWordIterator)
CAR_OBJECT_IMPL(CCharacterPickerDialog)

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos