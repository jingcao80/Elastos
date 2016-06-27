#ifndef  __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__

#include "_Elastos_Droid_Phone_CDeleteFdnContactScreen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include "elastos/droid/os/Runnable.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Database.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Net.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::AsyncQueryHandler;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace Phone {

/**
 * Activity to let the user delete an FDN contact.
 */
CarClass(CDeleteFdnContactScreen)
    , public Activity
{
private:
    class QueryHandler
        : public AsyncQueryHandler
    {
    public:
        QueryHandler(
            /* [in] */ IContentResolver* cr,
            /* [in] */ CDeleteFdnContactScreen* host);

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
        CDeleteFdnContactScreen* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CDeleteFdnContactScreen* host)
            : mHost(host)
        {}

        //@Override
        CARAPI Run();

    private:
        CDeleteFdnContactScreen* mHost;
    };

public:
    CAR_OBJECT_DECL();

    CDeleteFdnContactScreen();

    CARAPI constructor();

protected:
    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* icicle);

    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* intent);

private:
    CARAPI_(void) ResolveIntent();

    CARAPI DeleteContact();

    CARAPI AuthenticatePin2();

    CARAPI DisplayProgress(
        /* [in] */ Boolean flag);

    // Replace the status field with a toast to make things appear similar
    // to the rest of the settings.  Removed the useless status field.
    CARAPI_(void) ShowStatus(
        /* [in] */ ICharSequence* statusMsg);

    CARAPI HandleResult(
        /* [in] */ Boolean success);

    CARAPI_(void) Log(
        /* [in] */ const String& msg);

protected:
    AutoPtr<QueryHandler> mQueryHandler;

private:
    static const String TAG;
    static const Boolean DBG;

    static const String INTENT_EXTRA_NAME;
    static const String INTENT_EXTRA_NUMBER;

    static const Int32 PIN2_REQUEST_CODE;

    String mName;
    String mNumber;
    String mPin2;

    AutoPtr<IHandler> mHandler;
};

} // namespace Phone
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__