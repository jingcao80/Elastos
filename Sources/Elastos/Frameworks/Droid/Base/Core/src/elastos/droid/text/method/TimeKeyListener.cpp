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

#include "elastos/droid/text/method/TimeKeyListener.h"
#include "elastos/droid/text/method/CTimeKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

static AutoPtr<ArrayOf<Char32> > InitCHARACTERS()
{
    Char32 ch[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'm', 'p', ':' };
    AutoPtr<ArrayOf<Char32> > array = ArrayOf<Char32>::Alloc(ArraySize(ch));
    array->Copy(ch, ArraySize(ch));
    return array;
}

AutoPtr<ITimeKeyListener> TimeKeyListener::sInstance;

const AutoPtr<ArrayOf<Char32> > TimeKeyListener::CHARACTERS = InitCHARACTERS();

CAR_INTERFACE_IMPL(TimeKeyListener, NumberKeyListener, ITimeKeyListener);

TimeKeyListener::TimeKeyListener()
{}

TimeKeyListener::~TimeKeyListener()
{}

ECode TimeKeyListener::constructor()
{
    return NOERROR;
}

ECode TimeKeyListener::GetInstance(
    /* [out] */ ITimeKeyListener** ret)
{
    VALIDATE_NOT_NULL(ret);
    if (sInstance == NULL) {
        CTimeKeyListener::New((ITimeKeyListener**)&sInstance);
    }

    *ret = sInstance;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

ECode TimeKeyListener::GetInputType(
    /* [out] */ Int32* ret)
{
    VALIDATE_NOT_NULL(ret);
    *ret = IInputType::TYPE_CLASS_DATETIME | IInputType::TYPE_DATETIME_VARIATION_TIME;
    return NOERROR;
}

AutoPtr< ArrayOf<Char32> > TimeKeyListener::GetAcceptedChars()
{
    return CHARACTERS;
}

ECode TimeKeyListener::GetCHARACTERS(
    /* [out] */ ArrayOf<Char32>** ret)
{
    VALIDATE_NOT_NULL(ret)
    *ret = CHARACTERS;
    REFCOUNT_ADD(*ret);
    return NOERROR;
}

//override
ECode TimeKeyListener::OnKeyUp(
    /* [in] */ IView* view,
    /* [in] */ IEditable* content,
    /* [in] */ Int32 keyCode,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* ret)
{
    return MetaKeyKeyListener::OnKeyUp(view, content, keyCode, event, ret);
}

CARAPI TimeKeyListener::ClearMetaKeyState(
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