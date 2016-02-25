#include "elastos/droid/webkit/native/android_webview/AwAutofillClient.h"
#include "elastos/droid/webkit/native/android_webview/api/AwAutofillClient_dec.h"

#include <elastos/utility/logging/Logger.h>
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

//===============================================================
//         AwAutofillClient::InnerAutofillPopupDelegate
//===============================================================

AwAutofillClient::InnerAutofillPopupDelegate::InnerAutofillPopupDelegate(
    /* [in] */ AwAutofillClient* owner)
    : mOwner(owner)
{
}

//@Override
ECode AwAutofillClient::InnerAutofillPopupDelegate::RequestHide()
{
    return NOERROR;
}

//@Override
ECode AwAutofillClient::InnerAutofillPopupDelegate::SuggestionSelected(
    /* [in] */ Int32 listIndex)
{
    mOwner->NativeSuggestionSelected(mOwner->mNativeAwAutofillClient, listIndex);
    return NOERROR;
}

//===============================================================
//                      AwAutofillClient
//===============================================================

AwAutofillClient::AwAutofillClient(
    /* [in] */ Int64 nativeAwAutofillClient)
    : mNativeAwAutofillClient(nativeAwAutofillClient)
{
}

//@CalledByNative AwAutofillClient
AutoPtr<IInterface> AwAutofillClient::Create(
    /* [in] */ Int64 nativeClient)
{
    AutoPtr<AwAutofillClient> client = new AwAutofillClient(nativeClient);
    AutoPtr<IInterface> result = TO_IINTERFACE(client);
    return result;
}

void AwAutofillClient::Init(
    /* [in] */ ContentViewCore* contentViewCore)
{
    mContentViewCore = contentViewCore;
    mContainerView = contentViewCore->GetContainerView();
}

//@CalledByNative
void AwAutofillClient::ShowAutofillPopup(
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    if (mContentViewCore == NULL) return;

    if (mAutofillPopup == NULL) {
        AutoPtr<AutofillPopup::AutofillPopupDelegate> delegate = new InnerAutofillPopupDelegate(this);
        mAutofillPopup = new AutofillPopup(
            mContentViewCore->GetContext(),
            mContentViewCore->GetViewAndroidDelegate(),
            delegate);
    }
    mAutofillPopup->SetAnchorRect(x, y, width, height);
    mAutofillPopup->FilterAndShow(suggestions);
}

//@CalledByNative
void AwAutofillClient::HideAutofillPopup()
{
    if (mAutofillPopup == NULL)
        return;

    mAutofillPopup->Hide();
    mAutofillPopup = NULL;
}

//@CalledByNative ArrayOf AutofillSuggestion
AutoPtr<ArrayOf<IInterface*> > AwAutofillClient::CreateAutofillSuggestionArray(
    /* [in] */ Int32 size)
{
    AutoPtr<ArrayOf<IInterface*> > array = ArrayOf<IInterface*>::Alloc(size);
    return array;
}

/**
 * @param array AutofillSuggestion array that should get a new suggestion added.
 * @param index Index in the array where to place a new suggestion.
 * @param name Name of the suggestion.
 * @param label Label of the suggestion.
 * @param uniqueId Unique suggestion id.
 */
//@CalledByNative ArrayOf AutofillSuggestion
void AwAutofillClient::AddToAutofillSuggestionArray(
    /* [in] */ ArrayOf<IInterface*>* array,
    /* [in] */ Int32 index,
    /* [in] */ const String& name,
    /* [in] */ const String& label,
    /* [in] */ Int32 uniqueId)
{
    AutoPtr<AutofillSuggestion> autofillSuggestion = new AutofillSuggestion(name, label, uniqueId);
    AutoPtr<IInterface> suggestionInterface = TO_IINTERFACE(autofillSuggestion);
    array->Set(index, suggestionInterface.Get());
}

void AwAutofillClient::NativeSuggestionSelected(
    /* [in] */ Int64 nativeAwAutofillClient,
    /* [in] */ Int32 position)
{
    Elastos_AwAutofillClient_nativeSuggestionSelected(THIS_PROBE(IInterface), (Handle32)nativeAwAutofillClient, position);
}
//callback function definition
void AwAutofillClient::ShowAutofillPopup(
    /* [in] */ IInterface* obj,
    /* [in] */ Float x,
    /* [in] */ Float y,
    /* [in] */ Float width,
    /* [in] */ Float height,
    /* [in] */ ArrayOf<IInterface*>* suggestions)
{
    AutoPtr<AwAutofillClient> mObj = (AwAutofillClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwAutofillClient", "AwAutofillClient::ShowAutofillPopup, mObj is NULL");
        return;
    }
    mObj->ShowAutofillPopup(x, y, width, height, suggestions);
}

void AwAutofillClient::HideAutofillPopup(
    /* [in] */ IInterface* obj)
{
    AutoPtr<AwAutofillClient> mObj = (AwAutofillClient*)(IObject::Probe(obj));
    if (NULL == mObj)
    {
        Logger::E("AwAutofillClient", "AwAutofillClient::HideAutofillPopup, mObj is NULL");
        return;
    }
    mObj->HideAutofillPopup();
}

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos
