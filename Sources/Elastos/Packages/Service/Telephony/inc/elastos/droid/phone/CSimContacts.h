#ifndef  __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__
#define  __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__

#include "_Elastos_Droid_Phone_CSimContacts.h"
#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * SIM Address Book UI for the Phone app.
 */
CarClass(CSimContacts)
    , public ADNList
    , public ISimContacts
{
private:
    class NamePhoneTypePair
        : public Object
    {
    public:
        TO_STRING_IMPL("CSimContacts::NamePhoneTypePair")

        NamePhoneTypePair(
            /* [in] */ const String& nameWithPhoneType);

    private:
        String name;
        Int32 phoneType;
    };

    class ImportAllSimContactsThread
        : public Thread
        , public IOnCancelListener
        , public IOnClickListener
    {
    public:
        TO_STRING_IMPL("CSimContacts::ImportAllSimContactsThread")

        CAR_INTERFACE_DECL()

        ImportAllSimContactsThread();

        //@Override
        CARAPI Run();

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        Boolean mCanceled;
    }

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL();

    CARAPI constructor();

    //@Override
    CARAPI OnCreateOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnPrepareOptionsMenu(
        /* [in] */ IMenu* menu,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnOptionsItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnContextItemSelected(
        /* [in] */ IMenuItem* item,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI OnCreateContextMenu(
        /* [in] */ IContextMenu* menu,
        /* [in] */ IView* v,
        /* [in] */ IContextMenuContextMenuInfo* menuInfo);

    //@Override
    CARAPI OnListItemClick(
        /* [in] */ IListView* l,
        /* [in] */ IView* v,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    //@Override
    CARAPI OnKeyDown(
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

protected:
    /* Followings are overridden methods */
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI NewAdapter(
        /* [out] */ ICursorAdapter** adaptor);

    //@Override
    CARAPI ResolveIntent(
        /* [out] */ IUri** uri);

private:
    static CARAPI_(void) ActuallyImportOneSimContact(
        /* [in] */ ICursor* cursor,
        /* [in] */ IContentResolver* resolver,
        /* [in] */ IAccount* account);

    CARAPI_(void) ImportOneSimContact(
        /* [in] */ Int32 position);

public:
    static const String LOG_TAG;

    static const AutoPtr<IContentValues> sEmptyContentValues;

    static const Int32 MENU_IMPORT_ONE;
    static const Int32 MENU_IMPORT_ALL;
    AutoPtr<IProgressDialog> mProgressDialog;

    AutoPtr<IAccount> mAccount;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__