

package com.android.settings.notification;

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Media::IAudioManager;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Preference::IPreferenceManager;
using Elastos::Droid::Preference::ISeekBarPreference;
using Elastos::Droid::Preference::ISeekBarVolumizer;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::View::IView;
using Elastos::Droid::Widget::IImageView;
using Elastos::Droid::Widget::ISeekBar;

using Elastos::Droid::Settings::IR;

/** A slider preference that directly controls an audio stream volume (no dialog) **/
public class VolumeSeekBarPreference extends SeekBarPreference
        implements PreferenceManager.OnActivityStopListener {
    private static const String TAG = "VolumeSeekBarPreference";

    private Int32 mStream;
    private SeekBar mSeekBar;
    private SeekBarVolumizer mVolumizer;
    private Callback mCallback;
    private ImageView mIconView;

    public VolumeSeekBarPreference(Context context, AttributeSet attrs, Int32 defStyleAttr,
            Int32 defStyleRes) {
        Super(context, attrs, defStyleAttr, defStyleRes);
    }

    public VolumeSeekBarPreference(Context context, AttributeSet attrs, Int32 defStyleAttr) {
        This(context, attrs, defStyleAttr, 0);
    }

    public VolumeSeekBarPreference(Context context, AttributeSet attrs) {
        This(context, attrs, 0);
    }

    public VolumeSeekBarPreference(Context context) {
        This(context, NULL);
    }

    CARAPI SetStream(Int32 stream) {
        mStream = stream;
    }

    CARAPI SetCallback(Callback callback) {
        mCallback = callback;
    }

    //@Override
    CARAPI OnActivityStop() {
        if (mVolumizer != NULL) {
            mVolumizer->Stop();
        }
    }

    //@Override
    protected void OnBindView(View view) {
        super->OnBindView(view);
        if (mStream == 0) {
            Logger::W(TAG, "No stream found, not binding volumizer");
            return;
        }
        GetPreferenceManager()->RegisterOnActivityStopListener(this);
        final SeekBar seekBar = (SeekBar) view->FindViewById(R.id.seekbar);
        if (seekBar == mSeekBar) return;
        mSeekBar = seekBar;
        final SeekBarVolumizer.Callback sbvc = new SeekBarVolumizer->Callback() {
            //@Override
            CARAPI OnSampleStarting(SeekBarVolumizer sbv) {
                if (mCallback != NULL) {
                    mCallback->OnSampleStarting(sbv);
                }
            }
        };
        final Uri sampleUri = mStream == AudioManager.STREAM_MUSIC ? GetMediaVolumeUri() : NULL;
        if (mVolumizer == NULL) {
            mVolumizer = new SeekBarVolumizer(GetContext(), mStream, sampleUri, sbvc) {
                // we need to piggyback on SBV's SeekBar listener to update our icon
                //@Override
                CARAPI OnProgressChanged(SeekBar seekBar, Int32 progress,
                        Boolean fromTouch) {
                    super->OnProgressChanged(seekBar, progress, fromTouch);
                    mCallback->OnStreamValueChanged(mStream, progress);
                }
            };
        }
        mVolumizer->SetSeekBar(mSeekBar);
        mIconView = (ImageView) view->FindViewById(R.id.icon);
        mCallback->OnStreamValueChanged(mStream, mSeekBar->GetProgress());
    }

    // during initialization, this preference is the SeekBar listener
    //@Override
    CARAPI OnProgressChanged(SeekBar seekBar, Int32 progress,
            Boolean fromTouch) {
        super->OnProgressChanged(seekBar, progress, fromTouch);
        mCallback->OnStreamValueChanged(mStream, progress);
    }

    CARAPI ShowIcon(Int32 resId) {
        // Instead of using setIcon, which will trigger listeners, this just decorates the
        // preference temporarily with a new icon.
        if (mIconView != NULL) {
            mIconView->SetImageResource(resId);
        }
    }

    private Uri GetMediaVolumeUri() {
        return Uri->Parse(ContentResolver.SCHEME_ANDROID_RESOURCE + "://"
                + GetContext()->GetPackageName()
                + "/" + R.raw.media_volume);
    }

    public interface Callback {
        void OnSampleStarting(SeekBarVolumizer sbv);
        void OnStreamValueChanged(Int32 stream, Int32 progress);
    }
}
