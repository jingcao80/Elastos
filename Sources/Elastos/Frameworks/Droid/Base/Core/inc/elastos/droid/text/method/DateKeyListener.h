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

#ifndef __ELASTOS_DROID_TEXT_METHOD_DATEKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_DATEKEYLISTENER_H__

#include "elastos/droid/text/method/NumberKeyListener.h"

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * For entering dates in a text field.
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class DateKeyListener
    : public NumberKeyListener
    , public IDateKeyListener
{
public:
    CAR_INTERFACE_DECL()

    DateKeyListener();

    virtual ~DateKeyListener();

    CARAPI constructor();

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    static CARAPI GetCHARACTERS(
        /* [out] */ ArrayOf<Char32>** array);

    static AutoPtr<IDateKeyListener> GetInstance();

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
    virtual CARAPI_(AutoPtr< ArrayOf<Char32> >) GetAcceptedChars();

public:
    /**
     * The characters that are used.
     *
     * @see KeyEvent#getMatch
     * @see #getAcceptedChars
     */
    static const AutoPtr<ArrayOf<Char32> > CHARACTERS;// = new Char32[] { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '/', '-', '.' };
    static AutoPtr<IDateKeyListener> sInstance;
};


} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_DATEKEYLISTENER_H__
