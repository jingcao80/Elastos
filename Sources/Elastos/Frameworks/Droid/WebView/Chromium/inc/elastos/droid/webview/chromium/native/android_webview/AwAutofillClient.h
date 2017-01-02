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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/autofill/AutofillPopup.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/autofill/AutofillSuggestion.h"

using Elastos::Droid::View::IViewGroup;
using Elastos::Droid::Webkit::Webview::Chromium::Content::Browser::ContentViewCore;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Autofill::AutofillPopup;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Autofill::AutofillSuggestion;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
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
        /* [in] */ Handle64 nativeClient);

    virtual CARAPI_(void) Init(
        /* [in] */ ContentViewCore* contentViewCore);

    //@CalledByNative
    virtual CARAPI_(void) HideAutofillPopup();

private:
    AwAutofillClient(
        /* [in] */ Handle64 nativeAwAutofillClient);

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
        /* [in] */ Handle64 nativeAwAutofillClient,
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
    const Handle64 mNativeAwAutofillClient;
    AutoPtr<AutofillPopup> mAutofillPopup;
    AutoPtr<IViewGroup> mContainerView;
    AutoPtr<ContentViewCore> mContentViewCore;
};

} // namespace AndroidWebview
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_ANDROIDWEBVIEW_AWAUTOFILLCLIENT_H__
