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

#include "elastos/droid/text/method/DateTimeKeyListener.h"
#include "elastos/droid/text/method/CDateTimeKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

AutoPtr<IDateTimeKeyListener> DateTimeKeyListener::sInstance;

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'm', 'p', ':', '/', '-', ' ' };
    AutoPtr<ArrayOf<Char32> > chars = ArrayOf<Char32>::Alloc(ArraySize(ch));
    chars->Copy(ch, ArraySize(ch));
    return chars;
}

const AutoPtr<ArrayOf<Char32> > DateTimeKeyListener::CHARACTERS = InitCHARACTERS();

CAR_INTERFACE_IMPL(DateTimeKeyListener, NumberKeyListener, IDateTimeKeyListener)

ECode DateTimeKeyListener::constructor()
{
    return NOERROR;
}

ECode DateTimeKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = IInputType::TYPE_CLASS_DATETIME | IInputType::TYPE_DATETIME_VARIATION_NORMAL;
    return NOERROR;
}

AutoPtr< ArrayOf<Char32> > DateTimeKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode DateTimeKeyListener::GetCHARACTERS(
    /* [out] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DateTimeKeyListener::GetInstance(
    /* [out] */ IDateTimeKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret)
    if (sInstance == NULL) {
        CDateTimeKeyListener::New((IDateTimeKeyListener**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode DateTimeKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

ECode DateTimeKeyListener::ClearMetaKeyState(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 states)
{
    return MetaKeyKeyListener::ClearMetaKeyState(view, content, states);
}

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos