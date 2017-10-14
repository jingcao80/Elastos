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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_STREAMCONFIGURATIONMAP_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_STREAMCONFIGURATIONMAP_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Utility.h"
#include "elastos/droid/utility/Range.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>

using Elastos::Droid::Utility::ISize;
using Elastos::Droid::Utility::Range;
using Elastos::Droid::View::ISurface;
using Elastos::Core::Object;
using Elastos::Core::IInteger32;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Utility::ISize);
DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Droid::Utility::Range<IInteger32>);

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

class StreamConfigurationMap
    : public Object
    , public IStreamConfigurationMap
{
public:
    CAR_INTERFACE_DECL()

    StreamConfigurationMap();

    ~StreamConfigurationMap();

    CARAPI constructor();

    /**
     * Create a new {@link StreamConfigurationMap}.
     *
     * <p>The array parameters ownership is passed to this object after creation; do not
     * write to them after this constructor is invoked.</p>
     *
     * @param configurations a non-{@code null} array of {@link StreamConfiguration}
     * @param minFrameDurations a non-{@code null} array of {@link StreamConfigurationDuration}
     * @param stallDurations a non-{@code null} array of {@link StreamConfigurationDuration}
     * @param highSpeedVideoConfigurations an array of {@link HighSpeedVideoConfiguration}, null if
     *        camera device does not support high speed video recording
     *
     * @throws NullPointerException if any of the arguments except highSpeedVideoConfigurations
     *         were {@code null} or any subelements were {@code null}
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ ArrayOf<IStreamConfiguration*>* configurations,
        /* [in] */ ArrayOf<IStreamConfigurationDuration*>* minFrameDurations,
        /* [in] */ ArrayOf<IStreamConfigurationDuration*>* stallDurations,
        /* [in] */ ArrayOf<IHighSpeedVideoConfiguration*>* highSpeedVideoConfigurations);

    /**
     * Get the image {@code format} output formats in this stream configuration.
     *
     * <p>All image formats returned by this function will be defined in either {@link ImageFormat}
     * or in {@link PixelFormat} (and there is no possibility of collision).</p>
     *
     * <p>Formats listed in this array are guaranteed to return true if queried with
     * {@link #isOutputSupportedFor(int)}.</p>
     *
     * @return an array of integer format
     *
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetOutputFormats(
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    /**
     * Get the image {@code format} input formats in this stream configuration.
     *
     * <p>All image formats returned by this function will be defined in either {@link ImageFormat}
     * or in {@link PixelFormat} (and there is no possibility of collision).</p>
     *
     * @return an array of integer format
     *
     * @see ImageFormat
     * @see PixelFormat
     *
     * @hide
     */
    CARAPI GetInputFormats(
        /* [out, callee] */ ArrayOf<Int32>** outarr);

    /**
     * Get the supported input sizes for this input format.
     *
     * <p>The format must have come from {@link #getInputFormats}; otherwise
     * {@code null} is returned.</p>
     *
     * @param format a format from {@link #getInputFormats}
     * @return a non-empty array of sizes, or {@code null} if the format was not available.
     *
     * @hide
     */
    CARAPI GetInputSizes(
        /* [in] */ Int32 format,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Determine whether or not output surfaces with a particular user-defined format can be passed
     * {@link CameraDevice#createCaptureSession createCaptureSession}.
     *
     * <p>This method determines that the output {@code format} is supported by the camera device;
     * each output {@code surface} target may or may not itself support that {@code format}.
     * Refer to the class which provides the surface for additional documentation.</p>
     *
     * <p>Formats for which this returns {@code true} are guaranteed to exist in the result
     * returned by {@link #getOutputSizes}.</p>
     *
     * @param format an image format from either {@link ImageFormat} or {@link PixelFormat}
     * @return
     *          {@code true} iff using a {@code surface} with this {@code format} will be
     *          supported with {@link CameraDevice#createCaptureSession}
     *
     * @throws IllegalArgumentException
     *          if the image format was not a defined named constant
     *          from either {@link ImageFormat} or {@link PixelFormat}
     *
     * @see ImageFormat
     * @see PixelFormat
     * @see CameraDevice#createCaptureSession
     */
    CARAPI IsOutputSupportedFor(
        /* [in] */ Int32 format,
        /* [out] */ Boolean* value);

    /**
     * Determine whether or not output streams can be configured with a particular class
     * as a consumer.
     *
     * <p>The following list is generally usable for outputs:
     * <ul>
     * <li>{@link android.media.ImageReader} -
     * Recommended for image processing or streaming to external resources (such as a file or
     * network)
     * <li>{@link android.media.MediaRecorder} -
     * Recommended for recording video (simple to use)
     * <li>{@link android.media.MediaCodec} -
     * Recommended for recording video (more complicated to use, with more flexibility)
     * <li>{@link android.renderscript.Allocation} -
     * Recommended for image processing with {@link android.renderscript RenderScript}
     * <li>{@link android.view.SurfaceHolder} -
     * Recommended for low-power camera preview with {@link android.view.SurfaceView}
     * <li>{@link android.graphics.SurfaceTexture} -
     * Recommended for OpenGL-accelerated preview processing or compositing with
     * {@link android.view.TextureView}
     * </ul>
     * </p>
     *
     * <p>Generally speaking this means that creating a {@link Surface} from that class <i>may</i>
     * provide a producer endpoint that is suitable to be used with
     * {@link CameraDevice#createCaptureSession}.</p>
     *
     * <p>Since not all of the above classes support output of all format and size combinations,
     * the particular combination should be queried with {@link #isOutputSupportedFor(Surface)}.</p>
     *
     * @param klass a non-{@code null} {@link Class} object reference
     * @return {@code true} if this class is supported as an output, {@code false} otherwise
     *
     * @throws NullPointerException if {@code klass} was {@code null}
     *
     * @see CameraDevice#createCaptureSession
     * @see #isOutputSupportedFor(Surface)
     */
    static CARAPI IsOutputSupportedFor(
        /* [in] */ IClassInfo* klass,
        /* [out] */ Boolean* value);

    /**
     * Determine whether or not the {@code surface} in its current state is suitable to be included
     * in a {@link CameraDevice#createCaptureSession capture session} as an output.
     *
     * <p>Not all surfaces are usable with the {@link CameraDevice}, and not all configurations
     * of that {@code surface} are compatible. Some classes that provide the {@code surface} are
     * compatible with the {@link CameraDevice} in general
     * (see {@link #isOutputSupportedFor(Class)}, but it is the caller's responsibility to put the
     * {@code surface} into a state that will be compatible with the {@link CameraDevice}.</p>
     *
     * <p>Reasons for a {@code surface} being specifically incompatible might be:
     * <ul>
     * <li>Using a format that's not listed by {@link #getOutputFormats}
     * <li>Using a format/size combination that's not listed by {@link #getOutputSizes}
     * <li>The {@code surface} itself is not in a state where it can service a new producer.</p>
     * </li>
     * </ul>
     *
     * This is not an exhaustive list; see the particular class's documentation for further
     * possible reasons of incompatibility.</p>
     *
     * @param surface a non-{@code null} {@link Surface} object reference
     * @return {@code true} if this is supported, {@code false} otherwise
     *
     * @throws NullPointerException if {@code surface} was {@code null}
     *
     * @see CameraDevice#createCaptureSession
     * @see #isOutputSupportedFor(Class)
     */
    CARAPI IsOutputSupportedFor(
        /* [in] */ ISurface* surface,
        /* [out] */ Boolean* value);

    /**
     * Get a list of sizes compatible with {@code klass} to use as an output.
     *
     * <p>Since some of the supported classes may support additional formats beyond
     * an opaque/implementation-defined (under-the-hood) format; this function only returns
     * sizes for the implementation-defined format.</p>
     *
     * <p>Some classes such as {@link android.media.ImageReader} may only support user-defined
     * formats; in particular {@link #isOutputSupportedFor(Class)} will return {@code true} for
     * that class and this method will return an empty array (but not {@code null}).</p>
     *
     * <p>If a well-defined format such as {@code NV21} is required, use
     * {@link #getOutputSizes(int)} instead.</p>
     *
     * <p>The {@code klass} should be a supported output, that querying
     * {@code #isOutputSupportedFor(Class)} should return {@code true}.</p>
     *
     * @param klass
     *          a non-{@code null} {@link Class} object reference
     * @return
     *          an array of supported sizes for implementation-defined formats,
     *          or {@code null} iff the {@code klass} is not a supported output
     *
     * @throws NullPointerException if {@code klass} was {@code null}
     *
     * @see #isOutputSupportedFor(Class)
     */
    CARAPI GetOutputSizes(
        /* [in] */ IClassInfo* klass,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Get a list of sizes compatible with the requested image {@code format}.
     *
     * <p>The {@code format} should be a supported format (one of the formats returned by
     * {@link #getOutputFormats}).</p>
     *
     * @param format an image format from {@link ImageFormat} or {@link PixelFormat}
     * @return
     *          an array of supported sizes,
     *          or {@code null} if the {@code format} is not a supported output
     *
     * @see ImageFormat
     * @see PixelFormat
     * @see #getOutputFormats
     */
    CARAPI GetOutputSizes(
        /* [in] */ Int32 format,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Get a list of supported high speed video recording sizes.
     *
     * <p> When HIGH_SPEED_VIDEO is supported in
     * {@link CameraCharacteristics#CONTROL_AVAILABLE_SCENE_MODES available scene modes}, this
     * method will list the supported high speed video size configurations. All the sizes listed
     * will be a subset of the sizes reported by {@link #getOutputSizes} for processed non-stalling
     * formats (typically ImageFormat#YUV_420_888, ImageFormat#NV21, ImageFormat#YV12)</p>
     *
     * <p> To enable high speed video recording, application must set
     * {@link CaptureRequest#CONTROL_SCENE_MODE} to
     * {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} in capture
     * requests and select the video size from this method and
     * {@link CaptureRequest#CONTROL_AE_TARGET_FPS_RANGE FPS range} from
     * {@link #getHighSpeedVideoFpsRangesFor} to configure the recording and preview streams and
     * setup the recording requests. For example, if the application intends to do high speed
     * recording, it can select the maximum size reported by this method to configure output
     * streams. Note that for the use case of multiple output streams, application must select one
     * unique size from this method to use. Otherwise a request error might occur. Once the size is
     * selected, application can get the supported FPS ranges by
     * {@link #getHighSpeedVideoFpsRangesFor}, and use these FPS ranges to setup the recording
     * requests.</p>
     *
     * @return
     *          an array of supported high speed video recording sizes
     *
     * @see #getHighSpeedVideoFpsRangesFor(Size)
     */
    CARAPI GetHighSpeedVideoSizes(
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Get the frame per second ranges (fpsMin, fpsMax) for input high speed video size.
     *
     * <p> See {@link #getHighSpeedVideoSizes} for how to enable high speed recording.</p>
     *
     * <p> For normal video recording use case, where some application will NOT set
     * {@link CaptureRequest#CONTROL_SCENE_MODE} to
     * {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} in capture
     * requests, the {@link CaptureRequest#CONTROL_AE_TARGET_FPS_RANGE FPS ranges} reported in
     * this method must not be used to setup capture requests, or it will cause request error.</p>
     *
     * @param size one of the sizes returned by {@link #getHighSpeedVideoSizes()}
     * @return
     *          An array of FPS range to use with
     *          {@link CaptureRequest#CONTROL_AE_TARGET_FPS_RANGE TARGET_FPS_RANGE} when using
     *          {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} scene
     *          mode.
     *          The upper bound of returned ranges is guaranteed to be larger or equal to 60.
     *
     * @throws IllegalArgumentException if input size does not exist in the return value of
     *         getHighSpeedVideoSizes
     * @see #getHighSpeedVideoSizes()
     */
    CARAPI GetHighSpeedVideoFpsRangesFor(
        /* [in] */ ISize* size,
        /* [out, callee] */ ArrayOf<IInterface*>** outarr);

    /**
     * Get a list of supported high speed video recording FPS ranges.
     *
     * <p> When HIGH_SPEED_VIDEO is supported in
     * {@link CameraCharacteristics#CONTROL_AVAILABLE_SCENE_MODES available scene modes}, this
     * method will list the supported high speed video FPS range configurations. Application can
     * then use {@link #getHighSpeedVideoSizesFor} to query available sizes for one of returned
     * FPS range.</p>
     *
     * <p> To enable high speed video recording, application must set
     * {@link CaptureRequest#CONTROL_SCENE_MODE} to
     * {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} in capture
     * requests and select the video size from {@link #getHighSpeedVideoSizesFor} and
     * {@link CaptureRequest#CONTROL_AE_TARGET_FPS_RANGE FPS range} from
     * this method to configure the recording and preview streams and setup the recording requests.
     * For example, if the application intends to do high speed recording, it can select one FPS
     * range reported by this method, query the video sizes corresponding to this FPS range  by
     * {@link #getHighSpeedVideoSizesFor} and select one of reported sizes to configure output
     * streams. Note that for the use case of multiple output streams, application must select one
     * unique size from {@link #getHighSpeedVideoSizesFor}, and use it for all output streams.
     * Otherwise a request error might occur when attempting to enable
     * {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO}.
     * Once the stream is configured, application can set the FPS range in the recording requests.
     * </p>
     *
     * @return
     *          an array of supported high speed video recording FPS ranges
     *          The upper bound of returned ranges is guaranteed to be larger or equal to 60.
     *
     * @see #getHighSpeedVideoSizesFor
     */
    // @SuppressWarnings("unchecked")
    CARAPI GetHighSpeedVideoFpsRanges(
        /* [out, callee] */ ArrayOf<IInterface*>** outarr);

    /**
     * Get the supported video sizes for input FPS range.
     *
     * <p> See {@link #getHighSpeedVideoFpsRanges} for how to enable high speed recording.</p>
     *
     * <p> For normal video recording use case, where the application will NOT set
     * {@link CaptureRequest#CONTROL_SCENE_MODE} to
     * {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} in capture
     * requests, the {@link CaptureRequest#CONTROL_AE_TARGET_FPS_RANGE FPS ranges} reported in
     * this method must not be used to setup capture requests, or it will cause request error.</p>
     *
     * @param fpsRange one of the FPS range returned by {@link #getHighSpeedVideoFpsRanges()}
     * @return
     *          An array of video sizes to configure output stream when using
     *          {@link CaptureRequest#CONTROL_SCENE_MODE_HIGH_SPEED_VIDEO HIGH_SPEED_VIDEO} scene
     *          mode.
     *
     * @throws IllegalArgumentException if input FPS range does not exist in the return value of
     *         getHighSpeedVideoFpsRanges
     * @see #getHighSpeedVideoFpsRanges()
     */
    CARAPI GetHighSpeedVideoSizesFor(
        /* [in] */ IInterface* fpsRange,
        /* [out, callee] */ ArrayOf<ISize*>** outarr);

    /**
     * Get the minimum {@link CaptureRequest#SENSOR_FRAME_DURATION frame duration}
     * for the format/size combination (in nanoseconds).
     *
     * <p>{@code format} should be one of the ones returned by {@link #getOutputFormats()}.</p>
     * <p>{@code size} should be one of the ones returned by
     * {@link #getOutputSizes(int)}.</p>
     *
     * <p>This should correspond to the frame duration when only that stream is active, with all
     * processing (typically in {@code android.*.mode}) set to either {@code OFF} or {@code FAST}.
     * </p>
     *
     * <p>When multiple streams are used in a request, the minimum frame duration will be
     * {@code max(individual stream min durations)}.</p>
     *
     * <p>For devices that do not support manual sensor control
     * ({@link android.hardware.camera2.CameraMetadata#REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR}),
     * this function may return 0.</p>
     *
     * <!--
     * TODO: uncomment after adding input stream support
     * <p>The minimum frame duration of a stream (of a particular format, size) is the same
     * regardless of whether the stream is input or output.</p>
     * -->
     *
     * @param format an image format from {@link ImageFormat} or {@link PixelFormat}
     * @param size an output-compatible size
     * @return a minimum frame duration {@code >} 0 in nanoseconds, or
     *          0 if the minimum frame duration is not available.
     *
     * @throws IllegalArgumentException if {@code format} or {@code size} was not supported
     * @throws NullPointerException if {@code size} was {@code null}
     *
     * @see CaptureRequest#SENSOR_FRAME_DURATION
     * @see #getOutputStallDuration(int, Size)
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetOutputMinFrameDuration(
        /* [in] */ Int32 format,
        /* [in] */ ISize* size,
        /* [out] */Int64* value);

    /**
     * Get the minimum {@link CaptureRequest#SENSOR_FRAME_DURATION frame duration}
     * for the class/size combination (in nanoseconds).
     *
     * <p>This assumes a the {@code klass} is set up to use an implementation-defined format.
     * For user-defined formats, use {@link #getOutputMinFrameDuration(int, Size)}.</p>
     *
     * <p>{@code klass} should be one of the ones which is supported by
     * {@link #isOutputSupportedFor(Class)}.</p>
     *
     * <p>{@code size} should be one of the ones returned by
     * {@link #getOutputSizes(int)}.</p>
     *
     * <p>This should correspond to the frame duration when only that stream is active, with all
     * processing (typically in {@code android.*.mode}) set to either {@code OFF} or {@code FAST}.
     * </p>
     *
     * <p>When multiple streams are used in a request, the minimum frame duration will be
     * {@code max(individual stream min durations)}.</p>
     *
     * <p>For devices that do not support manual sensor control
     * ({@link android.hardware.camera2.CameraMetadata#REQUEST_AVAILABLE_CAPABILITIES_MANUAL_SENSOR}),
     * this function may return 0.</p>
     *
     * <!--
     * TODO: uncomment after adding input stream support
     * <p>The minimum frame duration of a stream (of a particular format, size) is the same
     * regardless of whether the stream is input or output.</p>
     * -->
     *
     * @param klass
     *          a class which is supported by {@link #isOutputSupportedFor(Class)} and has a
     *          non-empty array returned by {@link #getOutputSizes(Class)}
     * @param size an output-compatible size
     * @return a minimum frame duration {@code >} 0 in nanoseconds, or
     *          0 if the minimum frame duration is not available.
     *
     * @throws IllegalArgumentException if {@code klass} or {@code size} was not supported
     * @throws NullPointerException if {@code size} or {@code klass} was {@code null}
     *
     * @see CaptureRequest#SENSOR_FRAME_DURATION
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetOutputMinFrameDuration(
        /* [in] */ IClassInfo* klass,
        /* [in] */ ISize* size,
        /* [out] */Int64* value);

    /**
     * Get the stall duration for the format/size combination (in nanoseconds).
     *
     * <p>{@code format} should be one of the ones returned by {@link #getOutputFormats()}.</p>
     * <p>{@code size} should be one of the ones returned by
     * {@link #getOutputSizes(int)}.</p>
     *
     * <p>
     * A stall duration is how much extra time would get added to the normal minimum frame duration
     * for a repeating request that has streams with non-zero stall.
     *
     * <p>For example, consider JPEG captures which have the following characteristics:
     *
     * <ul>
     * <li>JPEG streams act like processed YUV streams in requests for which they are not included;
     * in requests in which they are directly referenced, they act as JPEG streams.
     * This is because supporting a JPEG stream requires the underlying YUV data to always be ready
     * for use by a JPEG encoder, but the encoder will only be used (and impact frame duration) on
     * requests that actually reference a JPEG stream.
     * <li>The JPEG processor can run concurrently to the rest of the camera pipeline, but cannot
     * process more than 1 capture at a time.
     * </ul>
     *
     * <p>In other words, using a repeating YUV request would result in a steady frame rate
     * (let's say it's 30 FPS). If a single JPEG request is submitted periodically,
     * the frame rate will stay at 30 FPS (as long as we wait for the previous JPEG to return each
     * time). If we try to submit a repeating YUV + JPEG request, then the frame rate will drop from
     * 30 FPS.</p>
     *
     * <p>In general, submitting a new request with a non-0 stall time stream will <em>not</em> cause a
     * frame rate drop unless there are still outstanding buffers for that stream from previous
     * requests.</p>
     *
     * <p>Submitting a repeating request with streams (call this {@code S}) is the same as setting
     * the minimum frame duration from the normal minimum frame duration corresponding to {@code S},
     * added with the maximum stall duration for {@code S}.</p>
     *
     * <p>If interleaving requests with and without a stall duration, a request will stall by the
     * maximum of the remaining times for each can-stall stream with outstanding buffers.</p>
     *
     * <p>This means that a stalling request will not have an exposure start until the stall has
     * completed.</p>
     *
     * <p>This should correspond to the stall duration when only that stream is active, with all
     * processing (typically in {@code android.*.mode}) set to {@code FAST} or {@code OFF}.
     * Setting any of the processing modes to {@code HIGH_QUALITY} effectively results in an
     * indeterminate stall duration for all streams in a request (the regular stall calculation
     * rules are ignored).</p>
     *
     * <p>The following formats may always have a stall duration:
     * <ul>
     * <li>{@link ImageFormat#JPEG JPEG}
     * <li>{@link ImageFormat#RAW_SENSOR RAW16}
     * </ul>
     * </p>
     *
     * <p>The following formats will never have a stall duration:
     * <ul>
     * <li>{@link ImageFormat#YUV_420_888 YUV_420_888}
     * <li>{@link #isOutputSupportedFor(Class) Implementation-Defined}
     * </ul></p>
     *
     * <p>
     * All other formats may or may not have an allowed stall duration on a per-capability basis;
     * refer to {@link CameraCharacteristics#REQUEST_AVAILABLE_CAPABILITIES
     * android.request.availableCapabilities} for more details.</p>
     * </p>
     *
     * <p>See {@link CaptureRequest#SENSOR_FRAME_DURATION android.sensor.frameDuration}
     * for more information about calculating the max frame rate (absent stalls).</p>
     *
     * @param format an image format from {@link ImageFormat} or {@link PixelFormat}
     * @param size an output-compatible size
     * @return a stall duration {@code >=} 0 in nanoseconds
     *
     * @throws IllegalArgumentException if {@code format} or {@code size} was not supported
     * @throws NullPointerException if {@code size} was {@code null}
     *
     * @see CaptureRequest#SENSOR_FRAME_DURATION
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetOutputStallDuration(
        /* [in] */ Int32 format,
        /* [in] */ ISize* size,
        /* [out] */ Int64* value);

    /**
     * Get the stall duration for the class/size combination (in nanoseconds).
     *
     * <p>This assumes a the {@code klass} is set up to use an implementation-defined format.
     * For user-defined formats, use {@link #getOutputMinFrameDuration(int, Size)}.</p>
     *
     * <p>{@code klass} should be one of the ones with a non-empty array returned by
     * {@link #getOutputSizes(Class)}.</p>
     *
     * <p>{@code size} should be one of the ones returned by
     * {@link #getOutputSizes(Class)}.</p>
     *
     * <p>See {@link #getOutputStallDuration(int, Size)} for a definition of a
     * <em>stall duration</em>.</p>
     *
     * @param klass
     *          a class which is supported by {@link #isOutputSupportedFor(Class)} and has a
     *          non-empty array returned by {@link #getOutputSizes(Class)}
     * @param size an output-compatible size
     * @return a minimum frame duration {@code >=} 0 in nanoseconds
     *
     * @throws IllegalArgumentException if {@code klass} or {@code size} was not supported
     * @throws NullPointerException if {@code size} or {@code klass} was {@code null}
     *
     * @see CaptureRequest#SENSOR_FRAME_DURATION
     * @see ImageFormat
     * @see PixelFormat
     */
    CARAPI GetOutputStallDuration(
        /* [in] */ IClassInfo* klass,
        /* [in] */ ISize* size,
        /* [out] */ Int64* value);

    /**
     * Check if this {@link StreamConfigurationMap} is equal to another
     * {@link StreamConfigurationMap}.
     *
     * <p>Two vectors are only equal if and only if each of the respective elements is equal.</p>
     *
     * @return {@code true} if the objects were equal, {@code false} otherwise
     */
    // @Override
    CARAPI Equals(
        /* [in] */ IInterface* obj,
        /* [out] */ Boolean* value);

    /**
     * {@inheritDoc}
     */
    // @Override
    CARAPI GetHashCode(
        /* [out] */ Int32* value);

    /**
     * Ensures that the format is either user-defined or implementation defined.
     *
     * <p>If a format has a different internal representation than the public representation,
     * passing in the public representation here will fail.</p>
     *
     * <p>For example if trying to use {@link ImageFormat#JPEG}:
     * it has a different public representation than the internal representation
     * {@code HAL_PIXEL_FORMAT_BLOB}, this check will fail.</p>
     *
     * <p>Any invalid/undefined formats will raise an exception.</p>
     *
     * @param format image format
     * @return the format
     *
     * @throws IllegalArgumentException if the format was invalid
     */
    static CARAPI CheckArgumentFormatInternal(
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    /**
     * Ensures that the format is publicly user-defined in either ImageFormat or PixelFormat.
     *
     * <p>If a format has a different public representation than the internal representation,
     * passing in the internal representation here will fail.</p>
     *
     * <p>For example if trying to use {@code HAL_PIXEL_FORMAT_BLOB}:
     * it has a different internal representation than the public representation
     * {@link ImageFormat#JPEG}, this check will fail.</p>
     *
     * <p>Any invalid/undefined formats will raise an exception, including implementation-defined.
     * </p>
     *
     * <p>Note that {@code @hide} and deprecated formats will not pass this check.</p>
     *
     * @param format image format
     * @return the format
     *
     * @throws IllegalArgumentException if the format was not user-defined
     */
    static CARAPI CheckArgumentFormat(
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    /**
     * Convert a public-visible {@code ImageFormat} into an internal format
     * compatible with {@code graphics.h}.
     *
     * <p>In particular these formats are converted:
     * <ul>
     * <li>HAL_PIXEL_FORMAT_BLOB => ImageFormat.JPEG
     * </ul>
     * </p>
     *
     * <p>Passing in an implementation-defined format which has no public equivalent will fail;
     * as will passing in a public format which has a different internal format equivalent.
     * See {@link #checkArgumentFormat} for more details about a legal public format.</p>
     *
     * <p>All other formats are returned as-is, no further invalid check is performed.</p>
     *
     * <p>This function is the dual of {@link #imageFormatToInternal}.</p>
     *
     * @param format image format from {@link ImageFormat} or {@link PixelFormat}
     * @return the converted image formats
     *
     * @throws IllegalArgumentException
     *          if {@code format} is {@code HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED} or
     *          {@link ImageFormat#JPEG}
     *
     * @see ImageFormat
     * @see PixelFormat
     * @see #checkArgumentFormat
     */
    static CARAPI ImageFormatToPublic(
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    /**
     * Convert image formats from internal to public formats (in-place).
     *
     * @param formats an array of image formats
     * @return {@code formats}
     *
     * @see #imageFormatToPublic
     */
    static CARAPI ImageFormatToPublic(
        /* [in] */ ArrayOf<Int32>* formats);

    /**
     * Convert a public format compatible with {@code ImageFormat} to an internal format
     * from {@code graphics.h}.
     *
     * <p>In particular these formats are converted:
     * <ul>
     * <li>ImageFormat.JPEG => HAL_PIXEL_FORMAT_BLOB
     * </ul>
     * </p>
     *
     * <p>Passing in an implementation-defined format here will fail (it's not a public format);
     * as will passing in an internal format which has a different public format equivalent.
     * See {@link #checkArgumentFormat} for more details about a legal public format.</p>
     *
     * <p>All other formats are returned as-is, no invalid check is performed.</p>
     *
     * <p>This function is the dual of {@link #imageFormatToPublic}.</p>
     *
     * @param format public image format from {@link ImageFormat} or {@link PixelFormat}
     * @return the converted image formats
     *
     * @see ImageFormat
     * @see PixelFormat
     *
     * @throws IllegalArgumentException
     *              if {@code format} was {@code HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED}
     */
    static CARAPI ImageFormatToInternal(
        /* [in] */ Int32 format,
        /* [out] */ Int32* result);

    /**
     * Convert image formats from public to internal formats (in-place).
     *
     * @param formats an array of image formats
     * @return {@code formats}
     *
     * @see #imageFormatToInternal
     *
     * @hide
     */
    static CARAPI ImageFormatToInternal(
        /* [in] */ ArrayOf<Int32>* formats);

private:
    // Check that the argument is supported by #getOutputFormats or #getInputFormats
    CARAPI CheckArgumentFormatSupported(
        /* [in] */ Int32 format,
        /* [in] */ Boolean output,
        /* [out] */ Int32* result);

    CARAPI GetPublicFormatSizes(
        /* [in] */ Int32 format,
        /* [in] */ Boolean output,
        /* [out, callee] */ ArrayOf<ISize*>** sizes);

    CARAPI GetInternalFormatSizes(
        /* [in] */ Int32 format,
        /* [in] */ Boolean output,
        /* [out, callee] */ ArrayOf<ISize*>** sizes);

    /** Get the list of publically visible output formats; does not include IMPL_DEFINED */
    CARAPI GetPublicFormats(
        /* [in] */ Boolean output,
        /* [out, callee] */ ArrayOf<Int32>** formats);

    /** Get the format -> size count map for either output or input formats */
    HashMap<Int32, Int32>& GetFormatsMap(
        /* [in] */ Boolean output);

    CARAPI GetInternalFormatDuration(
        /* [in] */ Int32 format,
        /* [in] */ ISize* size,
        /* [in] */ Int32 duration,
        /* [out] */ Int64* result);

    /**
     * Get the durations array for the kind of duration
     *
     * @see #DURATION_MIN_FRAME
     * @see #DURATION_STALL
     * */
    CARAPI GetDurations(
        /* [in] */ Int32 duration,
        /* [out, callee] */ ArrayOf<IStreamConfigurationDuration*>** durations);

    /** Count the number of publicly-visible output formats */
    CARAPI GetPublicFormatCount(
        /* [in] */ Boolean output,
        /* [out] */ Int32* result);

    static CARAPI_(Boolean) ArrayContains(
        /* [in] */ ArrayOf<IInterface*>* array,
        /* [in] */ IInterface* element);

private:
    static const String TAG;

    // from system/core/include/system/graphics.h
    static const Int32 HAL_PIXEL_FORMAT_BLOB;
    static const Int32 HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
    static const Int32 HAL_PIXEL_FORMAT_RAW_OPAQUE;

    /**
     * @see #getDurations(int)
     * @see #getDurationDefault(int)
     */
    static const Int32 DURATION_MIN_FRAME;
    static const Int32 DURATION_STALL;

    AutoPtr<ArrayOf<IStreamConfiguration*> > mConfigurations;
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > mMinFrameDurations;
    AutoPtr<ArrayOf<IStreamConfigurationDuration*> > mStallDurations;
    AutoPtr<ArrayOf<IHighSpeedVideoConfiguration*> > mHighSpeedVideoConfigurations;

    /** ImageFormat -> num output sizes mapping */
    HashMap</*ImageFormat*/Int32, /*Count*/Int32> mOutputFormats;

    /** ImageFormat -> num input sizes mapping */
    HashMap</*ImageFormat*/Int32, /*Count*/Int32> mInputFormats;

    /** High speed video Size -> FPS range count mapping*/
    HashMap</*HighSpeedVideoSize*/AutoPtr<ISize>, /*Count*/Int32> mHighSpeedVideoSizeMap;

    /** High speed video FPS range -> Size count mapping*/
    HashMap</*HighSpeedVideoFpsRange*/AutoPtr<Range<IInteger32> >, /*Count*/Int32> mHighSpeedVideoFpsRangeMap;
    static List< AutoPtr<IClassInfo> > sOutputSupportedClasses;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_STREAMCONFIGURATIONMAP_H__
