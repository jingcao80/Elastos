#ifndef __ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZER_H__
#define __ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZER_H__

#include "_Elastos_Droid_Media_Audiofx_CVisualizer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Os::ILooper;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::Handler;

namespace Elastos {
namespace Droid {
namespace Media {
namespace Audiofx {

/**
 * The Visualizer class enables application to retrieve part of the currently playing audio for
 * visualization purpose. It is not an audio recording interface and only returns partial and low
 * quality audio content. However, to protect privacy of certain audio data (e.g voice mail) the use
 * of the visualizer requires the permission android.permission.RECORD_AUDIO.
 * <p>The audio session ID passed to the constructor indicates which audio content should be
 * visualized:<br>
 * <ul>
 *   <li>If the session is 0, the audio output mix is visualized</li>
 *   <li>If the session is not 0, the audio from a particular {@link android.media.MediaPlayer} or
 *   {@link android.media.AudioTrack}
 *   using this audio session is visualized </li>
 * </ul>
 * <p>Two types of representation of audio content can be captured: <br>
 * <ul>
 *   <li>Waveform data: consecutive 8-bit (unsigned) mono samples by using the
 *   {@link #getWaveForm(byte[])} method</li>
 *   <li>Frequency data: 8-bit magnitude FFT by using the {@link #getFft(byte[])} method</li>
 * </ul>
 * <p>The length of the capture can be retrieved or specified by calling respectively
 * {@link #getCaptureSize()} and {@link #setCaptureSize(int)} methods. The capture size must be a
 * power of 2 in the range returned by {@link #getCaptureSizeRange()}.
 * <p>In addition to the polling capture mode described above with {@link #getWaveForm(byte[])} and
 *  {@link #getFft(byte[])} methods, a callback mode is also available by installing a listener by
 *  use of the {@link #setDataCaptureListener(OnDataCaptureListener, int, boolean, boolean)} method.
 *  The rate at which the listener capture method is called as well as the type of data returned is
 *  specified.
 * <p>Before capturing data, the Visualizer must be enabled by calling the
 * {@link #setEnabled(boolean)} method.
 * When data capture is not needed any more, the Visualizer should be disabled.
 * <p>It is good practice to call the {@link #release()} method when the Visualizer is not used
 * anymore to free up native resources associated to the Visualizer instance.
 * <p>Creating a Visualizer on the output mix (audio session 0) requires permission
 * {@link android.Manifest.permission#MODIFY_AUDIO_SETTINGS}
 */

CarClass(CVisualizer)
    , public Object
    , public IVisualizer
{
private:
    /**
     * Helper class to handle the forwarding of native events to the appropriate listeners
     */
    class NativeEventHandler
        : public Handler
    {
    public:
        NativeEventHandler(
            /* [in] */ CVisualizer* v,
            /* [in] */ ILooper* looper);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CARAPI_(void) HandleCaptureMessage(
            /* [in]*/ IMessage* msg);

        CARAPI_(void) HandleServerDiedMessage(
            /* [in]*/ IMessage* msg);

    private:
        CVisualizer* mVisualizer;
    };

public:
    CVisualizer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    virtual ~CVisualizer();

    //--------------------------------------------------------------------------
    // Constructor, Finalize
    //--------------------
    /**
     * Class constructor.
     * @param audioSession system wide unique audio session identifier. If audioSession
     *  is not 0, the visualizer will be attached to the MediaPlayer or AudioTrack in the
     *  same audio session. Otherwise, the Visualizer will apply to the output mix.
     *
     * @throws java.lang.UnsupportedOperationException
     * @throws java.lang.RuntimeException
     */
    CARAPI constructor(
        /* [in] */ Int32 audioSession);

    /**
     * Releases the native Visualizer resources. It is a good practice to release the
     * visualization engine when not in use.
     */
    CARAPI ReleaseResources();

    /**
     * Enable or disable the visualization engine.
     * @param enabled requested enable state
     * @return {@link #SUCCESS} in case of success,
     * {@link #ERROR_INVALID_OPERATION} or {@link #ERROR_DEAD_OBJECT} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetEnabled(
        /* [in] */ Boolean enabled,
        /* [out] */ Int32* result);

