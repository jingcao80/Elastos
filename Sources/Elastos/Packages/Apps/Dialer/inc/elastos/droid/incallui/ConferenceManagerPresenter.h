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

#ifndef __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"
#include "elastos/droid/incallui/CallList.h"
#include "elastos/droid/incallui/ContactInfoCache.h"
#include "elastos/droid/incallui/Presenter.h"

using Elastos::Droid::Content::IContext;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class ConferenceManagerPresenter
    : public Presenter
    , public IInCallStateListener
{
public:
    CAR_INTERFACE_DECL();

    // @Override
    CARAPI OnUiReady(
        /* [in] */ IUi* ui);

    // @Override
    CARAPI OnUiUnready(
        /* [in] */ IUi* ui);

    CARAPI OnStateChange(
        /* [in] */ InCallState oldState,
        /* [in] */ InCallState newState,
        /* [in] */ ICallList* callList);

    CARAPI_(void) Init(
        /* [in] */ IContext* context,
        /* [in] */ ICallList* callList);

    /**
      * Updates a single row of the "Manage conference" UI.  (One row in this
      * UI represents a single caller in the conference.)
      *
      * @param i the row to update
      * @param contactCacheEntry the contact details corresponding to this caller.
      *        If null, that means this is an "empty slot" in the conference,
      *        so hide this row in the UI.
      * @param canSeparate if true, show a "Separate" (i.e. "Private") button
      *        on this row in the UI.
      * @param canDisconnect if true, show a "Disconnect" button on this row in the UI.
      */
    CARAPI_(void) UpdateManageConferenceRow(
        /* [in] */ Int32 i,
        /* [in] */ ContactInfoCache::ContactCacheEntry* contactCacheEntry,
        /* [in] */ Boolean canSeparate,
        /* [in] */ Boolean canDisconnect);

    CARAPI_(Int32) GetMaxCallersInConference();

    CARAPI_(void) SeparateConferenceConnection(
        /* [in] */ Int32 rowId);

    CARAPI_(void) EndConferenceConnection(
        /* [in] */ Int32 rowId);

private:
    CARAPI_(void) Update(
        /* [in] */ ICallList* callList);

private:
    static const Int32 MAX_CALLERS_IN_CONFERENCE;

    AutoPtr< ArrayOf<String> > mCallerIds;
    AutoPtr<IContext> mContext;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__
