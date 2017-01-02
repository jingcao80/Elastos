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

#include "Elastos.Droid.Os.h"
#include "elastos/droid/content/res/CThemeProcessingListener.h"
#include "elastos/droid/content/res/CThemeManager.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/AutoLock.h>

#include <elastos/core/AutoLock.h>
using Elastos::Core::AutoLock;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

//==============================================================================
//                  CThemeProcessingListener::OnFinishedProcessingRunnable
//==============================================================================

ECode CThemeProcessingListener::OnFinishedProcessingRunnable::Run()
{
    {    AutoLock syncLock(mHost->mProcessingListenersLock);
        List<AutoPtr<IThemeProcessingListener> > listenersToRemove;
        HashSet<AutoPtr<IThemeProcessingListener> >::Iterator it = mHost->mProcessingListeners.Begin();
        for (; it != mHost->mProcessingListeners.End(); ++it) {
            // try {
            ECode ec = (*it)->OnFinishedProcessing(mPkgName);
            if (FAILED(ec)) {
                Logger::W(CThemeManager::TAG, "Unable to update theme change progress 0x%08x", ec);
                listenersToRemove.PushBack(*it);
            }
            // } catch (Throwable e) {
            //     Log.w(CThemeManager::TAG, "Unable to update theme change progress", e);
            //     listenersToRemove.add(listener);
            // }
        }
        if (listenersToRemove.IsEmpty() == FALSE) {
            List<AutoPtr<IThemeProcessingListener> >::Iterator removeIt = listenersToRemove.Begin();
            for (; removeIt != listenersToRemove.End(); ++removeIt) {
                mHost->mProcessingListeners.Erase(*removeIt);
            }
        }
    }
    return NOERROR;
}


//==============================================================================
//                  CThemeProcessingListener
//==============================================================================

CAR_INTERFACE_IMPL(CThemeProcessingListener, Object, IIThemeProcessingListener)

CAR_OBJECT_IMPL(CThemeProcessingListener)

ECode CThemeProcessingListener::constructor(
    /* [in] */ IThemeManager* host)
{
    mHost = (CThemeManager*)host;
    return NOERROR;
}

ECode CThemeProcessingListener::OnFinishedProcessing(
    /* [in] */ const String& pkgName)
{
    AutoPtr<IRunnable> runnable = new OnFinishedProcessingRunnable(pkgName, mHost);
    Boolean result;
    return mHost->mHandler->Post(runnable, &result);
}

}
}
}
}
