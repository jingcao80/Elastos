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

#include "CToggleStates.h"
#include "CInputModeSwitcher.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CAR_INTERFACE_IMPL(CToggleStates, Object, IToggleStates)

CAR_OBJECT_IMPL(CToggleStates)

CToggleStates::CToggleStates()
    : mQwerty(FALSE)
    , mQwertyUpperCase(FALSE)
    , mRowIdToEnable(0)
    , mKeyStatesNum(0)
{
    mKeyStates = ArrayOf<Int32>::Alloc(CInputModeSwitcher::MAX_TOGGLE_STATES);
}

ECode CToggleStates::SetRowIdToEnable(
    /* [in] */ Int32 id)
{
    mRowIdToEnable = id;
    return NOERROR;
}

ECode CToggleStates::GetRowIdToEnable(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mRowIdToEnable;
    return NOERROR;
}

ECode CToggleStates::SetQwerty(
    /* [in] */ Boolean qwerty)
{
    mQwerty = qwerty;
    return NOERROR;
}

ECode CToggleStates::GetQwerty(
    /* [out] */ Boolean* qwerty)
{
    VALIDATE_NOT_NULL(qwerty);
    *qwerty = mQwerty;
    return NOERROR;
}

ECode CToggleStates::SetQwertyUpperCase(
    /* [in] */ Boolean qwertyUpperCase)
{
    mQwertyUpperCase = qwertyUpperCase;
    return NOERROR;
}

ECode CToggleStates::GetQwertyUpperCase(
    /* [out] */ Boolean* qwertyUpperCase)
{
    VALIDATE_NOT_NULL(qwertyUpperCase);
    *qwertyUpperCase = mQwertyUpperCase;
    return NOERROR;
}

ECode CToggleStates::SetKeyStatesNum(
    /* [in] */ Int32 keyStatesNum)
{
    mKeyStatesNum = keyStatesNum;
    return NOERROR;
}

ECode CToggleStates::GetKeyStatesNum(
    /* [out] */ Int32* keyStatesNum)
{
    VALIDATE_NOT_NULL(keyStatesNum);
    *keyStatesNum = mKeyStatesNum;
    return NOERROR;
}

ECode CToggleStates::SetKeyStates(
    /* [in] */ ArrayOf<Int32> * keyStates)
{
    mKeyStates = keyStates;
    return NOERROR;
}

ECode CToggleStates::GetKeyStates(
    /* [out, callee] */ ArrayOf<Int32> ** keyStates)
{
    VALIDATE_NOT_NULL(keyStates);
    *keyStates = mKeyStates;
    REFCOUNT_ADD(*keyStates);
    return NOERROR;
}


} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
