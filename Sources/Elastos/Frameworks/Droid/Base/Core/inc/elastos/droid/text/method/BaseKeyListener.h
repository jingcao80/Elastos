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

#ifndef __ELASTOS_DROID_TEXT_METHOD_BASEKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_BASEKEYLISTENER_H__

#include "Elastos.Droid.Text.h"
#include "elastos/droid/text/method/MetaKeyKeyListener.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Text::IEditable;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * Abstract base class for key listeners.
 *
 * Provides a basic foundation for entering and editing text.
 * Subclasses should override {@link #onKeyDown} and {@link #onKeyUp} to insert
 * characters as keys are pressed.
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class ECO_PUBLIC BaseKeyListener
    : public MetaKeyKeyListener
    , public IKeyListener
{
public:
    CAR_INTERFACE_DECL()

    BaseKeyListener();

    virtual ~BaseKeyListener();

    /**
     * Performs the action that happens when you press the {@link KeyEvent#KEYCODE_DEL} key in
     * a {@link TextView}.  If there is a selection, deletes the selection; otherwise,
     * deletes the character before the cursor, if any; ALT+DEL deletes everything on
     * the line the cursor is on.
     *
     * @return true if anything was deleted; false otherwise.
     */
    CARAPI Backspace(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    /**
     * Performs the action that happens when you press the {@link KeyEvent#KEYCODE_FORWARD_DEL}
     * key in a {@link TextView}.  If there is a selection, deletes the selection; otherwise,
     * deletes the character before the cursor, if any; ALT+FORWARD_DEL deletes everything on
     * the line the cursor is on.
     *
     * @return true if anything was deleted; false otherwise.
     */
    CARAPI ForwardDelete(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    static CARAPI MakeTextContentType(
        /* [in] */ Capitalize caps,
        /* [in] */ Boolean autoText,
        /* [out] */ Int32* ret);

    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    /**
     * Base implementation handles ACTION_MULTIPLE KEYCODE_UNKNOWN by inserting
     * the event's text into the content.
     */
    CARAPI OnKeyOther(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    using MetaKeyKeyListener::ClearMetaKeyState;

    CARAPI ClearMetaKeyState(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 states);

private:
    CARAPI_(Boolean) BackspaceOrForwardDelete(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [in] */ Boolean isForwardDelete);

    CARAPI_(Boolean) DeleteSelection(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content);

    CARAPI_(Boolean) DeleteLine(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content);

public:
    /* package */
    static const AutoPtr<IInterface> OLD_SEL_START;// = new NoCopySpan.Concrete();

};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_BASEKEYLISTENER_H__
