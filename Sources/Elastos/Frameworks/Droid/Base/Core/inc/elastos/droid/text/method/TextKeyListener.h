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

#ifndef __ELASTOS_DROID_TEXT_METHOD_TEXTKEYLISTENER_H__
#define __ELASTOS_DROID_TEXT_METHOD_TEXTKEYLISTENER_H__

#include "elastos/droid/text/method/BaseKeyListener.h"
#include "elastos/droid/database/ContentObserver.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::IIContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Text {
namespace Method {

class TextKeyListener
    : public BaseKeyListener
    , public ITextKeyListener
    , public ISpanWatcher
    , public INoCopySpan
{
public:
    class NullKeyListener
        : public Object
        , public IKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI GetInputType(
            /* [out] */ Int32* inputType);

        CARAPI OnKeyDown(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        CARAPI OnKeyUp(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        CARAPI OnKeyOther(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* res);

        CARAPI ClearMetaKeyState(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 states);

        static AutoPtr<IKeyListener> GetInstance();

    private:
        static AutoPtr<IKeyListener> sInstance;
    };

    class SettingsObserver
        : public ContentObserver
    {
    public:
        TO_STRING_IMPL("TextKeyListener::SettingsObserver")

        SettingsObserver(
            /* [in] */ TextKeyListener* host);

        CARAPI constructor();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

    private:
        TextKeyListener* mHost;
    };

public:
    friend class SettingsObserver;

    CAR_INTERFACE_DECL()

    TextKeyListener();

    virtual ~TextKeyListener();

    CARAPI constructor(
        /* [in] */ Capitalize cap,
        /* [in] */ Boolean autotext);

    static CARAPI GetInstance(
        /* [in] */ Boolean autotext,
        /* [in] */ Capitalize cap,
        /* [out] */ ITextKeyListener** ret);

    static CARAPI GetInstance(
        /* [out] */ ITextKeyListener** ret);

    static CARAPI ShouldCap(
        /* [in] */ Capitalize cap,
        /* [in] */ ICharSequence* cs,
        /* [in] */ Int32 off,
        /* [out] */ Boolean* ret);

    CARAPI GetInputType(
        /* [out] */ Int32* ret);

    CARAPI OnKeyDown(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI OnKeyUp(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    CARAPI OnKeyOther(
        /* [in] */ IView* view,
        /* [in] */ IEditable* content,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* ret);

    static CARAPI Clear(
        /* [in] */ IEditable* e);

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

    CARAPI OnSpanChanged(
        /* [in] */ ISpannable* s,
        /* [in] */ IInterface* what,
        /* [in] */ Int32 start,
        /* [in] */ Int32 end,
        /* [in] */ Int32 st,
        /* [in] */ Int32 en);

    CARAPI ReleaseListener();

    /* package */
    CARAPI GetPrefs(
        /* [in] */ IContext* context,
        /* [out] */ Int32* ret);

private:
    CARAPI_(AutoPtr<IKeyListener>) GetKeyListener(
        /* [in] */ IKeyEvent* event);

    CARAPI_(void) InitPrefs(
        /* [in] */ IContext* context);

    CARAPI_(void) UpdatePrefs(
        /* [in] */ IContentResolver* resolver);

public:
    static const AutoPtr<IInterface> ACTIVE;
    static const AutoPtr<IInterface> CAPPED;
    static const AutoPtr<IInterface> INHIBIT_REPLACEMENT;
    static const AutoPtr<IInterface> LAST_TYPED;

public:
    static const Int32 AUTO_CAP;
    static const Int32 AUTO_TEXT;
    static const Int32 AUTO_PERIOD;
    static const Int32 SHOW_PASSWORD;

protected:
    static const Int32 CAPITALIZELENGTH;;
    static AutoPtr<ArrayOf<ITextKeyListener*> > sInstance;

protected:
    Object mLock;

private:
    Capitalize mAutoCap;
    Boolean mAutoText;
    Int32 mPrefs;
    Boolean mPrefsInited;
    AutoPtr<IWeakReference> mResolver;
    AutoPtr<SettingsObserver> mObserver;
};

} // namespace Method
} // namespace Text
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_TEXT_METHOD_TEXTKEYLISTENER_H__
