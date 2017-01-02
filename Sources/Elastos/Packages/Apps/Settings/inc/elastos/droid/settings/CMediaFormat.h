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

#ifndef __ELASTOS_DROID_SETTINGS_CMEDIAFORMAT_H__
#define __ELASTOS_DROID_SETTINGS_CMEDIAFORMAT_H__

#include "_Elastos_Droid_Settings_CMediaFormat.h"
#include "elastos/droid/app/Activity.h"

using Elastos::Droid::App::Activity;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::View::ILayoutInflater;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::Widget::IButton;

namespace Elastos {
namespace Droid {
namespace Settings {

/**
 * Confirm and execute a format of the sdcard.
 * Multiple confirmations are required: first, a general "are you sure
 * you want to do this?" prompt, followed by a keyguard pattern trace if the user
 * has defined one, followed by a final strongly-worded "THIS WILL ERASE EVERYTHING
 * ON THE SD CARD" prompt.  If at any time the phone is allowed to go to sleep, is
 * locked, et cetera, then the confirmation sequence is abandoned.
 */
CarClass(CMediaFormat)
    , public Activity
{
private:
    class InitViewOnClickListener
        : public Object
        , public IViewOnClickListener
    {
    public:
        TO_STRING_IMPL("CMediaFormat::InitViewOnClickListener")

        CAR_INTERFACE_DECL()

        InitViewOnClickListener(
            /* [in] */ CMediaFormat* host,
            /* [in] */ Int32 id);

        CARAPI OnClick(
            /* [in] */ IView* v);

    private:
        CMediaFormat* mHost;
        Int32 mId;
    };

public:
    TO_STRING_IMPL("CMediaFormat")

    CAR_OBJECT_DECL()

    CMediaFormat();

    virtual ~CMediaFormat();

    CARAPI constructor();

    /** Abandon all progress through the confirmation sequence by returning
     * to the initial view any time the activity is interrupted (e.g. by
     * idle timeout).
     */
    //@Override
    CARAPI OnPause();

protected:
    //@Override
    CARAPI OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    //@Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedState);

private:
    /**
     *  Keyguard validation is run using the standard {@link ConfirmLockPattern}
     * component as a subactivity
     */
    CARAPI_(Boolean) RunKeyguardConfirmation(
        /* [in] */ Int32 request);

    /**
     * Configure the UI for the final confirmation interaction
     */
    CARAPI_(void) EstablishFinalConfirmationState();

    /**
     * In its initial state, the activity presents a button for the user to
     * click in order to initiate a confirmation sequence.  This method is
     * called from various other points in the code to reset the activity to
     * this base state.
     *
     * <p>Reinflating views from resources is expensive and prevents us from
     * caching widget pointers, so we use a single-inflate pattern:  we lazy-
     * inflate each view, caching all of the widget pointers we'll need at the
     * time, then simply reuse the inflated views directly whenever we need
     * to change contents.
     */
    CARAPI_(void) EstablishInitialState();

private:
    static const Int32 KEYGUARD_REQUEST = 55;

    AutoPtr<ILayoutInflater> mInflater;

    AutoPtr<IView> mInitialView;
    AutoPtr<IButton> mInitiateButton;

    AutoPtr<IView> mFinalView;
    AutoPtr<IButton> mFinalButton;

    /**
     * The user has gone through the multiple confirmation, so now we go ahead
     * and invoke the Mount Service to format the SD card.
     */
    AutoPtr<IViewOnClickListener> mFinalClickListener;
    /**
     * If the user clicks to begin the reset sequence, we next require a
     * keyguard confirmation if the user has currently enabled one.  If there
     * is no keyguard available, we simply go to the final confirmation prompt.
     */
    AutoPtr<IViewOnClickListener> mInitiateListener;
};

} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_CMEDIAFORMAT_H__
