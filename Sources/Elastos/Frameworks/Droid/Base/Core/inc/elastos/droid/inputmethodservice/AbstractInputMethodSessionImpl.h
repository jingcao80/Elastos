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

#ifndef __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODSESSIONIMPL_H__
#define __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODSESSIONIMPL_H__

#include "Elastos.Droid.InputMethodService.h"
#include "elastos/droid/inputmethodservice/AbstractInputMethodService.h"

using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::View::InputMethod::ILocalInputMethodSessionEventCallback;
using Elastos::Droid::View::InputMethod::IInputMethodSession;

namespace Elastos {
namespace Droid {
namespace InputMethodService {

/**
 * Base class for derived classes to implement their {@link InputMethodSession}
 * interface.  This takes care of basic maintenance of the session,
 * but most behavior must be implemented in a derived class.
 */
class ECO_PUBLIC AbstractInputMethodSessionImpl
    : public Object
    , public IAbstractInputMethodSessionImpl
    , public IInputMethodSession
{
public:
    CAR_INTERFACE_DECL()

    AbstractInputMethodSessionImpl(
        /* [in] */ AbstractInputMethodService* host);

    /**
     * Check whether this session has been enabled by the system.  If not
     * enabled, you should not execute any calls on to it.
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Check whether this session has been revoked by the system.  Revoked
     * session is also always disabled, so there is generally no need to
     * explicitly check for this.
     */
    CARAPI IsRevoked(
        /* [out] */ Boolean* revoked);

    /**
     * Change the enabled state of the session.  This only works if the
     * session has not been revoked.
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled);

    /**
     * Revoke the session from the client.  This disabled the session, and
     * prevents it from ever being enabled again.
     */
    CARAPI RevokeSelf();

    /**
     * Take care of dispatching incoming key events to the appropriate
     * callbacks on the service, and tell the client when this is done.
     */
    CARAPI DispatchKeyEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IKeyEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* callback);

    /**
     * Take care of dispatching incoming trackball events to the appropriate
     * callbacks on the service, and tell the client when this is done.
     */
    CARAPI DispatchTrackballEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IMotionEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* callback);

    CARAPI DispatchGenericMotionEvent(
        /* [in] */ Int32 seq,
        /* [in] */ IMotionEvent* event,
        /* [in] */ ILocalInputMethodSessionEventCallback* callback);

private:
    Boolean mEnabled;
    Boolean mRevoked;

protected:
    AbstractInputMethodService* mAbstractHost;
};

} // namespace InputMethodService
} // namespace Droid
} // namespace Elastos

#endif  // __ELASTOS_DROID_INPUTMETHODSERVICE_ABSTRACTINPUTMETHODSESSIONIMPL_H__
