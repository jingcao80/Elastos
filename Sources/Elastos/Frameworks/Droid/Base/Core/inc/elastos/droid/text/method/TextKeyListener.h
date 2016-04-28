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
{
private:
    class NullKeyListener
        : public Object
        , public IKeyListener
    {
    public:
        CAR_INTERFACE_DECL()

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

        static AutoPtr<NullKeyListener> GetInstance();

    private:
        static AutoPtr<NullKeyListener> sInstance;
    };

    class SettingsObserver
        : public IContentObserver
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("TextKeyListener::SettingsObserver")

        SettingsObserver(
            /* [in] */ TextKeyListener* host);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange);

        //override IContentObserver
        CARAPI GetContentObserver(
            /* [out] */ IIContentObserver** observer);

        CARAPI ReleaseContentObserver(
            /* [out] */ IIContentObserver** oldObserver);

        CARAPI DeliverSelfNotifications(
            /* [out] */ Boolean* result);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 userId);

        CARAPI DispatchChange(
            /* [in] */ Boolean selfChange);

        CARAPI DispatchChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

        CARAPI DispatchChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 userId);

    private:
        TextKeyListener* mHost;
    };

public:
    friend class SettingsObserver;

    TextKeyListener();

    virtual ~TextKeyListener();

    CAR_INTERFACE_DECL()

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

    //override
    CARAPI ClearMetaKeyState(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 states);

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
