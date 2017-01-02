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

#ifndef __ELASTOS_DROID_VIEW_WINDOWIDFOCUSOBSERVER_H__
#define __ELASTOS_DROID_VIEW_WINDOWIDFOCUSOBSERVER_H__

#include "Elastos.Droid.View.h"
#include <elastos/droid/ext/frameworkext.h>
#include <elastos/core/Object.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Core::Object;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace View {

/**
 * Subclass for observing changes to the focus state of an {@link WindowId}.
 * You should use the same instance of this class for observing multiple
 * {@link WindowId} objects, since this class is fairly heavy-weight -- the
 * base class includes all of the mechanisms for connecting to and receiving updates
 * from the window.
 */
class ECO_PUBLIC WindowIdFocusObserver
    : public Object
    , public IWindowIdFocusObserver
{
private:
    class H
        : public Handler
    {
    public:
        TO_STRING_IMPL("WindowIdFocusObserver::H")

        H(
            /* [in] */ WindowIdFocusObserver* host);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        WindowIdFocusObserver* mHost;
    };

public:
    /**
     * Construct a new observer.  This observer will be configured so that all
     * of its callbacks are dispatched on the current calling thread.
     */
    WindowIdFocusObserver();

    CARAPI constructor();

    /**
     * Called when one of the monitored windows gains input focus.
     */
    virtual CARAPI OnFocusGained(
        /* [in] */ IWindowId* token) = 0;

    /**
     * Called when one of the monitored windows loses input focus.
     */
    virtual CARAPI OnFocusLost(
        /* [in] */ IWindowId* token) = 0;

public:
    AutoPtr<IIWindowFocusObserver> mIObserver;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IHashMap> mRegistrations;
};


} // namespace View
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_VIEW_WINDOWIDFOCUSOBSERVER_H__
