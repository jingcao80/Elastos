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

#include "Elastos.Droid.View.h"
#include "elastos/droid/inputmethod/pinyin/KeyMapDream.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::CKeyEventHelper;
using Elastos::Droid::View::IKeyEventHelper;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

const Int32 KeyMapDream::SHIFT_FWCH;
const Int32 KeyMapDream::sKeyMap[] = {
    IKeyEvent::KEYCODE_UNKNOWN,
    IKeyEvent::KEYCODE_SOFT_LEFT,
    IKeyEvent::KEYCODE_SOFT_RIGHT,
    IKeyEvent::KEYCODE_HOME,
    IKeyEvent::KEYCODE_BACK,
    IKeyEvent::KEYCODE_CALL,
    IKeyEvent::KEYCODE_ENDCALL,
    IKeyEvent::KEYCODE_0 | (0xff09/*'\uff09'*/ << SHIFT_FWCH), // )
    IKeyEvent::KEYCODE_1 | (0xff01/*'\uff01'*/ << SHIFT_FWCH), // !
    IKeyEvent::KEYCODE_2 | (0xff20/*'\uff20'*/ << SHIFT_FWCH), // @
    IKeyEvent::KEYCODE_3 | (0xff03/*'\uff03'*/ << SHIFT_FWCH), // #
    IKeyEvent::KEYCODE_4 | (0xffe5/*'\uffe5'*/ << SHIFT_FWCH), // $ - fullwidth Yuan
    IKeyEvent::KEYCODE_5 | (0xff05/*'\uff05'*/ << SHIFT_FWCH), // %
    IKeyEvent::KEYCODE_6 | (0x2026/*'\u2026'*/ << SHIFT_FWCH), // ^ - Apostrophe
    IKeyEvent::KEYCODE_7 | (0xff06/*'\uff06'*/ << SHIFT_FWCH), // &
    IKeyEvent::KEYCODE_8 | (0xff0a/*'\uff0a'*/ << SHIFT_FWCH), // *
    IKeyEvent::KEYCODE_9 | (0xff08/*'\uff08'*/ << SHIFT_FWCH), // (
    IKeyEvent::KEYCODE_STAR,
    IKeyEvent::KEYCODE_POUND,
    IKeyEvent::KEYCODE_DPAD_UP,
    IKeyEvent::KEYCODE_DPAD_DOWN,
    IKeyEvent::KEYCODE_DPAD_LEFT,
    IKeyEvent::KEYCODE_DPAD_RIGHT,
    IKeyEvent::KEYCODE_DPAD_CENTER,
    IKeyEvent::KEYCODE_VOLUME_UP,
    IKeyEvent::KEYCODE_VOLUME_DOWN,
    IKeyEvent::KEYCODE_POWER,
    IKeyEvent::KEYCODE_CAMERA,
    IKeyEvent::KEYCODE_CLEAR,
    IKeyEvent::KEYCODE_A,
    IKeyEvent::KEYCODE_B | (0Xff3d/*'\uff3d'*/ << SHIFT_FWCH), // ]
    IKeyEvent::KEYCODE_C | (0X00a9/*'\u00a9'*/ << SHIFT_FWCH), // copyright
    IKeyEvent::KEYCODE_D | (0X3001/*'\u3001'*/ << SHIFT_FWCH), /* \\ */
    IKeyEvent::KEYCODE_E | ('_' << SHIFT_FWCH), // _
    IKeyEvent::KEYCODE_F | (0Xff5b/*'\uff5b'*/ << SHIFT_FWCH), // {
    IKeyEvent::KEYCODE_G | (0Xff5d/*'\uff5d'*/ << SHIFT_FWCH), // }
    IKeyEvent::KEYCODE_H | (0Xff1a/*'\uff1a'*/ << SHIFT_FWCH), // :
    IKeyEvent::KEYCODE_I | (0Xff0d/*'\uff0d'*/ << SHIFT_FWCH), // -
    IKeyEvent::KEYCODE_J | (0Xff1b/*'\uff1b'*/ << SHIFT_FWCH), // ;
    IKeyEvent::KEYCODE_K | (0X201c/*'\u201c'*/ << SHIFT_FWCH), // "
    IKeyEvent::KEYCODE_L | (0X2019/*'\u2019'*/ << SHIFT_FWCH), // '
    IKeyEvent::KEYCODE_M | (0X300b/*'\u300b'*/ << SHIFT_FWCH), // > - French quotes
    IKeyEvent::KEYCODE_N | (0X300a/*'\u300a'*/ << SHIFT_FWCH), // < - French quotes
    IKeyEvent::KEYCODE_O | (0Xff0b/*'\uff0b'*/ << SHIFT_FWCH), // +
    IKeyEvent::KEYCODE_P | (0Xff1d/*'\uff1d'*/ << SHIFT_FWCH), // =
    IKeyEvent::KEYCODE_Q | ('\t' << SHIFT_FWCH), // \t
    IKeyEvent::KEYCODE_R | (0X00ae/*'\u00ae'*/ << SHIFT_FWCH), // trademark
    IKeyEvent::KEYCODE_S | (0Xff5c/*'\uff5c'*/ << SHIFT_FWCH), // |
    IKeyEvent::KEYCODE_T | (0X20ac/*'\u20ac'*/ << SHIFT_FWCH), //
    IKeyEvent::KEYCODE_U | (0X00d7/*'\u00d7'*/ << SHIFT_FWCH), // multiplier
    IKeyEvent::KEYCODE_V | (0Xff3b/*'\uff3b'*/ << SHIFT_FWCH), // [
    IKeyEvent::KEYCODE_W | (0Xff40/*'\uff40'*/ << SHIFT_FWCH), // `
    IKeyEvent::KEYCODE_X, IKeyEvent::KEYCODE_Y | (0x00f7/*'\u00f7'*/ << SHIFT_FWCH),
    IKeyEvent::KEYCODE_Z,
    IKeyEvent::KEYCODE_COMMA | (0xff1f/*'\uff1f'*/ << SHIFT_FWCH),
    IKeyEvent::KEYCODE_PERIOD | (0xff0f/*'\uff0f'*/ << SHIFT_FWCH),
    IKeyEvent::KEYCODE_ALT_LEFT, IKeyEvent::KEYCODE_ALT_RIGHT,
    IKeyEvent::KEYCODE_SHIFT_LEFT, IKeyEvent::KEYCODE_SHIFT_RIGHT,
    IKeyEvent::KEYCODE_TAB, IKeyEvent::KEYCODE_SPACE, IKeyEvent::KEYCODE_SYM,
    IKeyEvent::KEYCODE_EXPLORER, IKeyEvent::KEYCODE_ENVELOPE,
    IKeyEvent::KEYCODE_ENTER, IKeyEvent::KEYCODE_DEL,
    IKeyEvent::KEYCODE_GRAVE, IKeyEvent::KEYCODE_MINUS,
    IKeyEvent::KEYCODE_EQUALS, IKeyEvent::KEYCODE_LEFT_BRACKET,
    IKeyEvent::KEYCODE_RIGHT_BRACKET, IKeyEvent::KEYCODE_BACKSLASH,
    IKeyEvent::KEYCODE_SEMICOLON, IKeyEvent::KEYCODE_APOSTROPHE,
    IKeyEvent::KEYCODE_SLASH,
    IKeyEvent::KEYCODE_AT | (0xff5e/*'\uff5e'*/ << SHIFT_FWCH),
    IKeyEvent::KEYCODE_NUM, IKeyEvent::KEYCODE_HEADSETHOOK,
    IKeyEvent::KEYCODE_FOCUS, IKeyEvent::KEYCODE_PLUS,
    IKeyEvent::KEYCODE_MENU, IKeyEvent::KEYCODE_NOTIFICATION,
    IKeyEvent::KEYCODE_SEARCH,};


Char32 KeyMapDream::GetChineseLabel(
    /* [in] */ Int32 keyCode)
{
    AutoPtr<IKeyEventHelper> helper;
    CKeyEventHelper::AcquireSingleton((IKeyEventHelper**)&helper);

    Int32 maxKeyCode = 0;
    helper->GetMaxKeyCode(&maxKeyCode);
    if (keyCode <= 0 || keyCode >= maxKeyCode) return 0;
    assert ((sKeyMap[keyCode] & 0x000000ff) == keyCode);
    return (Char32) (sKeyMap[keyCode] >> SHIFT_FWCH);
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
