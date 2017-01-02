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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_CTOGGLESTATES_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_CTOGGLESTATES_H__

#include "_CToggleStates.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

CarClass(CToggleStates)
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CToggleStates();

    CARAPI SetRowIdToEnable(
        /* [in] */ Int32 id);

    CARAPI GetRowIdToEnable(
        /* [out] */ Int32* id);

    CARAPI SetQwerty(
        /* [in] */ Boolean qwerty);

    CARAPI GetQwerty(
        /* [out] */ Boolean* qwerty);

    CARAPI SetQwertyUpperCase(
        /* [in] */ Boolean qwertyUpperCase);

    CARAPI GetQwertyUpperCase(
        /* [out] */ Boolean* qwertyUpperCase);

    CARAPI SetKeyStatesNum(
        /* [in] */ Int32 keyStatesNum);

    CARAPI GetKeyStatesNum(
        /* [out] */ Int32* keyStatesNum);

    CARAPI SetKeyStates(
        /* [in] */ ArrayOf<Int32> * keyStates);

    CARAPI GetKeyStates(
        /* [out, callee] */ ArrayOf<Int32> ** keyStates);
fdfd
public:
    /**
     * If it is true, this soft keyboard is a QWERTY one.
     */
    Boolean mQwerty;

    /**
     * If {@link #mQwerty} is true, this variable is used to decide the
     * letter case of the QWERTY keyboard.
     */
    Boolean mQwertyUpperCase;

    /**
     * The id of enabled row in the soft keyboard. Refer to
     * {@link com.android.inputmethod.pinyin.SoftKeyboard.KeyRow} for
     * details.
     */
    Int32 mRowIdToEnable;

    /**
     * Used to store all other toggle states for the current input mode.
     */
    AutoPtr<ArrayOf<Int32> > mKeyStates;

    /**
     * Number of states to toggle.
     */
    Int32 mKeyStatesNum;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_CTOGGLESTATES_H__
