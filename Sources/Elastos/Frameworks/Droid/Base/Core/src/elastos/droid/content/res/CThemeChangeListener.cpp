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
#include "elastos/droid/content/res/CThemeChangeListener.h"
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
//                  CThemeChangeListener::OnProgressRunnable
//==============================================================================

ECode CThemeChangeListener::OnProgressRunnable::Run()
{
    {    AutoLock syncLock(mHost->mChangeListenersLock);
        List<AutoPtr<IThemeChangeListener> > listenersToRemove;
        HashSet<AutoPtr<IThemeChangeListener> >::Iterator it = mHost->mChangeListeners.Begin();
        for (; it != mHost->mChangeListeners.End(); ++it) {
            // try {
            ECode ec = (*it)->OnProgress(mProgress);
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
            List<AutoPtr<IThemeChangeListener> >::Iterator removeIt = listenersToRemove.Begin();
            for (; removeIt != listenersToRemove.End(); ++removeIt) {
                mHost->mChangeListeners.Erase(*removeIt);
            }
        }
    }
    return NOERROR;
}


//==============================================================================
//                  CThemeChangeListener::OnFinishRunnable
//==============================================================================

ECode CThemeChangeListener::OnFinishRunnable::Run()
{
    {    AutoLock syncLock(mHost->mChangeListenersLock);
        List<AutoPtr<IThemeChangeListener> > listenersToRemove;
        HashSet<AutoPtr<IThemeChangeListener> >::Iterator it = mHost->mChangeListeners.Begin();
        for (; it != mHost->mChangeListeners.End(); ++it) {
            // try {
            ECode ec = (*it)->OnFinish(mIsSuccess);
            if (FAILED(ec)) {
                Logger::W(CThemeManager::TAG, "Unable to update theme change listener 0x%08x", ec);
                listenersToRemove.PushBack(*it);
            }
            // } catch (Throwable e) {
            //     Log.w(CThemeManager::TAG, "Unable to update theme change listener", e);
            //     listenersToRemove.add(listener);
            // }
        }
        if (listenersToRemove.IsEmpty() == FALSE) {
            List<AutoPtr<IThemeChangeListener> >::Iterator removeIt = listenersToRemove.Begin();
            for (; removeIt != listenersToRemove.End(); ++removeIt) {
                mHost->mChangeListeners.Erase(*removeIt);
            }
        }
    }
    return NOERROR;
}


//==============================================================================
//                  CThemeChangeListener
//==============================================================================

CAR_INTERFACE_IMPL(CThemeChangeListener, Object, IIThemeChangeListener)

CAR_OBJECT_IMPL(CThemeChangeListener)

ECode CThemeChangeListener::constructor(
    /* [in] */ IThemeManager* host)
{
    mHost = (CThemeManager*)host;
    return NOERROR;
}

ECode CThemeChangeListener::OnProgress(
    /* [in] */ Int32 progress)
{
    AutoPtr<IRunnable> runnable = new OnProgressRunnable(progress, mHost);
    Boolean result;
    return mHost->mHandler->Post(runnable, &result);
}

ECode CThemeChangeListener::OnFinish(
    /* [in] */ Boolean isSuccess)
{
    AutoPtr<IRunnable> runnable = new OnFinishRunnable(isSuccess, mHost);
    Boolean result;
    return mHost->mHandler->Post(runnable, &result);
}

}
}
}
}
