#ifndef _ELASTOS_DROID_PROVIDERS_MEDIA_RINGTONEPICKERACTIVITY_H__
#define _ELASTOS_DROID_PROVIDERS_MEDIA_RINGTONEPICKERACTIVITY_H__

#include "_Elastos.Droid.Providers.Media.h"
#include "Elastos.Droid.Widget.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include <elastos/droid/os/Handler.h>
#include <elastos/droid/os/Runnable.h>
#include <elastos/droid/internal/app/AlertActivity.h>

using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Content::IDialogInterfaceOnClickListener;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Media::IRingtoneManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Internal::App::AlertActivity;
using Elastos::Droid::Internal::App::IAlertControllerAlertParamsOnPrepareListViewListener;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IAdapterView;
using Elastos::Droid::Widget::IAdapterViewOnItemSelectedListener;
using Elastos::Droid::Widget::IListView;
using Elastos::Core::IRunnable;

namespace Elastos {
namespace Droid {
namespace Providers {
namespace Media {

class RingtonePickerActivity
    : public AlertActivity
    , public IAdapterViewOnItemSelectedListener
    , public IRunnable
    , public IDialogInterfaceOnClickListener
    , public IAlertControllerAlertParamsOnPrepareListViewListener
    , public IRingtonePickerActivity
{
private:
    class MyDialogInterfaceOnClickListener
        : public Object
        , public IDialogInterfaceOnClickListener
    {
        friend class RingtonePickerActivity;

    public:
        MyDialogInterfaceOnClickListener(
            /* [in] */ RingtonePickerActivity* owner);

        CAR_INTERFACE_DECL()

        CARAPI OnClick(
            /* [in] */ IDialogInterface* dialog,
            /* [in] */ Int32 which);

    private:
        RingtonePickerActivity* mOwner;
    };

    class MyRunnable
        : public Runnable
    {
        friend class RingtonePickerActivity;
    public:
        MyRunnable(
            /* [in] */ RingtonePickerActivity* owner);

        CARAPI Run();

    private:
        RingtonePickerActivity* mOwner;
    };

public:
    CAR_INTERFACE_DECL()

    RingtonePickerActivity();

    virtual ~RingtonePickerActivity();

    CARAPI OnSaveInstanceState(
        /* [in] */ IBundle* outState);

    CARAPI OnPrepareListView(
        /* [in] */ IListView* listView);

    CARAPI OnClick(
        /* [in] */ IDialogInterface* dialog,
        /* [in] */ Int32 which);

    CARAPI OnItemSelected(
        /* [in] */ IAdapterView* parent,
        /* [in] */ IView* view,
        /* [in] */ Int32 position,
        /* [in] */ Int64 id);

    CARAPI OnNothingSelected(
        /* [in] */ IAdapterView* parent);

    CARAPI Run();

protected:
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    CARAPI OnStop();

    CARAPI OnPause();

private:
    CARAPI_(Int32) AddStaticItem(
        /* [in] */ IListView* listView,
        /* [in] */ Int32 textResId);

    CARAPI_(Int32) AddDefaultRingtoneItem(
        /* [in] */ IListView* listView);

    CARAPI_(Int32) AddSilentItem(
        /* [in] */ IListView* listView);

    CARAPI_(void) PlayRingtone(
        /* [in] */ Int32 position,
        /* [in] */ Int32 delayMs);

    CARAPI_(void) SaveAnyPlayingRingtone();

    CARAPI_(void) StopAnyPlayingRingtone();

    CARAPI_(Int32) GetRingtoneManagerPosition(
        /* [in] */ Int32 listPos);

    CARAPI_(Int32) GetListPosition(
        /* [in] */ Int32 ringtoneManagerPos);

private:
    static const Int32 POS_UNKNOWN;

    static const String TAG;

    static const Int32 DELAY_MS_SELECTION_PLAYED;

    static const String SAVE_CLICKED_POS;

    AutoPtr<IRingtoneManager> mRingtoneManager;
    Int32 mType;
    AutoPtr<ICursor> mCursor;
    AutoPtr<Handler> mHandler;

    /** The position in the list of the 'Silent' item. */
    Int32 mSilentPos;

    /** The position in the list of the 'Default' item. */
    Int32 mDefaultRingtonePos;

    /** The position in the list of the last clicked item. */
    Int32 mClickedPos;

    /** The position in the list of the ringtone to sample. */
    Int32 mSampleRingtonePos;

    /** Whether this list has the 'Silent' item. */
    Boolean mHasSilentItem;

    /** The Uri to place a checkmark next to. */
    AutoPtr<IUri> mExistingUri;

    /** The number of static items in the list. */
    Int32 mStaticItemCount;

    /** Whether this list has the 'Default' item. */
    Boolean mHasDefaultItem;

    /** The Uri to play when the 'Default' item is clicked. */
    AutoPtr<IUri> mUriForDefaultItem;

    /**
     * A Ringtone for the default ringtone. In most cases, the RingtoneManager
     * will stop the previous ringtone. However, the RingtoneManager doesn't
     * manage the default ringtone for us, so we should stop this one manually.
     */
    AutoPtr<IRingtone> mDefaultRingtone;

    /**
     * The ringtone that's currently playing, unless the currently playing one is the default
     * ringtone.
     */
    AutoPtr<IRingtone> mCurrentRingtone;

    /**
     * Keep the currently playing ringtone around when changing orientation, so that it
     * can be stopped later, after the activity is recreated.
     */
    static AutoPtr<IRingtone> sPlayingRingtone;

    AutoPtr<IDialogInterfaceOnClickListener> mRingtoneClickListener;
};

} // namespace Media
} // namespace Providers
} // namespace Droid
} // namespace Elastos

#endif //_ELASTOS_DROID_PROVIDERS_MEDIA_RINGTONEPICKERACTIVITY_H__