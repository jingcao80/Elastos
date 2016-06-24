#ifndef  __ELASTOS_DROID_PHONE_CEDITFDNCONTACTSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CEDITFDNCONTACTSCREEN_H__

#include "_Elastos_Droid_Phone_CEditFdnContactScreen.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Activity to let the user add or edit an FDN contact.
 */
CarClass(CEditFdnContactScreen)
    , public Activity
    , public IEditFdnContactScreen
{
private:
    class MyViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CEditFdnContactScreen::MyViewOnClickListener")

        CAR_INTERFACE_DECL()

        MyViewOnClickListener(
            /* [in] */ CEditFdnContactScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CEditFdnContactScreen* mHost;

    };

    class MyViewOnFocusChangeListener
        : public Object
        , public IViewOnFocusChangeListener
    {
    public:
        TO_STRING_IMPL("CEditFdnContactScreen::MyViewOnFocusChangeListener")

        CAR_INTERFACE_DECL()

        MyViewOnFocusChangeListener(
            /* [in] */ CEditFdnContactScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI OnFocusChange(
            /* [in] */ IView* v,
            /* [in] */ Boolean hasFocus);

    private:
        CEditFdnContactScreen* mHost;
    };

    class QueryHandler :
        : public AsyncQueryHandler
    {
    public:
        TO_STRING_IMPL("CEditFdnContactScreen::AsyncQueryHandler")

        QueryHandler(
            /* [in] */ IContentResolver* cr,
            /* [in] */ CEditFdnContactScreen* host);

    protected:
        //@Override
        CARAPI OnQueryComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ ICursor* c);

        //@Override
        CARAPI OnInsertComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ IUri* uri);

        //@Override
        CARAPI OnUpdateComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ Int32 result);

        //@Override
        CARAPI OnDeleteComplete(
            /* [in] */ Int32 token,
            /* [in] */ IInterface* cookie,
            /* [in] */ Int32 result);

    private:
        CEditFdnContactScreen* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        TO_STRING_IMPL("CEditFdnContactScreen::MyRunnable")

        MyRunnable(
            /* [in] */ CEditFdnContactScreen* host)
            : mHost(host)

        //@Override
        CARAPI Run();

    private:
        CEditFdnContactScreen* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CEditFdnContactScreen();

    CARAPI constructor();

    /**
     * Overridden to display the import and delete commands.
     */
    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    /**
     * Allow the menu to be opened ONLY if we're not busy.
     */
    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    /**
     * Overridden to allow for handling of delete and import.
     */
    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    /**
     * We now want to bring up the pin request screen AFTER the
     * contact information is displayed, to help with user
     * experience.
     *
     * Also, process the results from the contact picker.
     */
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) ResolveIntent();

    /**
     * We have multiple layouts, one to indicate that the user needs to
     * open the keyboard to enter information (if the keybord is hidden).
     * So, we need to make sure that the layout here matches that in the
     * layout file.
     */
    CARAPI_(void) SetupView();

    CARAPI_(String) GetNameFromTextField();

    CARAPI_(String) GetNumberFromTextField();

    CARAPI_(AutoPtr<IUri>) GetContentURI();

    /**
      * @param number is voice mail number
      * @return true if number length is less than 20-digit limit
      *
      * TODO: Fix this logic.
      */
    CARAPI(Boolean) IsValidNumber(
        /* [in] */ const String& number);

    CARAPI_(void) AddContact();

    CARAPI_(void) UpdateContact();

    /**
     * Handle the delete command, based upon the state of the Activity.
     */
    CARAPI_(void) DeleteSelected();

    CARAPI_(void) AuthenticatePin2();

    CARAPI_(void) DisplayProgress(
        /* [in] */ Boolean flag);

    /**
     * Removed the status field, with preference to displaying a toast
     * to match the rest of settings UI.
     */
    CARAPI_(void) ShowStatus(
        /* [in] */ ICharSequence* statusMsg);

    CARAPI_(void) HandleResult(
        /* [in] */ Boolean success,
        /* [in] */ Boolean invalidNumber);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

private:
    static const String TAG;
    static const Boolean DBG;

    // Menu item codes
    static const Int32 MENU_IMPORT;
    static const Int32 MENU_DELETE;

    static const String INTENT_EXTRA_NAME;
    static const String INTENT_EXTRA_NUMBER;

    static const Int32 PIN2_REQUEST_CODE;

    String mName;
    String mNumber;
    String mPin2;
    Boolean mAddContact;
    AutoPtr<IQueryHandler> mQueryHandler;

    AutoPtr<IEditText> mNameField;
    AutoPtr<IEditText> mNumberField;
    AutoPtr<ILinearLayout> mPinFieldContainer;
    AutoPtr<IButton> mButton;

    AutoPtr<IHandler> mHandler;

    /**
     * Constants used in importing from contacts
     */
    /** request code when invoking subactivity */
    static const Int32 CONTACTS_PICKER_CODE;
    /** projection for phone number query */
    static const AutoPtr<ArrayOf<String> > NUM_PROJECTION;
    /** static intent to invoke phone number picker */
    static const AutoPtr<IIntent> CONTACT_IMPORT_INTENT;
    /** flag to track saving state */
    Boolean mDataBusy;

    AutoPtr<IViewOnClickListener> mClicked;

    AutoPtr<IViewOnFocusChangeListener> mOnFocusChangeHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CEDITFDNCONTACTSCREEN_H__