    /**
     * Get current activation state of the visualizer.
     * @return true if the visualizer is active, false otherwise
     */
    CARAPI GetEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Returns the capture size range.
     * @return the mininum capture size is returned in first array element and the maximum in second
     * array element.
     */
    static CARAPI GetCaptureSizeRange(
        /* [out] */ ArrayOf<Int32>** captureSizeRange);

    /**
     * Returns the maximum capture rate for the callback capture method. This is the maximum value
     * for the rate parameter of the
     * {@link #setDataCaptureListener(OnDataCaptureListener, int, boolean, boolean)} method.
     * @return the maximum capture rate expressed in milliHertz
     */
    static CARAPI GetMaxCaptureRate(
        /* [out] */ Int32* captureRate);

    /**
     * Sets the capture size, i.e. the number of bytes returned by {@link #getWaveForm(byte[])} and
     * {@link #getFft(byte[])} methods. The capture size must be a power of 2 in the range returned
     * by {@link #getCaptureSizeRange()}.
     * This method must not be called when the Visualizer is enabled.
     * @param size requested capture size
     * @return {@link #SUCCESS} in case of success,
     * {@link #ERROR_BAD_VALUE} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetCaptureSize(
        /* [in] */ Int32 size,
        /* [out] */ Int32* result);

    /**
     * Returns current capture size.
     * @return the capture size in bytes.
     */
    CARAPI GetCaptureSize(
        /* [out] */ Int32* captureSize);

    /**
     * Set the type of scaling applied on the captured visualization data.
     * @param mode see {@link #SCALING_MODE_NORMALIZED}
     *     and {@link #SCALING_MODE_AS_PLAYED}
     * @return {@link #SUCCESS} in case of success,
     *     {@link #ERROR_BAD_VALUE} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetScalingMode(
        /* [in] */ Int32 mode,
        /* [out]*/ Int32* result);

    /**
     * Returns the current scaling mode on the captured visualization data.
     * @return the scaling mode, see {@link #SCALING_MODE_NORMALIZED}
     *     and {@link #SCALING_MODE_AS_PLAYED}.
     * @throws IllegalStateException
     */
    CARAPI GetScalingMode(
        /* [out] */ Int32* result);

    /**
     * Sets the combination of measurement modes to be performed by this audio effect.
     * @param mode a mask of the measurements to perform. The valid values are
     *     {@link #MEASUREMENT_MODE_NONE} (to cancel any measurement)
     *     or {@link #MEASUREMENT_MODE_PEAK_RMS}.
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE} in case of failure.
     * @throws IllegalStateException
     */
    CARAPI SetMeasurementMode(
        /* [in] */ Int32 mode,
        /* [out] */ Int32* result);

    /**
     * Returns the current measurement modes performed by this audio effect
     * @return the mask of the measurements,
     *     {@link #MEASUREMENT_MODE_NONE} (when no measurements are performed)
     *     or {@link #MEASUREMENT_MODE_PEAK_RMS}.
     * @throws IllegalStateException
     */
    CARAPI GetMeasurementMode(
        /* [out] */ Int32* result);

    /**
     * Returns the sampling rate of the captured audio.
     * @return the sampling rate in milliHertz.
     */
    CARAPI GetSamplingRate(
        /* [out] */ Int32* samplingRate);

    /**
     * Returns a waveform capture of currently playing audio content. The capture consists in
     * a number of consecutive 8-bit (unsigned) mono PCM samples equal to the capture size returned
     * by {@link #getCaptureSize()}.
     * <p>This method must be called when the Visualizer is enabled.
     * @param waveform array of bytes where the waveform should be returned
     * @return {@link #SUCCESS} in case of success,
     * {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or {@link #ERROR_DEAD_OBJECT}
     * in case of failure.
     * @throws IllegalStateException
     */
    CARAPI GetWaveForm(
        /* [in] */ ArrayOf<Byte>* waveform,
        /* [out] */ Int32* result);

