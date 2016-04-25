
#ifndef __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__
#define __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__

#include <elastos/core/Object.h>

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::InputMethod::IInputConnection;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace InputMethod {
namespace Pinyin {

/**
 * Class to handle English input.
 */
class EnglishInputProcessor: public Object
{
public:
    EnglishInputProcessor();

    CARAPI_(Boolean) ProcessKey(
        /* [in] */ IInputConnection* inputContext,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean upperCase,
        /* [in] */ Boolean realAction);

private:
    Int32 mLastKeyCode;
};

} // namespace Pinyin
} // namespace InputMethod
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHOD_PINYIN_ENGLISHINPUTPROCESSOR_H__
