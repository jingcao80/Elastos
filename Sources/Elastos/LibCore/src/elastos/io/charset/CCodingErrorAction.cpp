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

#include "CCodingErrorAction.h"

namespace Elastos {
namespace IO {
namespace Charset {

CAR_INTERFACE_IMPL(CCodingErrorAction, Object, ICodingErrorAction)

CAR_OBJECT_IMPL(CCodingErrorAction)

AutoPtr<ICodingErrorAction> CreateAction(
    /* [in] */ const String& action)
{
    AutoPtr<CCodingErrorAction> p;
    CCodingErrorAction::NewByFriend((CCodingErrorAction**)&p);
    p->mAction = action;
    return (ICodingErrorAction*)p.Get();
}

const AutoPtr<ICodingErrorAction> CCodingErrorAction::IGNORE = CreateAction(String("IGNORE"));
const AutoPtr<ICodingErrorAction> CCodingErrorAction::REPLACE = CreateAction(String("REPLACE"));
const AutoPtr<ICodingErrorAction> CCodingErrorAction::REPORT = CreateAction(String("REPORT"));

ECode CCodingErrorAction::constructor()
{
    return NOERROR;
}

ECode CCodingErrorAction::GetIGNORE(
    /* [out] */ ICodingErrorAction** ignore)
{
    VALIDATE_NOT_NULL(ignore);
    *ignore = IGNORE;
    REFCOUNT_ADD(*ignore);
    return NOERROR;
}

ECode CCodingErrorAction::GetREPLACE(
    /* [out] */ ICodingErrorAction** replace)
{
    VALIDATE_NOT_NULL(replace);
    *replace = REPLACE;
    REFCOUNT_ADD(*replace);
    return NOERROR;
}

ECode CCodingErrorAction::GetREPORT(
    /* [out] */ ICodingErrorAction** report)
{
    VALIDATE_NOT_NULL(report);
    *report = REPORT;
    REFCOUNT_ADD(*report);
    return NOERROR;
}

ECode CCodingErrorAction::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    *str = String("Action: ");
    *str += (mAction);
    return NOERROR;
}

} // namespace Charset
} // namespace IO
} // namespace Elastos
