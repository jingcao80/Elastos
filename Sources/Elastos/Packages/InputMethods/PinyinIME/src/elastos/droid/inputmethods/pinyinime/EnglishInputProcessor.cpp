
#include "EnglishInputProcessor.h"
#include <elastos/core/StringBuilder.h>

using Elastos::Core::ICharSequence;
using Elastos::Core::StringBuilder;
using Elastos::Core::CStringWrapper;

namespace Elastos {
namespace Droid {
namespace Inputmethods {
namespace PinyinIME {

EnglishInputProcessor::EnglishInputProcessor()
    : mLastKeyCode(IKeyEvent::KEYCODE_UNKNOWN)
{
}

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
    } else if (keyCode >= IKeyEvent::KEYCODE_0
            && keyCode <= IKeyEvent::KEYCODE_9)
        keyChar = keyCode - IKeyEvent::KEYCODE_0 + '0';
    else if (keyCode == IKeyEvent::KEYCODE_COMMA)
        keyChar = ',';
    else if (keyCode == IKeyEvent::KEYCODE_PERIOD)
        keyChar = '.';
    else if (keyCode == IKeyEvent::KEYCODE_APOSTROPHE)
        keyChar = '\'';
    else if (keyCode == IKeyEvent::KEYCODE_AT)
        keyChar = '@';
    else if (keyCode == IKeyEvent::KEYCODE_SLASH) keyChar = '/';

    if (0 == keyChar) {
        mLastKeyCode = keyCode;

        String insert;
        if (IKeyEvent::KEYCODE_DEL == keyCode) {
            if (realAction)  {
                Boolean flag = FALSE;
                inputContext->DeleteSurroundingText(1, 0, &flag);
            }
        } else if (IKeyEvent::KEYCODE_ENTER == keyCode) {
            insert = String("\n");
        } else if (IKeyEvent::KEYCODE_SPACE == keyCode) {
            insert = String(" ");
        } else {
            return FALSE;
        }

        if (NULL != insert && realAction) {
            Boolean flag = FALSE;
            AutoPtr<ICharSequence> text;
            CStringWrapper::New(insert, (ICharSequence**)&text);
            inputContext->CommitText(text, insert.GetLength(), &flag);
        }

        return TRUE;
    }

    if (!realAction)
        return TRUE;

    if (IKeyEvent::KEYCODE_SHIFT_LEFT == mLastKeyCode
            || IKeyEvent::KEYCODE_SHIFT_LEFT == mLastKeyCode) {
        if (keyChar >= 'a' && keyChar <= 'z')
            keyChar = keyChar - 'a' + 'A';
    } else if (IKeyEvent::KEYCODE_ALT_LEFT == mLastKeyCode) {
    }

    StringBuilder builder;
    builder.AppendChar(keyChar);
    String result = builder.ToString();

    Boolean flag = FALSE;
    AutoPtr<ICharSequence> text;
    CStringWrapper::New(result, (ICharSequence**)&text);
    inputContext->CommitText(text, result.GetLength(), &flag);
    mLastKeyCode = keyCode;
    return TRUE;
}

} // namespace PinyinIME
} // namespace Inputmethods
} // namespace Droid
} // namespace Elastos
