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

#ifndef __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCE_H__
#define __ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCE_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/preference/SeekBarDialogPreference.h"
#include "elastos/droid/database/ContentObserver.h"
#include <elastos/core/Runnable.h>
#include <elastos/core/Object.h>

using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Media::IRingtone;
using Elastos::Droid::Net::IUri;
using Elastos::Core::Runnable;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::Widget::ISeekBarOnSeekBarChangeListener;

namespace Elastos {
namespace Droid {
namespace Preference {

/**
 * @hide
 */
class VolumePreference
    : public SeekBarDialogPreference
    , public IVolumePreference
{
public:
    class VolumeStore
        : public Object
        , public IVolumePreferenceVolumeStore
    {
    public:
        CAR_INTERFACE_DECL()

        VolumeStore();

        CARAPI constructor();

        CARAPI GetVolume(
            /* [out] */ Int32* volume);

        CARAPI GetOriginalVolume(
            /* [out] */ Int32* volume);

        CARAPI SetVolume(
            /* [in] */ Int32 volume);

        CARAPI SetOriginalVolume(
            /* [in] */ Int32 volume);

    public:
        Int32 mVolume;
        Int32 mOriginalVolume;
    };

private:
    class InnerListener
        : public Object
        , public IPreferenceManagerOnActivityStopListener
        , public IViewOnKeyListener
        , public ISeekBarVolumizerCallback
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("VolumePreference::InnerListener")

        InnerListener(
            /* [in] */ VolumePreference* host);

        CARAPI OnActivityStop();

        CARAPI OnKey(
            /* [in] */ IView* v,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* result);

        CARAPI OnSampleStarting(
            /* [in] */ ISeekBarVolumizer* volumizer);

    private:
        VolumePreference* mHost;
    };

public:
    CAR_INTERFACE_DECL()

    VolumePreference();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Int32 defStyleAttr);

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI SetStreamType(
        /* [in] */ Int32 streamType);

    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI OnActivityStop();

    CARAPI OnSampleStarting(
        /* [in] */ ISeekBarVolumizer* volumizer);

protected:
    CARAPI OnBindDialogView(
        /* [in] */ IView* view);

    CARAPI OnDialogClosed(
        /* [in] */ Boolean positiveResult);

    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** state);

    CARAPI OnRestoreInstanceState(
        /* [in] */ IParcelable* state);

private:
    CARAPI_(void) Cleanup();

public:
    const static String TAG;

private:
    Int32 mStreamType;
    AutoPtr<ISeekBarVolumizer> mSeekBarVolumizer;
    AutoPtr<InnerListener> mListener;
};

}
}
}

#endif //__ELASTOS_DROID_PREFERENCE_VOLUMEPREFERENCE_H__