    /**
     * Returns a frequency capture of currently playing audio content.
     * <p>This method must be called when the Visualizer is enabled.
     * <p>The capture is an 8-bit magnitude FFT, the frequency range covered being 0 (DC) to half of
     * the sampling rate returned by {@link #getSamplingRate()}. The capture returns the real and
     * imaginary parts of a number of frequency points equal to half of the capture size plus one.
     * <p>Note: only the real part is returned for the first point (DC) and the last point
     * (sampling frequency / 2).
     * <p>The layout in the returned byte array is as follows:
     * <ul>
     *   <li> n is the capture size returned by getCaptureSize()</li>
     *   <li> Rfk, Ifk are respectively  the real and imaginary parts of the kth frequency
     *   component</li>
     *   <li> If Fs is the sampling frequency retuned by getSamplingRate() the kth frequency is:
     *   (k*Fs)/(n/2) </li>
     * </ul>
     * <table border="0" cellspacing="0" cellpadding="0">
     * <tr><td>Index </p></td>
     *     <td>0 </p></td>
     *     <td>1 </p></td>
     *     <td>2 </p></td>
     *     <td>3 </p></td>
     *     <td>4 </p></td>
     *     <td>5 </p></td>
     *     <td>... </p></td>
     *     <td>n - 2 </p></td>
     *     <td>n - 1 </p></td></tr>
     * <tr><td>Data </p></td>
     *     <td>Rf0 </p></td>
     *     <td>Rf(n/2) </p></td>
     *     <td>Rf1 </p></td>
     *     <td>If1 </p></td>
     *     <td>Rf2 </p></td>
     *     <td>If2 </p></td>
     *     <td>... </p></td>
     *     <td>Rf(n-1)/2 </p></td>
     *     <td>If(n-1)/2 </p></td></tr>
     * </table>
     * @param fft array of bytes where the FFT should be returned
     * @return {@link #SUCCESS} in case of success,
     * {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or {@link #ERROR_DEAD_OBJECT}
     * in case of failure.
     * @throws IllegalStateException
     */
    CARAPI GetFft(
        /* [in] */ ArrayOf<Byte>* fft,
        /* [out] */ Int32* result);

    /**
     * Retrieves the latest peak and RMS measurement.
     * Sets the peak and RMS fields of the supplied {@link Visualizer.MeasurementPeakRms} to the
     * latest measured values.
     * @param measurement a non-null {@link Visualizer.MeasurementPeakRms} instance to store
     *    the measurement values.
     * @return {@link #SUCCESS} in case of success, {@link #ERROR_BAD_VALUE},
     *    {@link #ERROR_NO_MEMORY}, {@link #ERROR_INVALID_OPERATION} or {@link #ERROR_DEAD_OBJECT}
     *    in case of failure.
     */
    CARAPI GetMeasurementPeakRms(
        /* [in] */ IVisualizerMeasurementPeakRms * measurement,
        /* [out] */ Int32 * result);

    /**
     * Registers an OnDataCaptureListener interface and specifies the rate at which the capture
     * should be updated as well as the type of capture requested.
     * <p>Call this method with a null listener to stop receiving the capture updates.
     * @param listener OnDataCaptureListener registered
     * @param rate rate in milliHertz at which the capture should be updated
     * @param waveform true if a waveform capture is requested: the onWaveFormDataCapture()
     * method will be called on the OnDataCaptureListener interface.
     * @param fft true if a frequency capture is requested: the onFftDataCapture() method will be
     * called on the OnDataCaptureListener interface.
     * @return {@link #SUCCESS} in case of success,
     * {@link #ERROR_NO_INIT} or {@link #ERROR_BAD_VALUE} in case of failure.
     */
    CARAPI SetDataCaptureListener(
        /* [in] */ IVisualizerOnDataCaptureListener* listener,
        /* [in] */ Int32 rate,
        /* [in] */ Boolean waveform,
        /* [in] */ Boolean fft,
        /* [out] */ Int32* result);

