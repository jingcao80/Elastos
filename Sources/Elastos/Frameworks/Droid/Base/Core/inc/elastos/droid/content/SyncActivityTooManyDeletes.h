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

#ifndef __ELASTOS_DROID_CONTENT_SYNCACTIVITYTOOMANYDELETES_H__
#define __ELASTOS_DROID_CONTENT_SYNCACTIVITYTOOMANYDELETES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/app/Activity.h"
#include <elastos/core/Object.h>

using Elastos::Droid::App::Activity;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterViewOnItemClickListener;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Accounts::IAccount;

namespace Elastos {
namespace Droid {
namespace Content {

/**
 * Presents multiple options for handling the case where a sync was aborted because there
 * were too many pending deletes. One option is to force the delete, another is to rollback
 * the deletes, the third is to do nothing.
 * @hide
 */
class SyncActivityTooManyDeletes
    : public Activity
    , public ISyncActivityTooManyDeletes
{
private:
    class InnerListener
        : public Object
        , public IAdapterViewOnItemClickListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ SyncActivityTooManyDeletes* host);

        CARAPI OnItemClick(
            /* [in] */ IAdapterView* parent,
            /* [in] */ IView* view,
            /* [in] */ Int32 position,
            /* [in] */ Int64 id);
    private:
        SyncActivityTooManyDeletes* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    SyncActivityTooManyDeletes();

    virtual ~SyncActivityTooManyDeletes();

    CARAPI constructor();

    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    virtual CARAPI OnItemClick(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

private:
    CARAPI StartSyncReallyDelete();

    CARAPI StartSyncUndoDeletes();

private:
    Int64 mNumDeletes;
    AutoPtr<IAccount> mAccount;
    String mAuthority;
    String mProvider;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_SYNCACTIVITYTOOMANYDELETES_H__
