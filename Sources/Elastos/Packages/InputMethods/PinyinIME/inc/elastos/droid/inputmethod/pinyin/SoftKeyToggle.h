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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEYTOGGLE_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEYTOGGLE_H__

#include "_Elastos.Droid.InputMethod.Pinyin.h"
#include "elastos/droid/inputmethod/pinyin/SoftKey.h"

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class for soft keys which defined in the keyboard xml file. A soft key can be
 * a basic key or a toggling key.
 *
 * @see com.android.inputmethod.pinyin.SoftKey
 */
class SoftKeyToggle
    : public SoftKey
    , public ISoftKeyToggle
{
public:
    class ToggleState : public Object
    {
        friend class SoftKeyToggle;

    public:
        ToggleState();

        CARAPI_(void) SetStateId(
            /* [in] */ Int32 stateId);

        CARAPI_(void) SetStateFlags(
            /* [in] */ Boolean repeat,
            /* [in] */ Boolean balloon);

    public:
        AutoPtr<SoftKeyType> mKeyType;
        Int32 mKeyCode;
        AutoPtr<IDrawable> mKeyIcon;
        AutoPtr<IDrawable> mKeyIconPopup;
        String mKeyLabel;
        AutoPtr<ToggleState> mNextState;

    private:
        // The id should be bigger than 0;
        Int32 mIdAndFlags;
    };

public:
    CAR_INTERFACE_DECL();

    CARAPI_(Int32) GetToggleStateId();

    // The state id should be valid, and less than 255.
    // If resetIfNotFound is TRUE and there is no such toggle state with the
    // given id, the key state will be reset.
    // If the key state is newly changed (enabled to the given state, or
    // reseted) and needs re-draw, return TRUE.
    CARAPI_(Boolean) EnableToggleState(
        /* [in] */ Int32 stateId,
        /* [in] */ Boolean resetIfNotFound);

    // The state id should be valid, and less than 255.
    // If resetIfNotFound is TRUE and there is no such toggle state with the
    // given id, the key state will be reset.
    // If the key state is newly changed and needs re-draw, return TRUE.
    CARAPI_(Boolean) DisableToggleState(
        /* [in] */ Int32 stateId,
        /* [in] */ Boolean resetIfNotFound);

    // Clear any toggle state. If the key needs re-draw, return TRUE.
    CARAPI_(Boolean) DisableAllToggleStates();

    // @Override
    CARAPI_(AutoPtr<IDrawable>) GetKeyIcon();

    // @Override
    CARAPI_(AutoPtr<IDrawable>) GetKeyIconPopup();

    // @Override
    CARAPI_(Int32) GetKeyCode();

    // @Override
    CARAPI_(String) GetKeyLabel();

    // @Override
    CARAPI_(AutoPtr<IDrawable>) GetKeyBg();

    // @Override
    CARAPI_(AutoPtr<IDrawable>) GetKeyHlBg();

    // @Override
    CARAPI_(Int32) GetColor();

    // @Override
    CARAPI_(Int32) GetColorHl();

    // @Override
    CARAPI_(Int32) GetColorBalloon();

    // @Override
    CARAPI_(Boolean) IsKeyCodeKey();

    // @Override
    CARAPI_(Boolean) IsUserDefKey();

    // @Override
    CARAPI_(Boolean) IsUniStrKey();

    // @Override
    CARAPI_(Boolean) NeedBalloon();

    // @Override
    CARAPI_(Boolean) Repeatable();

    // @Override
    CARAPI_(void) ChangeCase(
        /* [in] */ Boolean lowerCase);

    CARAPI_(AutoPtr<ToggleState>) CreateToggleState();

    CARAPI_(Boolean) SetToggleStates(
        /* [in] */ ToggleState* rootState);

private:
    CARAPI_(AutoPtr<ToggleState>) GetToggleState();

private:
    /**
     * The current state number is stored in the lowest 8 bits of mKeyMask, this
     * mask is used to get the state number. If the current state is 0, the
     * normal state is enabled; if the current state is more than 0, a toggle
     * state in the toggle state chain will be enabled.
     */
    static const Int32 KEYMASK_TOGGLE_STATE;

    AutoPtr<ToggleState> mToggleState;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEYTOGGLE_H__
