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

#ifndef __ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__
#define __ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Runnable.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::Handler;
using Elastos::Core::Runnable;
using Elastos::Core::IRunnable;
using Elastos::Core::IThread;

namespace Elastos {
namespace Droid {
namespace Webkit {

class WebViewDatabase;

class WebSyncManager : public Runnable
{
public:
    CARAPI Run();

    /**
     * sync() forces sync manager to sync now
     */
    virtual CARAPI Sync();

    /**
     * resetSync() resets sync manager's timer
     */
    virtual CARAPI ResetSync();

    /**
     * startSync() requests sync manager to start sync
     */
    virtual CARAPI StartSync();

    /**
     * stopSync() requests sync manager to stop sync. remove any SYNC_MESSAGE in
     * the queue to break the sync loop
     */
    virtual CARAPI StopSync();

    virtual CARAPI_(void) SyncFromRamToFlash() = 0;

    CARAPI ToString(
        /* [out] */ String* info);

protected:
    WebSyncManager();

    WebSyncManager(
        /* [in] */ IContext* context,
        /* [in] */ const String& name);

    virtual CARAPI_(AutoPtr<IInterface>) Clone();

    virtual CARAPI_(void) OnSyncInit();

protected:
    static const String LOGTAG;
    AutoPtr<IWebViewDatabase> mDataBase;
    AutoPtr<IHandler> mHandler;
};

} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_WEBKIT_WEBSYNCMANAGER_H__
