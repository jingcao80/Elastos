
#ifndef  __CTOGGLESTATES_H__
#define  __CTOGGLESTATES_H__

#include "_CToggleStates.h"

namespace Elastos {
namespace Droid {
namespace DevSamples {
namespace PinyinIME {

CarClass(CToggleStates)
{
public:
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

} // namespace PinyinIME
} // namespace DevSamples
} // namespace Droid
} // namespace Elastos

#endif  // __CTOGGLESTATES_H__
