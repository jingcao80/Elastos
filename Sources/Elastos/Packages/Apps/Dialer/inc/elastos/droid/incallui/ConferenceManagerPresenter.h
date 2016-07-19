
#ifndef __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__
#define __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__

#include "_Elastos.Droid.Dialer.h"

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

    ConferenceManagerPresenter();

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
        /* [in] */ CallList* callList);

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
        /* [in] */ CallList* callList);

private:
    static const Int32 MAX_CALLERS_IN_CONFERENCE;

    AutoPtr< ArrayOf<String> > mCallerIds;
    AutoPtr<IContext> mContext;
};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_INCALLUI_CONFERENCEMANAGERPRESENTER_H__
