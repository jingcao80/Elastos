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

#ifndef __ELASTOS_DROID_TEXT_METHOD_MULTITAPKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_MULTITAPKEYLISTENER_H__

#include "elastos/droid/text/method/BaseKeyListener.h"
#include "elastos/droid/os/HandlerRunnable.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::HandlerRunnable;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

/**
 * This is the standard key listener for alphabetic input on 12-key
 * keyboards.  You should generally not need to instantiate this yourself;
 * TextKeyListener will do it for you.
 * <p></p>
 * As for all implementations of {@link KeyListener}, this class is only concerned
 * with hardware keyboards.  Software input methods have no obligation to trigger
 * the methods in this class.
 */
class MultiTapKeyListener
    : public BaseKeyListener
    , public IMultiTapKeyListener
    , public ISpanWatcher
    , public INoCopySpan
{
    friend class TextKeyListener;

private:
    class Timeout
        : public HandlerRunnable
    {
    friend class MultiTapKeyListener;

    public:
        TO_STRING_IMPL("MultiTapKeyListener::Timeout")

        Timeout(
            /* [in] */ IEditable* buffer);

        CARAPI constructor();

        CARAPI Run();

    private:
        AutoPtr<IEditable> mBuffer;
    };

public:

    MultiTapKeyListener();

    virtual ~MultiTapKeyListener();

    CAR_INTERFACE_DECL()

    CARAPI constructor(
        /* [in] */ Capitalize cap,
        /* [in] */ Boolean autotext);

    static CARAPI GetInstance(
        /* [in] */ Boolean autotext,
        /* [in] */ Capitalize cap,
        /* [out] */ IMultiTapKeyListener** ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI OnSpanChanged(
        /* [in] */ ISpannable* buf,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 s,
        /* [in] */ Int32 e,
        /* [in] */ Int32 start,
        /* [in] */ Int32 stop);

    CARAPI OnSpanAdded(
        /* [in] */ ISpannable* s,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

    CARAPI OnSpanRemoved(
        /* [in] */ ISpannable* s,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end);

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

private:
    static CARAPI_(void) RemoveTimeouts(
        /* [in] */ ISpannable* buf);

protected://private
    //private static final SparseArray<String> sRecs = new SparseArray<String>();
    static AutoPtr<HashMap<Int32, String> > sRecs;
    Capitalize mCapitalize;
    Boolean mAutoText;

private:
    static AutoPtr< ArrayOf< IMultiTapKeyListener* > > sInstance;// = new MultiTapKeyListener[4];

public:
    static const Int32 CAPITALIZELENGTH;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_TEXT_METHOD_MULTITAPKEYLISTENER_H__
