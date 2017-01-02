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

#ifndef __ELASTOS_DROID_DIALER_INTERACTIONS_PHONENUMBERINTERACTION_H__
#define __ELASTOS_DROID_DIALER_INTERACTIONS_PHONENUMBERINTERACTION_H__

#include "_Elastos.Droid.Dialer.h"
// #include <elastos/droid/app/DialogFragment.h>
// #include <elastos/droid/widget/ArrayAdapter.h>
#include <elastos/core/Object.h>
// #include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
// #include "Elastos.Droid.Database.h"
// #include "Elastos.Droid.Net.h"
// #include "Elastos.Droid.Os.h"
// #include "Elastos.Droid.View.h"
// #include "Elastos.Droid.Widget.h"
// #include "Elastos.CoreLibrary.Utility.h"

// using Elastos::Droid::App::DialogFragment;
// using Elastos::Droid::App::IDialog;
// using Elastos::Droid::App::IFragmentManager;
// using Elastos::Droid::Content::IContext;
// using Elastos::Droid::Content::ICursorLoader;
// using Elastos::Droid::Content::IDialogInterface;
// using Elastos::Droid::Content::IDialogInterfaceOnDismissListener;
// using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
// using Elastos::Droid::Content::ILoader;
using Elastos::Droid::Content::ILoaderOnLoadCompleteListener;
// using Elastos::Droid::Database::ICursor;
// using Elastos::Droid::Net::IUri;
// using Elastos::Droid::Os::IBundle;
// using Elastos::Droid::View::IView;
// using Elastos::Droid::View::IViewGroup;
// using Elastos::Droid::Widget::ArrayAdapter;
// using Elastos::Droid::Widget::IListAdapter;
using Elastos::Core::Object;
// using Elastos::Utility::IList;
// using Elastos::Utility::IArrayList;
// using Elastos::Droid::Dialer::Activity::ITransactionSafeActivity;

namespace Elastos{
namespace Droid{
namespace Dialer {
namespace Interactions {

class PhoneNumberInteraction
    : public Object
    , public ILoaderOnLoadCompleteListener
{
public:
    /**
     * A model object for capturing a phone number for a given contact.
     */
    // @VisibleForTesting
    class PhoneItem
        : public Object
        , public IParcelable
        // TODO:
        // , public ICollapserCollapsible
    {
    public:
        CAR_INTERFACE_DECL();

        PhoneItem();

        // @Override
        CARAPI WriteToParcel(
            /* [in] */ IParcel* dest);

        CARAPI ReadFromParcel(
            /* [in] */ IParcel* source);

        // @Override
        CARAPI CollapseWith(
            /* [in] */ IInterface* phoneItem);

        // @Override
        CARAPI ShouldCollapseWith(
            /* [in] */ IInterface* phoneItem,
            /* [in] */ IContext* context,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI ToString(
            /* [out] */ String* str);

    public:
        Int64 mId;
        String mPhoneNumber;
        String mAccountType;
        String mDataSet;
        Int64 mType;
        String mLabel;
        /** {@link Phone#CONTENT_ITEM_TYPE} or {@link SipAddress#CONTENT_ITEM_TYPE}. */
        String mMimeType;
    };

    /**
     * {@link DialogFragment} used for displaying a dialog with a list of phone numbers of which
     * one will be chosen to make a call or initiate an sms message.
     *
     * It is recommended to use
     * {@link PhoneNumberInteraction#startInteractionForPhoneCall(TransactionSafeActivity, Uri)} or
     * {@link PhoneNumberInteraction#startInteractionForTextMessage(TransactionSafeActivity, Uri)}
     * instead of directly using this class, as those methods handle one or multiple data cases
     * appropriately.
     */
    /* Made public to let the system reach this class */
    class PhoneDisambiguationDialogFragment
        : public DialogFragment
        , public IPhoneDisambiguationDialogFragment
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL();

        static CARAPI_(void) Show(
            /* [in] */ IFragmentManager* fragmentManager,
            /* [in] */ IArrayList* phoneList,
            /* [in] */ Int32 interactionType,
            /* [in] */ const String& callOrigin);

        // @Override
        CARAPI OnCreateDialog(
            /* [in] */ IBundle* savedInstanceState,
            /* [out] */ IDialog** dialog);

        // @Override
        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        static const String ARG_PHONE_LIST; // = "phoneList";
        static const String ARG_INTERACTION_TYPE; // = "interactionType";
        static const String ARG_CALL_ORIGIN; // = "callOrigin";

        Int32 mInteractionType;
        AutoPtr<IListAdapter> mPhonesAdapter;
        AutoPtr<IList> mPhoneList;
        String mCallOrigin;
    };

private:
    class PhoneItemAdapter
        : public ArrayAdapter
    {
    public:
        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ IList* list,
            /* [in] */ Int32 interactionType);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);
    private:
        Int32 mInteractionType;
    };

public:
    CAR_INTERFACE_DECL();