    /**
     * @hide
     *
     * Registers an OnServerDiedListener interface.
     * <p>Call this method with a null listener to stop receiving server death notifications.
     * @return {@link #SUCCESS} in case of success,
     */
    CARAPI SetServerDiedListener(
        /* [in] */ IVisualizerOnServerDiedListener* listener,
        /* [out] */ Int32* result);

private:
    friend void captureCallback(
        /* [in] */ void* user,
        /* [in] */ uint32_t waveformSize,
        /* [in] */ uint8_t *waveform,
        /* [in] */ uint32_t fftSize,
        /* [in] */ uint8_t *fft,
        /* [in] */ uint32_t samplingrate);

    friend void elastos_media_visualizer_effect_callback(
        /* [in] */ int event,
        /* [in] */ void* user,
        /* [in] */ void* info);

    //---------------------------------------------------------
    // Interface definitions
    //--------------------
    static CARAPI_(void) Native_Init();

    CARAPI_(Int32) Native_Setup(
        /* [in] */ Int32 audioSession,
        /* [in] */ ArrayOf<Int32>* id);

    CARAPI_(void) Native_Finalize();

    CARAPI_(void) Native_Release();

    CARAPI_(Int32) Native_SetEnabled(
        /* [in] */ Boolean enabled);

    CARAPI_(Boolean) Native_GetEnabled();

    CARAPI_(Int32) Native_SetCaptureSize(
        /* [in] */ Int32 size);

    CARAPI_(Int32) Native_GetCaptureSize();

    CARAPI_(Int32) Native_SetScalingMode(
        /* [in] */ Int32 mode);

    CARAPI_(Int32) Native_GetScalingMode();

    CARAPI_(Int32) Native_SetMeasurementMode(
        /* [in] */ Int32 mode);

    CARAPI_(Int32) Native_GetMeasurementMode();

    CARAPI_(Int32) Native_GetSamplingRate();

    CARAPI_(Int32) Native_GetWaveForm(
        /* [in] */ ArrayOf<Byte>* waveform);

    CARAPI_(Int32) Native_GetFft(
        /* [in] */ ArrayOf<Byte>* fft);

    CARAPI_(Int32) Native_GetPeakRms(
        /* [in] */ IVisualizerMeasurementPeakRms * measurement);

    CARAPI_(Int32) Native_SetPeriodicCapture(
        /* [in] */ Int32 rate,
        /* [in] */ Boolean waveForm,
        /* [in] */ Boolean fft);

    // @SuppressWarnings("unused")
    static CARAPI_(Void) PostEventFromNative(
        /* [in] */ CVisualizer* effect_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

private:
    static const String TAG;

    // to keep in sync with frameworks/base/media/jni/audioeffect/android_media_Visualizer.cpp
    static const Int32 NATIVE_EVENT_PCM_CAPTURE;
    static const Int32 NATIVE_EVENT_FFT_CAPTURE;
    static const Int32 NATIVE_EVENT_SERVER_DIED;

    //--------------------------------------------------------------------------
    // Member variables
    //--------------------
    /**
     * Indicates the state of the Visualizer instance
     */
    Int32 mState;
    /**
     * Lock to synchronize access to mState
     */
    Object mStateLock;
    /**
     * System wide unique Identifier of the visualizer engine used by this Visualizer instance
     */
    Int32 mId;

    /**
     * Lock to protect listeners updates against event notifications
     */
    Object mListenerLock;
    /**
     * Handler for events coming from the native code
     */
    AutoPtr<NativeEventHandler> mNativeEventHandler;
    /**
     *  PCM and FFT capture listener registered by client
     */
    AutoPtr<IVisualizerOnDataCaptureListener> mCaptureListener;
    /**
     *  Server Died listener registered by client
     */
    AutoPtr<IVisualizerOnServerDiedListener>  mServerDiedListener;

    // accessed by native methods
    Int64 mNativeVisualizer;
    Int64 mJniData;
};

} // namespace audiofx
} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_MEDIA_AUDIOFX_CVISUALIZER_H__
