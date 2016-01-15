#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/ui/autofill/AutofillPopup.h"
#include "elastos/droid/webkit/native/ui/autofill/AutofillSuggestion.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Webkit::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Ui::Autofill::AutofillPopup;
using Elastos::Droid::Webkit::Ui::Autofill::AutofillSuggestion;

// import org.chromium.base.CalledByNative;
// import org.chromium.base.JNINamespace;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Java counterpart to the AwAutofillClient. This class is owned by AwContents and has
 * a weak reference from native side.
 */
//@JNINamespace("android_webview")
class AwAutofillClient
    : public Object
{
private:
    class InnerAutofillPopupDelegate
        : public AutofillPopup::AutofillPopupDelegate
    {
    public:
        InnerAutofillPopupDelegate(
            /* [in] */ AwAutofillClient* owner);

        //@Override
        CARAPI RequestHide();

        //@Override
        CARAPI SuggestionSelected(
            /* [in] */ Int32 listIndex);

    private:
        AwAutofillClient* mOwner;
    };

public:
    //@CalledByNative AwAutofillClient
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ Int64 nativeClient);

    virtual CARAPI_(void) Init(
        /* [in] */ ContentViewCore* contentViewCore);

    //@CalledByNative
    virtual CARAPI_(void) HideAutofillPopup();

private:
    AwAutofillClient(
        /* [in] */ Int64 nativeAwAutofillClient);

    //@CalledByNative
    CARAPI_(void) ShowAutofillPopup(
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float width,
        /* [in] */ Float height,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    //@CalledByNative ArrayOf AutofillSuggestion
    static CARAPI_(AutoPtr<ArrayOf<IInterface*> >) CreateAutofillSuggestionArray(
        /* [in] */ Int32 size);

    /**
     * @param array AutofillSuggestion array that should get a new suggestion added.
     * @param index Index in the array where to place a new suggestion.
     * @param name Name of the suggestion.
     * @param label Label of the suggestion.
     * @param uniqueId Unique suggestion id.
     */
    //@CalledByNative ArrayOf AutofillSuggestion
    static CARAPI_(void) AddToAutofillSuggestionArray(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ Int32 index,
        /* [in] */ const String& name,
        /* [in] */ const String& label,
        /* [in] */ Int32 uniqueId);

    CARAPI_(void) NativeSuggestionSelected(
        /* [in] */ Int64 nativeAwAutofillClient,
        /* [in] */ Int32 position);

//callback function declaration
public:
    static CARAPI_(void*) ElaAwAutofillClientCallback_Init();

private:
    static CARAPI_(void) ShowAutofillPopup(
        /* [in] */ IInterface* obj,
        /* [in] */ Float x,
        /* [in] */ Float y,
        /* [in] */ Float width,
        /* [in] */ Float height,
        /* [in] */ ArrayOf<IInterface*>* suggestions);

    static CARAPI_(void) HideAutofillPopup(
        /* [in] */ IInterface* obj);

private:
    const Int64 mNativeAwAutofillClient;
    AutoPtr<AutofillPopup> mAutofillPopup;
    AutoPtr<IViewGroup> mContainerView;
    AutoPtr<ContentViewCore> mContentViewCore;
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
