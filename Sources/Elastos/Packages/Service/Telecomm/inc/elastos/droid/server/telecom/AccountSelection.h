
#ifndef __ELASTOS_DROID_SERVER_TELECOM_ACCOUNTSELECTION_H__
#define __ELASTOS_DROID_SERVER_TELECOM_ACCOUNTSELECTION_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <Elastos.CoreLibrary.Utility.h>
#include <Elastos.Droid.Net.h>
#include <Elastos.Droid.Telecomm.h>
#include <elastos/droid/app/Activity.h>
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/droid/widget/ArrayAdapter.h>
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnCancelListener;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Telecomm::Telecom::ITelecomManager;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::ArrayAdapter;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

class AccountSelection
    : public Activity
    , public IAccountSelection
{
private:
    class AccountListAdapter
        : public ArrayAdapter
    {
    public:
        AccountListAdapter(
            /* [in] */ AccountSelection* host);

        CARAPI constructor(
            /* [in] */ IContext* context,
            /* [in] */ Int32 resource,
            /* [in] */ IList* accountHandles);

        // @Override
        CARAPI GetView(
            /* [in] */ Int32 position,
            /* [in] */ IView* convertView,
            /* [in] */ IViewGroup* parent,
            /* [out] */ IView** result);

    private:
        AccountSelection* mHost;

        AutoPtr<IContext> mContext;

        Int32 mResId;
    };

    class SubDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
        // @Override
    public:
        CAR_INTERFACE_DECL()

        SubDialogInterfaceOnClickListener(
            /* [in] */ AccountSelection* host);

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        AccountSelection* mHost;
    };

    class SubDialogInterfaceOnCancelListener
        : public Object
        , public IDialogInterfaceOnCancelListener
    {
    public:
        CAR_INTERFACE_DECL()

        SubDialogInterfaceOnCancelListener(
            /* [in] */ AccountSelection* host);

        // @Override
        CARAPI OnCancel(
            /* [in] */ IDialogInterface* dialog);

    private:
        AccountSelection* mHost;
     };

public:
    CAR_INTERFACE_DECL()

    AccountSelection();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    // @Override
    CARAPI OnDestroy();

public:
    AutoPtr<ITelecomManager> mTelecomManager;

    AutoPtr<IList> mAccountHandles;

    AutoPtr<IUri> mHandle;

    Boolean mIsSelected;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_ACCOUNTSELECTION_H__