    /**
     * Constructs a new {@link PhoneNumberInteraction}. The constructor takes in a {@link Context}
     * instead of a {@link TransactionSafeActivity} for testing purposes to verify the functionality
     * of this class. However, all factory methods for creating {@link PhoneNumberInteraction}s
     * require a {@link TransactionSafeActivity} (i.e. see {@link #startInteractionForPhoneCall}).
     */
    // @VisibleForTesting
    PhoneNumberInteraction(
        /* [in] */ IContext* context,
        /* [in] */ Int32 interactionType,
        /* [in] */ IDialogInterfaceOnDismissListener* dismissListener);

    /**
     * Initiates the interaction. This may result in a phone call or sms message started
     * or a disambiguation dialog to determine which phone number should be used. If there
     * is a primary phone number, it will be automatically used and a disambiguation dialog
     * will no be shown.
     */
    // @VisibleForTesting
    CARAPI_(void) StartInteraction(
        /* [in] */ IUri* uri);

    /**
     * Initiates the interaction to result in either a phone call or sms message for a contact.
     * @param uri Contact Uri
     * @param useDefault Whether or not to use the primary(default) phone number. If true, the
     * primary phone number will always be used by default if one is available. If false, a
     * disambiguation dialog will be shown regardless of whether or not a primary phone number
     * is available.
     */
    // @VisibleForTesting
    CARAPI StartInteraction(
        /* [in] */ IUri* uri,
        /* [in] */ Boolean useDefault);

    // @Override
    CARAPI OnLoadComplete(
        /* [in] */ ILoader* loader,
        /* [in] */ IInterface* cursor);

    /**
     * Start call action using given contact Uri. If there are multiple candidates for the phone
     * call, dialog is automatically shown and the user is asked to choose one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     */
    static CARAPI_(void) StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri);

    /**
     * Start call action using given contact Uri. If there are multiple candidates for the phone
     * call, dialog is automatically shown and the user is asked to choose one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     * @param useDefault Whether or not to use the primary(default) phone number. If true, the
     * primary phone number will always be used by default if one is available. If false, a
     * disambiguation dialog will be shown regardless of whether or not a primary phone number
     * is available.
     */
    static CARAPI_(void) StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri,
        /* [in] */ Boolean useDefault);

    /**
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param callOrigin If non null, {@link PhoneConstants#EXTRA_CALL_ORIGIN} will be
     * appended to the Intent initiating phone call. See comments in Phone package (PhoneApp)
     * for more detail.
     */
    static CARAPI_(void) StartInteractionForPhoneCall(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri,
        /* [in] */ const String& callOrigin);

    /**
     * Start text messaging (a.k.a SMS) action using given contact Uri. If there are multiple
     * candidates for the phone call, dialog is automatically shown and the user is asked to choose
     * one.
     *
     * @param activity that is calling this interaction. This must be of type
     * {@link TransactionSafeActivity} because we need to check on the activity state after the
     * phone numbers have been queried for.
     * @param uri contact Uri (built from {@link Contacts#CONTENT_URI}) or data Uri
     * (built from {@link Data#CONTENT_URI}). Contact Uri may show the disambiguation dialog while
     * data Uri won't.
     */
    static CARAPI_(void) StartInteractionForTextMessage(
        /* [in] */ ITransactionSafeActivity* activity,
        /* [in] */ IUri* uri);

    // @VisibleForTesting
    CARAPI_(AutoPtr<ICursorLoader>) GetLoader();

    // @VisibleForTesting
    CARAPI_(void) ShowDisambiguationDialog(
        /* [in] */ IArrayList* phoneList);

private:
    PhoneNumberInteraction(
        /* [in] */ IContext* context,
        /* [in] */ Int32 interactionType,
        /* [in] */ IDialogInterfaceOnDismissListener* dismissListener,
        /* [in] */ const String& callOrigin);

    CARAPI_(void) PerformAction(
        /* [in] */ const String& phoneNumber);

    static CARAPI_(void) PerformAction(
        /* [in] */ IContext* context,
        /* [in] */ const String& phoneNumber,
        /* [in] */ Int32 interactionType,
        /* [in] */ const String& callOrigin);

    CARAPI_(Boolean) IsSafeToCommitTransactions();

    CARAPI_(void) OnDismiss();
private:
    static const String TAG;

    static const String PHONE_NUMBER_PROJECTION[];

    static const Int32 _ID; // = 0;
    static const Int32 NUMBER; // = 1;
    static const Int32 IS_SUPER_PRIMARY; // = 2;
    static const Int32 ACCOUNT_TYPE; // = 3;
    static const Int32 DATA_SET; // = 4;
    static const Int32 TYPE; // = 5;
    static const Int32 LABEL; // = 6;
    static const Int32 MIMETYPE; // = 7;
    static const Int32 CONTACT_ID; // = 8;

    static const String PHONE_NUMBER_SELECTION;

    AutoPtr<IContext> mContext;
    AutoPtr<IDialogInterfaceOnDismissListener> mDismissListener;
    Int32 mInteractionType;

    String mCallOrigin;
    Boolean mUseDefault;

    static const Int32 UNKNOWN_CONTACT_ID; // = -1;
    Int64 mContactId; //  = UNKNOWN_CONTACT_ID;

    AutoPtr<ICursorLoader> mLoader;
};

} // Interactions
} // Dialer
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_DIALER_INTERACTIONS_PHONENUMBERINTERACTION_H__
