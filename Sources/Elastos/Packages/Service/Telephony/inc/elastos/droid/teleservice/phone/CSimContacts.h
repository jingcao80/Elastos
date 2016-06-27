#ifndef  __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__
#define  __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__

#include "_Elastos_Droid_TeleService_Phone_CSimContacts.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/teleservice/phone/ADNList.h"
#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Accounts.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.View.h"
#include <elastos/core/Object.h>
#include <elastos/core/Thread.h>

using Elastos::Droid::App::IProgressDialog;
using Elastos::Droid::Accounts::IAccount;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::View::IContextMenu;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMenu;
using Elastos::Droid::View::IMenuItem;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IContextMenuInfo;

using Elastos::Core::Thread;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace TeleService {
namespace Phone {

/**
 * SIM Address Book UI for the Phone app.
 */
CarClass(CSimContacts)
    , public ADNList
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
        friend class CSimContacts;

        String mName;
        Int32 mPhoneType;
    };

    class ImportAllSimContactsThread
        : public Thread
        , public IDialogInterfaceOnCancelListener
        , public IDialogInterfaceOnClickListener
    {
    public:
        TO_STRING_IMPL("CSimContacts::ImportAllSimContactsThread")

        CAR_INTERFACE_DECL()

        ImportAllSimContactsThread(
            /* [in] */ CSimContacts* host);

        //@Override
        CARAPI Run();

        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        CSimContacts* mHost;
        Boolean mCanceled;
    };

public:
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
        /* [in] */ IContextMenuInfo* menuInfo);

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
    static const String TAG;

    static const AutoPtr<IContentValues> sEmptyContentValues;

    static const Int32 MENU_IMPORT_ONE;
    static const Int32 MENU_IMPORT_ALL;
    AutoPtr<IProgressDialog> mProgressDialog;

    AutoPtr<IAccount> mAccount;
};

} // namespace Phone
} // namespace TeleService
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_PHONE_CSIMCONTACTS_H__