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
#include "Elastos.Droid.View.h"
#include "Elastos.CoreLibrary.Core.h"
#include "elastos/droid/inputmethod/pinyin/EnglishInputProcessor.h"

using Elastos::Core::ICharSequence;
using Elastos::Core::CString;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

EnglishInputProcessor::EnglishInputProcessor()
    : mLastKeyCode(IKeyEvent::KEYCODE_UNKNOWN)
{}

Boolean EnglishInputProcessor::ProcessKey(
    /* [in] */ IInputConnection* inputContext,
    /* [in] */ IKeyEvent* event,
    /* [in] */ Boolean upperCase,
    /* [in] */ Boolean realAction)
{
    if (NULL == inputContext || NULL == event) return FALSE;

    Int32 keyCode = 0;
    event->GetKeyCode(&keyCode);

    AutoPtr<ICharSequence> prefix;
    inputContext->GetTextBeforeCursor(2, 0, (ICharSequence**)&prefix);

    Int32 keyChar = 0;
    if (keyCode >= IKeyEvent::KEYCODE_A && keyCode <= IKeyEvent::KEYCODE_Z) {
        keyChar = keyCode - IKeyEvent::KEYCODE_A + 'a';
        if (upperCase) {
            keyChar = keyChar + 'A' - 'a';
        }
    }
    else if (keyCode >= IKeyEvent::KEYCODE_0
            && keyCode <= IKeyEvent::KEYCODE_9) {
        keyChar = keyCode - IKeyEvent::KEYCODE_0 + '0';
    }
    else if (keyCode == IKeyEvent::KEYCODE_COMMA) {
        keyChar = ',';
    }
    else if (keyCode == IKeyEvent::KEYCODE_PERIOD) {
        keyChar = '.';
    }
    else if (keyCode == IKeyEvent::KEYCODE_APOSTROPHE) {
        keyChar = '\'';
    }
    else if (keyCode == IKeyEvent::KEYCODE_AT) {
        keyChar = '@';
    }
    else if (keyCode == IKeyEvent::KEYCODE_SLASH) keyChar = '/';

    if (0 == keyChar) {
        mLastKeyCode = keyCode;

        String insert;
        if (IKeyEvent::KEYCODE_DEL == keyCode) {
            if (realAction)  {
                Boolean flag = FALSE;
                inputContext->DeleteSurroundingText(1, 0, &flag);
            }
        }
        else if (IKeyEvent::KEYCODE_ENTER == keyCode) {
            insert = "\n";
        }
        else if (IKeyEvent::KEYCODE_SPACE == keyCode) {
            insert = " ";
        }
        else {
            return FALSE;
        }

        if (!insert.IsNull() && realAction) {
            AutoPtr<ICharSequence> text;
            CString::New(insert, (ICharSequence**)&text);
            Boolean flag = FALSE;
            inputContext->CommitText(text, insert.GetLength(), &flag);
        }

        return TRUE;
    }

    if (!realAction) {
        return TRUE;
    }

    if (IKeyEvent::KEYCODE_SHIFT_LEFT == mLastKeyCode
            || IKeyEvent::KEYCODE_SHIFT_LEFT == mLastKeyCode) {
        if (keyChar >= 'a' && keyChar <= 'z') {
            keyChar = keyChar - 'a' + 'A';
        }
    }
    else if (IKeyEvent::KEYCODE_ALT_LEFT == mLastKeyCode) {
    }

    String result("");
    result.Append((Char32)keyChar);

    Boolean flag = FALSE;
    AutoPtr<ICharSequence> text;
    CString::New(result, (ICharSequence**)&text);
    inputContext->CommitText(text, result.GetLength(), &flag);
    mLastKeyCode = keyCode;
    return TRUE;
}

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos
