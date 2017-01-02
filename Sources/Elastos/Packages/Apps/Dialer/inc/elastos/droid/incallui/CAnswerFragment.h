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

#ifndef __ELASTOS_DROID_INCALLUI_CANSWERFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_CANSWERFRAGMENT_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "_Elastos_Droid_InCallUI_CAnswerFragment.h"
#include "elastos/droid/incallui/BaseFragment.h"
#include "elastos/droid/incallui/CGlowPadWrapper.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Text::IEditable;
using Elastos::Droid::Text::INoCopySpan;
using Elastos::Droid::Text::ITextWatcher;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IArrayAdapter;
using Elastos::Droid::Widget::IEditText;
using Elastos::Core::Object;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace InCallUI {

CarClass(CAnswerFragment)
    , public BaseFragment
    , public IAnswerFragment
    , public IAnswerUi
    , public IUi
{
public:
    class InnerListener
        : public Object
        , public IAnswerListener
    {
    public:
        CAR_INTERFACE_DECL();

        InnerListener(
            /* [in] */ CAnswerFragment* host)
            : mHost(host)
        {}

        // @Override
        virtual CARAPI OnAnswer(
            /* [in] */ Int32 videoState,
            /* [in] */ IContext* context);

        // @Override
        virtual CARAPI OnDecline();

        // @Override
        virtual CARAPI OnText();

    private:
        CAnswerFragment* mHost;
    };

    /**
     * OnItemClickListener for the "Respond via SMS" popup.
     */
    class RespondViaSmsItemClickListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        RespondViaSmsItemClickListener(
            /* [in] */ CAnswerFragment* host)
            : mHost(host)
        {}

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);

    private:
        CAnswerFragment* mHost;
    };

private:
    class DialogCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL();

        DialogCancelListener(
            /* [in] */ CAnswerFragment* host)
            : mHost(host)
        {}

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        CAnswerFragment* mHost;
    };

    class PositiveButtonClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        PositiveButtonClickListener(
            /* [in] */ IEditText* et,
            /* [in] */ CAnswerFragment* host)
            : mEditText(et)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<IEditText> mEditText;
        CAnswerFragment* mHost;
    };

    class NegativeButtonClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        NegativeButtonClickListener(
            /* [in] */ CAnswerFragment* host)
            : mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CAnswerFragment* mHost;
    };

    class MyTextWatcher
        : public Object
        , public ITextWatcher
        , public INoCopySpan
    {
    public:
        CAR_INTERFACE_DECL();

        MyTextWatcher(
            /* [in] */ CAnswerFragment* host)
            : mHost(host)
        {}

        CARAPI BeforeTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 count,
            /* [in] */ Int32 after);

        CARAPI OnTextChanged(
            /* [in] */ ICharSequence* s,
            /* [in] */ Int32 start,
            /* [in] */ Int32 before,
            /* [in] */ Int32 count);

        CARAPI AfterTextChanged(
            /* [in] */ IEditable* s);

    private:
        CAnswerFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CAR_OBJECT_DECL();

    CAnswerFragment();

    CARAPI constructor();

    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnDestroyView();

    // @Override
    CARAPI ShowAnswerUi(
        /* [in] */ Boolean show);

    /**
     * Sets targets on the glowpad according to target set identified by the parameter.
     * @param targetSet Integer identifying the set of targets to use.
     */
    // @Override
    CARAPI ShowTargets(
        /* [in] */ Int32 targetSet);

    // @Override
    CARAPI ShowMessageDialog();

    CARAPI_(void) DismissPendingDialogues();

    CARAPI_(Boolean) HasPendingDialogs();

    /**
     * Shows the custom message entry dialog.
     */
    CARAPI_(void) ShowCustomMessageDialog();

    // @Override
    CARAPI ConfigureMessageDialog(
        /* [in] */ IList* textResponses);

    // @Override
    CARAPI GetContext(
        /* [out] */ IContext** context);

    // @Override
    virtual CARAPI OnAnswer(
        /* [in] */ Int32 videoState,
        /* [in] */ IContext* context);

    // @Override
    virtual CARAPI OnDecline();

    // @Override
    virtual CARAPI OnText();

private:
    CARAPI_(Boolean) IsCannedResponsePopupShowing();

    CARAPI_(Boolean) IsCustomMessagePopupShowing();

    /**
     * Dismiss the canned response list popup.
     *
     * This is safe to call even if the popup is already dismissed, and even if you never called
     * showRespondViaSmsPopup() in the first place.
     */
    CARAPI_(void) DismissCannedResponsePopup();

    /**
     * Dismiss the custom compose message popup.
     */
    CARAPI_(void) DismissCustomMessagePopup();

public:
    static const Int32 TARGET_SET_FOR_AUDIO_WITHOUT_SMS = 0;
    static const Int32 TARGET_SET_FOR_AUDIO_WITH_SMS = 1;
    static const Int32 TARGET_SET_FOR_VIDEO_WITHOUT_SMS = 2;
    static const Int32 TARGET_SET_FOR_VIDEO_WITH_SMS = 3;
    static const Int32 TARGET_SET_FOR_VIDEO_UPGRADE_REQUEST = 4;

private:
    /**
     * The popup showing the list of canned responses.
     *
     * This is an AlertDialog containing a ListView showing the possible choices.  This may be null
     * if the InCallScreen hasn't ever called showRespondViaSmsPopup() yet, or if the popup was
     * visible once but then got dismissed.
     */
    AutoPtr<IDialog> mCannedResponsePopup;

    /**
     * The popup showing a text field for users to type in their custom message.
     */
    AutoPtr<IAlertDialog> mCustomMessagePopup;

    AutoPtr<IArrayAdapter> mSmsResponsesAdapter;

    AutoPtr<IList> mSmsResponses;

    AutoPtr<CGlowPadWrapper> mGlowpad;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CANSWERFRAGMENT_H__
