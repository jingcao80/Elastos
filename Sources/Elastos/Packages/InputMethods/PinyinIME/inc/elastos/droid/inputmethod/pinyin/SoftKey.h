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

#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEY_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEY_H__

#include "Elastos.Droid.Graphics.h"
#include "_Elastos.Droid.InputMethod.Pinyin.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

class SoftKeyType : public Object
{
public:
    SoftKeyType(
        /* [in] */ Int32 id,
        /* [in] */ IDrawable* bg,
        /* [in] */ IDrawable* hlBg);

    CARAPI_(void) SetColors(
        /* [in] */ Int32 color,
        /* [in] */ Int32 colorHl,
        /* [in] */ Int32 colorBalloon);

public:
    static const Int32 KEYTYPE_ID_NORMAL_KEY = 0;

    Int32 mKeyTypeId;
    AutoPtr<IDrawable> mKeyBg;
    AutoPtr<IDrawable> mKeyHlBg;
    Int32 mColor;
    Int32 mColorHl;
    Int32 mColorBalloon;
};

/**
 * Class for soft keys which defined in the keyboard xml file. A soft key can be
 * a basic key or a toggling key.
 *
 * @see com.android.inputmethod.pinyin.SoftKeyToggle
 */
class SoftKey : public Object
{
    friend class CSkbContainer;
    friend class CSoftKeyboardView;
    friend class SoftKeyboard;

public:
    SoftKey();

    CARAPI_(void) SetKeyType(
        /* [in] */ SoftKeyType* keyType,
        /* [in] */ IDrawable* keyIcon,
        /* [in] */ IDrawable* keyIconPopup);

    // The caller guarantees that all parameters are in [0, 1]
    CARAPI_(void) SetKeyDimensions(
        /* [in] */ Float left,
        /* [in] */ Float top,
        /* [in] */ Float right,
        /* [in] */ Float bottom);

    CARAPI_(void) SetKeyAttribute(
        /* [in] */ Int32 keyCode,
        /* [in] */ const String& label,
        /* [in] */ Boolean repeat,
        /* [in] */ Boolean balloon);

    CARAPI_(void) SetPopupSkbId(
        /* [in] */ Int32 popupSkbId);

    // Call after setKeyDimensions(). The caller guarantees that the
    // keyboard with and height are valid.
    CARAPI_(void) SetSkbCoreSize(
        /* [in] */ Int32 skbWidth,
        /* [in] */ Int32 skbHeight);

    virtual CARAPI_(AutoPtr<IDrawable>) GetKeyIcon();

    virtual CARAPI_(AutoPtr<IDrawable>) GetKeyIconPopup();

    virtual CARAPI_(Int32) GetKeyCode();

    virtual CARAPI_(String) GetKeyLabel();

    virtual CARAPI_(void) ChangeCase(
        /* [in] */ Boolean upperCase);

    virtual CARAPI_(AutoPtr<IDrawable>) GetKeyBg();

    virtual CARAPI_(AutoPtr<IDrawable>) GetKeyHlBg();

    virtual CARAPI_(Int32) GetColor();

    virtual CARAPI_(Int32) GetColorHl();

    virtual CARAPI_(Int32) GetColorBalloon();

    virtual CARAPI_(Boolean) IsKeyCodeKey();

    virtual CARAPI_(Boolean) IsUserDefKey();

    virtual CARAPI_(Boolean) IsUniStrKey();

    virtual CARAPI_(Boolean) NeedBalloon();

    virtual CARAPI_(Boolean) Repeatable();

    CARAPI_(Int32) GetPopupResId();

    CARAPI_(Int32) GetWidth();

    CARAPI_(Int32) GetHeight();

    CARAPI_(Boolean) MoveWithinKey(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y);

    CARAPI ToString(
        /* [out] */ String* info);

public:
    /**
     * For a finger touch device, after user presses a key, there will be some
     * consequent moving events because of the changing in touching pressure. If
     * the moving distance in x is within this threshold, the moving events will
     * be ignored.
     */
    static const Int32 MAX_MOVE_TOLERANCE_X;

    /**
     * For a finger touch device, after user presses a key, there will be some
     * consequent moving events because of the changing in touching pressure. If
     * the moving distance in y is within this threshold, the moving events will
     * be ignored.
     */
    static const Int32 MAX_MOVE_TOLERANCE_Y;

    /**
     * If this value is not 0, this key can be used to popup a sub soft keyboard
     * when user presses it for some time.
     */
    Int32 mPopupSkbId;

    Float mLeftF;
    Float mRightF;
    Float mTopF;
    Float mBottomF;
    Int32 mLeft;
    Int32 mRight;
    Int32 mTop;
    Int32 mBottom;

protected:
    static const Int32 KEYMASK_REPEAT;
    static const Int32 KEYMASK_BALLOON;

    /**
     * Used to indicate the type and attributes of this key. the lowest 8 bits
     * should be reserved for SoftkeyToggle.
     */
    Int32 mKeyMask;

    AutoPtr<SoftKeyType> mKeyType;

    AutoPtr<IDrawable> mKeyIcon;

    AutoPtr<IDrawable> mKeyIconPopup;

    String mKeyLabel;

    Int32 mKeyCode;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_SOFTKEY_H__
