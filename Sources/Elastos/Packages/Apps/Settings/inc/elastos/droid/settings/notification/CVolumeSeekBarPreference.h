#ifndef __ELASTOS_DROID_SETTINGS_NOTIFICATION_CVOLUMESEEKBARPREFERENCE_H__
#define __ELASTOS_DROID_SETTINGS_NOTIFICATION_CVOLUMESEEKBARPREFERENCE_H__

#include "_Elastos_Droid_Settings_Notification_CVolumeSeekBarPreference.h"
#include "elastos/droid/preference/SeekBarPreference.h"
#include "elastos/droid/preference/SeekBarVolumizer.h"
#include "_Elastos.Droid.Settings.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreferenceManagerOnActivityStopListener;
using Elastos::Droid::Preference::SeekBarPreference;
using Elastos::Droid::Preference::SeekBarVolumizer;
using Elastos::Droid::Preference::ISeekBarVolumizerCallback;
using Elastos::Droid::Preference::ISeekBarVolumizer;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISeekBar;

namespace Elastos {
namespace Droid {
namespace Settings {
namespace Notification {

/** A slider preference that directly controls an audio stream volume (no dialog) **/
CarClass(CVolumeSeekBarPreference)
    , public SeekBarPreference
{
public:
    class InnerListener
        : public Object
        , public IPreferenceManagerOnActivityStopListener
    {
    public:
        CAR_INTERFACE_DECL()

        InnerListener(
            /* [in] */ CVolumeSeekBarPreference* host);

        //@Override
        CARAPI OnActivityStop();

    private:
        CVolumeSeekBarPreference* mHost;
    };

private:
    class SeekBarVolumizerCallback
        : public Object
        , public ISeekBarVolumizerCallback
    {
    public:
        TO_STRING_IMPL("CVolumeSeekBarPreference::SeekBarVolumizerCallback")

        CAR_INTERFACE_DECL()

        SeekBarVolumizerCallback(
            /* [in] */ CVolumeSeekBarPreference* host);

        //@Override
        CARAPI OnSampleStarting(
            /* [in] */ ISeekBarVolumizer* sbv);

    private:
        CVolumeSeekBarPreference* mHost;
    };

    class OnBindViewSeekBarVolumizer
        : public SeekBarVolumizer
    {
    public:
        TO_STRING_IMPL("CVolumeSeekBarPreference::OnBindViewSeekBarVolumizer")

        OnBindViewSeekBarVolumizer();

        CARAPI constructor(
            /* [in] */ CVolumeSeekBarPreference* host,
            /* [in] */ IContext* context,
            /* [in] */ Int32 streamType,
            /* [in] */ IUri* defaultUri,
            /* [in] */ ISeekBarVolumizerCallback* callback);

        // we need to piggyback on SBV's SeekBar listener to update our icon
        //@Override
        CARAPI OnProgressChanged(
            /* [in] */ ISeekBar* seekBar,
            /* [in] */ Int32 progress,
            /* [in] */ Boolean fromTouch);

    private:
        CVolumeSeekBarPreference* mHost;
    };

public:
    CAR_OBJECT_DECL()

    TO_STRING_IMPL("CVolumeSeekBarPreference")

    CVolumeSeekBarPreference();

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

    CARAPI constructor(
        /* [in] */ IContext* context);

    virtual CARAPI SetStream(
        /* [in] */ Int32 stream);

    virtual CARAPI SetCallback(
        /* [in] */ IVolumeSeekBarPreferenceCallback* callback);

    //@Override
    CARAPI OnActivityStop();

    // during initialization, this preference is the SeekBar listener
    //@Override
    CARAPI OnProgressChanged(
        /* [in] */ ISeekBar* seekBar,
        /* [in] */ Int32 progress,
        /* [in] */ Boolean fromTouch);

    virtual CARAPI ShowIcon(
        /* [in] */ Int32 resId);

protected:
    //@Override
    CARAPI OnBindView(
        /* [in] */ IView* view);

private:
    CARAPI_(AutoPtr<IUri>) GetMediaVolumeUri();

private:
    static const String TAG;

    Int32 mStream;
    AutoPtr<ISeekBar> mSeekBar;
    AutoPtr<OnBindViewSeekBarVolumizer> mVolumizer;
    AutoPtr<IVolumeSeekBarPreferenceCallback> mCallback;
    AutoPtr<IImageView> mIconView;
};

} // namespace Notification
} // namespace Settings
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_SETTINGS_NOTIFICATION_CVOLUMESEEKBARPREFERENCE_H__