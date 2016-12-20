
#ifndef __ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_IMPORTEXPORTDIALOGFRAGMENT_H__
#define __ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_IMPORTEXPORTDIALOGFRAGMENT_H__

#include "Elastos.Droid.App.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/dialerbind/analytics/AnalyticsDialogFragment.h"
#include <elastos/droid/widget/ArrayAdapter.h>

using Elastos::Droid::App::IFragmentManager;
using Elastos::Droid::Contacts::Common::Model::Account::IAccountWithDataSet;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::DialerBind::Analytics::AnalyticsDialogFragment;
using Elastos::Droid::Contacts::Common::Editor::ISelectAccountDialogFragmentListener;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::Widget::ArrayAdapter;

namespace Elastos {
namespace Droid {
namespace Contacts {
namespace Common {
namespace Interactions {

/**
 * An dialog invoked to import/export contacts.
 */
class ImportExportDialogFragment
    : public AnalyticsDialogFragment
    , public ISelectAccountDialogFragmentListener
{
private:
    class DialogItemAdapter
        : public ArrayAdapter
    {
    public:
        DialogItemAdapter(
            /* [in] */ ILayoutInflater* dialogInflater)
            : mDialogInflater(dialogInflater)
        {}

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 textViewResourceId);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** view);
    private:
        AutoPtr<ILayoutInflater> mDialogInflater;
    };

    class DialogOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        DialogOnClickListener(
            /* [in] */ DialogItemAdapter* adapter,
            /* [in] */ IContext* context,
            /* [in] */ const String& callingActivity,
            /* [in] */ ImportExportDialogFragment* host)
            : mAdapter(adapter)
            , mContext(context)
            , mCallingActivity(callingActivity)
            , mHost(host)
        {}

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AutoPtr<DialogItemAdapter> mAdapter;
        AutoPtr<IContext> mContext;
        String mCallingActivity;
        ImportExportDialogFragment* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    CARAPI constructor();

    using AnalyticsDialogFragment::Show;

    /** Preferred way to show this dialog */
    static CARAPI_(void) Show(
        /* [in] */ IFragmentManager* fragmentManager,
        /* [in] */ Boolean contactsAreAvailable,
        /* [in] */ IObject* callingActivity);

    // @Override
    CARAPI OnAttach(
        /* [in] */ IActivity* activity);

    // @Override
    CARAPI OnCreateDialog(
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IDialog** dialog);

    // @Override
    CARAPI OnAccountChosen(
        /* [in] */ IAccountWithDataSet* account,
        /* [in] */ IBundle* extraArgs);

    CARAPI OnAccountSelectorCancelled();

private:
    CARAPI_(void) DoShareVisibleContacts();

    /**
     * Handle "import from SIM" and "import from SD".
     *
     * @return {@code true} if the dialog show be closed.  {@code false} otherwise.
     */
    CARAPI_(Boolean) HandleImportRequest(
        /* [in] */ Int32 resId);

private:
    static const String TAG;

    static const String KEY_RES_ID;
    static const String ARG_CONTACTS_ARE_AVAILABLE;

    static const AutoPtr<ArrayOf<String> > LOOKUP_PROJECTION;

    friend class DialogOnClickListener;
};

} // namespace Interactions
} // namespace Common
} // namespace Contacts
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_CONTACTS_COMMON_INTERACTIONS_IMPORTEXPORTDIALOGFRAGMENT_H__
