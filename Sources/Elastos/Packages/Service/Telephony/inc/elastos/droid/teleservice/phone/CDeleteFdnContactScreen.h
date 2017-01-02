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

#ifndef  __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__
#define  __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__

#include "_Elastos_Droid_TeleService_Phone_CDeleteFdnContactScreen.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/content/AsyncQueryHandler.h"
#include <elastos/core/Runnable.h>
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
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::IBundle;

namespace Elastos {
namespace Droid {
namespace TeleService {
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
        TO_STRING_IMPL("CDeleteFdnContactScreen::QueryHandler")

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
} // namespace TeleService
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PHONE_CDELETEFDNCONTACTSCREEN_H__