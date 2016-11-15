
#ifndef __ELASTOS_DROID_CONTENT_CCLIPBOARDMANAGER_H__
#define __ELASTOS_DROID_CONTENT_CCLIPBOARDMANAGER_H__

#include "_Elastos_Droid_Content_CClipboardManager.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Os::Handler;
using Elastos::Core::ICharSequence;
using Elastos::Utility::Etl::List;

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CClipboardManager)
    , public Object // public Elastos::Droid::Text::ClipboardManager
    , public IClipboardManager
{
private:

    class MyHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("CClipboardManager::Handler")

        MyHandler(
            /* [in] */ CClipboardManager* host)
            : mHost(host)
        {}

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        CClipboardManager* mHost;
    };

    class PrimaryClipChangedServiceListener
        : public Object
        , public IOnPrimaryClipChangedListener
    {
    public:
        CAR_INTERFACE_DECL()

        PrimaryClipChangedServiceListener(
            /* [in] */ CClipboardManager* context);

        ~PrimaryClipChangedServiceListener();


        CARAPI DispatchPrimaryClipChanged();

    private:
        CClipboardManager* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CClipboardManager();

    virtual ~CClipboardManager();

    /**
     * Sets the current primary clip on the clipboard.  This is the clip that
     * is involved in normal cut and paste operations.
     *
     * @param clip The clipped data item to set.
     */
    CARAPI SetPrimaryClip(
        /* [in] */ IClipData* clip);

    /**
     * Returns the current primary clip on the clipboard.
     */
    CARAPI GetPrimaryClip(
        /* [out] */ IClipData** clipData);

    /**
     * Returns a description of the current primary clip on the clipboard
     * but not a copy of its data.
     */
    CARAPI GetPrimaryClipDescription(
        /* [out] */ IClipDescription** clipDescription);

    /**
     * Returns true if there is currently a primary clip on the clipboard.
     */
    CARAPI HasPrimaryClip(
        /* [out] */ Boolean* hasClip);

    CARAPI AddPrimaryClipChangedListener(
        /* [in] */ IClipboardManagerOnPrimaryClipChangedListener* what);

    CARAPI RemovePrimaryClipChangedListener(
        /* [in] */ IClipboardManagerOnPrimaryClipChangedListener* what);

    /**
     * Returns the text on the clipboard.  It will eventually be possible
     * to store types other than text too, in which case this will return
     * null if the type cannot be coerced to text.
     */
    CARAPI GetText(
        /* [out] */ ICharSequence** text);

    /**
     * Sets the contents of the clipboard to the specified text.
     */
    CARAPI SetText(
        /* [in] */ ICharSequence* text);

    /**
     * Returns true if the clipboard contains text; false otherwise.
     */
    CARAPI HasText(
        /* [out] */ Boolean* hasText);

    /** {@hide} */
    CARAPI constructor(
        /* [in] */ IContext* context);

private:
    friend class PrimaryClipChangedServiceListener;

    static CARAPI GetService(
        /* [out] */ IIClipboard** clipboard);

    CARAPI ReportPrimaryClipChanged();

private:
    static AutoPtr<IIClipboard> sService;
    static Object sStaticLock;
    static const Int32 MSG_REPORT_PRIMARY_CLIP_CHANGED;

private:
    IContext* mContext;
    List<AutoPtr<IClipboardManagerOnPrimaryClipChangedListener> > mPrimaryClipChangedListeners;
    AutoPtr<IOnPrimaryClipChangedListener> mPrimaryClipChangedServiceListener;
    Object mPrimaryClipChangedListenersLock;
    AutoPtr<MyHandler> mHandler;
};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CCLIPBOARDMANAGER_H__
