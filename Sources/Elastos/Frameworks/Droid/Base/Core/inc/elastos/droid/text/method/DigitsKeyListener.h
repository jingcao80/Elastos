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

#ifndef __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__

#include "elastos/droid/text/method/NumberKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For digits-only text entry
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class DigitsKeyListener
    : public NumberKeyListener
    , public IDigitsKeyListener
{
public:

    CAR_INTERFACE_DECL()

    /**
     * Allocates a DigitsKeyListener that accepts the digits 0 through 9.
     */
    DigitsKeyListener();

    virtual ~DigitsKeyListener();

    CARAPI constructor();

    /**
     * Allocates a DigitsKeyListener that accepts the digits 0 through 9,
     * plus the minus sign (only at the beginning) and/or decimal point
     * (only one per field) if specified.
     */
    CARAPI constructor(
        /* [in] */ Boolean sign,
        /* [in] */ Boolean decimal);

    static CARAPI GetInstance(
        /* [out] */ IDigitsKeyListener** ret);

    static CARAPI GetInstance(
        /* [in] */ Boolean sign,
        /* [in] */ Boolean decimal,
        /* [out] */ IDigitsKeyListener** ret);

    static CARAPI GetInstance(
        /* [in] */ const String& accepted,
        /* [out] */ IDigitsKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    //@Override
    CARAPI Filter(
        /* [in] */ ICharSequence* source,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ ISpanned* dest,
        /* [in] */ Int32 dstart,
        /* [in] */ Int32 dend,
        /* [out] */ ICharSequence** ret);

    //override
    CARAPI OnKeyUp(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI ClearMetaKeyState(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 states);

protected:
    //@Override
    CARAPI_(AutoPtr< ArrayOf<Char32> >) GetAcceptedChars();


protected:
    static const Int32 SIGN;// = 1;
    static const Int32 DECIMAL;// = 2;
    /**
     * The characters that are used.
     *
     * @see KeyEvent#getMatch
     * @see #getAcceptedChars
     */
    AutoPtr< ArrayOf<Char32> > mAccepted;

private:
    static CARAPI_(Boolean) IsSignChar(
        /* [in] */ const Char32 c);

    static CARAPI_(Boolean) IsDecimalPointChar(
        /* [in] */ const Char32 c);

private:
    Boolean mSign;
    Boolean mDecimal;

    static const Char32* CHARACTERS[];

    static const Char32 CHARACTERS0[];
    static const Char32 CHARACTERS1[];
    static const Char32 CHARACTERS2[];
    static const Char32 CHARACTERS3[];

    static AutoPtr< ArrayOf< IDigitsKeyListener* > > sInstance;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_DIGITSKEYLISTENER_H__
