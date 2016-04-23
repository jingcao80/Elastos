
#ifndef __ELASTOS_DROID_HARDWARE_HARDWARECAMERA_H__
#define __ELASTOS_DROID_HARDWARE_HARDWARECAMERA_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Hardware.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/core/AutoLock.h>
#include <elastos/droid/os/Handler.h>
#include <camera/Camera.h>
#include <system/camera.h>
#include <utils/Vector.h>


using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Droid::Graphics::ISurfaceTexture;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Privacy::IPrivacySettingsManager;
using Elastos::Droid::Content::Pm::IIPackageManager;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::Droid::View::ISurface;
using Elastos::Core::IInteger32;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::IHashMap;

namespace Elastos {
namespace Droid {
namespace Hardware {

class HardwareCamera
    : public Object
    , public IHardwareCamera
{
public:
    /**
     * Information about a camera
     *
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    //@Deprecated
    class CameraInfo
        : public Object
        , public IHardwareCameraInfo
    {
    public:
        CameraInfo();

        CAR_INTERFACE_DECL()

        CARAPI SetFacing(
            /* [in] */ Int32 facing);

        CARAPI GetFacing(
            /* [out] */ Int32* facing);

        CARAPI SetOrientation(
            /* [in] */ Int32 orientation);

        CARAPI GetOrientation(
            /* [out] */ Int32* orientation);

        CARAPI SetCanDisableShutterSound(
            /* [in] */ Boolean can);

        CARAPI GetCanDisableShutterSound(
            /* [out] */ Boolean* can);

    public:
        /**
         * The direction that the camera faces. It should be
         * CAMERA_FACING_BACK or CAMERA_FACING_FRONT.
         */
        Int32 mFacing;

        /**
         * <p>The orientation of the camera image. The value is the angle that the
         * camera image needs to be rotated clockwise so it shows correctly on
         * the display in its natural orientation. It should be 0, 90, 180, or 270.</p>
         *
         * <p>For example, suppose a device has a naturally tall screen. The
         * back-facing camera sensor is mounted in landscape. You are looking at
         * the screen. If the top side of the camera sensor is aligned with the
         * right edge of the screen in natural orientation, the value should be
         * 90. If the top side of a front-facing camera sensor is aligned with
         * the right of the screen, the value should be 270.</p>
         *
         * @see #setDisplayOrientation(Int32)
         * @see Parameters#setRotation(Int32)
         * @see Parameters#setPreviewSize(Int32, Int32)
         * @see Parameters#setPictureSize(Int32, Int32)
         * @see Parameters#setJpegThumbnailSize(Int32, Int32)
         */
        Int32 mOrientation;

        /**
         * <p>Whether the shutter sound can be disabled.</p>
         *
         * <p>On some devices, the camera shutter sound cannot be turned off
         * through {@link #enableShutterSound enableShutterSound}. This field
         * can be used to determine whether a call to disable the shutter sound
         * will succeed.</p>
         *
         * <p>If this field is set to TRUE, then a call of
         * {@code enableShutterSound(FALSE)} will be successful. If set to
         * FALSE, then that call will fail, and the shutter sound will be played
         * when {@link Camera#takePicture takePicture} is called.</p>
         */
        Boolean mCanDisableShutterSound;
    };


    /**
     * Information about a face identified through camera face detection.
     *
     * <p>When face detection is used with a camera, the {@link FaceDetectionListener} returns a
     * list of face objects for use in focusing and metering.</p>
     *
     * @see FaceDetectionListener
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    //@Deprecated
    class Face
        : public Object
        , public ICameraFace
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Create an empty face.
         */
        Face();

        CARAPI SetRect(
            /* [in] */ IRect* rect);

        CARAPI SetScore(
            /* [in] */ Int32 score);

        CARAPI SetId(
            /* [in] */ Int32 id);

        CARAPI SetLeftEye(
            /* [in] */ IPoint* leftEye);

        CARAPI SetRightEye(
            /* [in] */ IPoint* rightEye);

        CARAPI SetMouth(
            /* [in] */ IPoint* mouth);

        CARAPI GetRect(
            /* [out] */ IRect** rect);

        CARAPI GetScore(
            /* [out] */ Int32* score);

        CARAPI GetId(
            /* [out] */ Int32* id);

        CARAPI GetLeftEye(
            /* [out] */ IPoint** leftEye);

        CARAPI GetRightEye(
            /* [out] */ IPoint** rightEye);

        CARAPI GetMouth(
            /* [out] */ IPoint** mouth);

    public:
        /**
         * Bounds of the face. (-1000, -1000) represents the top-left of the
         * camera field of view, and (1000, 1000) represents the bottom-right of
         * the field of view. For example, suppose the size of the viewfinder UI
         * is 800x480. The rect passed from the driver is (-1000, -1000, 0, 0).
         * The corresponding viewfinder rect should be (0, 0, 400, 240). It is
         * guaranteed left < right and top < bottom. The coordinates can be
         * smaller than -1000 or bigger than 1000. But at least one vertex will
         * be within (-1000, -1000) and (1000, 1000).
         *
         * <p>The direction is relative to the sensor orientation, that is, what
         * the sensor sees. The direction is not affected by the rotation or
         * mirroring of {@link #setDisplayOrientation(Int32)}. The face bounding
         * rectangle does not provide any information about face orientation.</p>
         *
         * <p>Here is the matrix to convert driver coordinates to View coordinates
         * in pixels.</p>
         * <pre>
         * Matrix matrix = new Matrix();
         * CameraInfo info = CameraHolder.instance().getCameraInfo()[cameraId];
         * // Need mirror for front camera.
         * Boolean mirror = (info.facing == CameraInfo.CAMERA_FACING_FRONT);
         * matrix.setScale(mirror ? -1 : 1, 1);
         * // This is the value for android.hardware.Camera.setDisplayOrientation.
         * matrix.postRotate(displayOrientation);
         * // Camera driver coordinates range from (-1000, -1000) to (1000, 1000).
         * // UI coordinates range from (0, 0) to (width, height).
         * matrix.postScale(view.getWidth() / 2000f, view.getHeight() / 2000f);
         * matrix.postTranslate(view.getWidth() / 2f, view.getHeight() / 2f);
         * </pre>
         *
         * @see #startFaceDetection()
         */
        AutoPtr<IRect> mRect;

        /**
         * <p>The confidence level for the detection of the face. The range is 1 to
         * 100. 100 is the highest confidence.</p>
         *
         * <p>Depending on the device, even very low-confidence faces may be
         * listed, so applications should filter out faces with low confidence,
         * depending on the use case. For a typical point-and-shoot camera
         * application that wishes to display rectangles around detected faces,
         * filtering out faces with confidence less than 50 is recommended.</p>
         *
         * @see #startFaceDetection()
         */
        Int32 mScore;

        /**
         * An unique id per face while the face is visible to the tracker. If
         * the face leaves the field-of-view and comes back, it will get a new
         * id. This is an optional field, may not be supported on all devices.
         * If not supported, id will always be set to -1. The optional fields
         * are supported as a set. Either they are all valid, or none of them
         * are.
         */
        Int32 mId;

        /**
         * The coordinates of the center of the left eye. The coordinates are in
         * the same space as the ones for {@link #rect}. This is an optional
         * field, may not be supported on all devices. If not supported, the
         * value will always be set to NULL. The optional fields are supported
         * as a set. Either they are all valid, or none of them are.
         */
        AutoPtr<IPoint> mLeftEye;

        /**
         * The coordinates of the center of the right eye. The coordinates are
         * in the same space as the ones for {@link #rect}.This is an optional
         * field, may not be supported on all devices. If not supported, the
         * value will always be set to NULL. The optional fields are supported
         * as a set. Either they are all valid, or none of them are.
         */
        AutoPtr<IPoint> mRightEye;

        /**
         * The coordinates of the center of the mouth.  The coordinates are in
         * the same space as the ones for {@link #rect}. This is an optional
         * field, may not be supported on all devices. If not supported, the
         * value will always be set to NULL. The optional fields are supported
         * as a set. Either they are all valid, or none of them are.
         */
        AutoPtr<IPoint> mMouth;
    };

    /**
     * Image size (width and height dimensions).
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    //@Deprecated
    class Size
        : public Object
        , public ICameraSize
    {
    public:
        CAR_INTERFACE_DECL()

        Size();
        /**
         * Sets the dimensions for pictures.
         *
         * @param w the photo width (pixels)
         * @param h the photo height (pixels)
         */
        CARAPI constructor(
            /* [in] */ Int32 w,
            /* [in] */ Int32 h);

        /**
         * Compares {@code obj} to this size.
         *
         * @param obj the object to compare this size with.
         * @return {@code TRUE} if the width and height of {@code obj} is the
         *         same as those of this size. {@code FALSE} otherwise.
         */
        //@Override
        CARAPI Equals(
            /* [in] */ ICameraSize* obj,
            /* [out] */ Boolean* e);

        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* e);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

        CARAPI GetWidth(
            /* [out] */ Int32* width);

        CARAPI GetHeight(
            /* [out] */ Int32* height);

    public:
        /** width of the picture */
        Int32 mWidth;
        /** height of the picture */
        Int32 mHeight;
    };

    /**
     * <p>The Area class is used for choosing specific metering and focus areas for
     * the camera to use when calculating auto-exposure, auto-white balance, and
     * auto-focus.</p>
     *
     * <p>To find out how many simultaneous areas a given camera supports, use
     * {@link Parameters#getMaxNumMeteringAreas()} and
     * {@link Parameters#getMaxNumFocusAreas()}. If metering or focusing area
     * selection is unsupported, these methods will return 0.</p>
     *
     * <p>Each Area consists of a rectangle specifying its bounds, and a weight
     * that determines its importance. The bounds are relative to the camera's
     * current field of view. The coordinates are mapped so that (-1000, -1000)
     * is always the top-left corner of the current field of view, and (1000,
     * 1000) is always the bottom-right corner of the current field of
     * view. Setting Areas with bounds outside that range is not allowed. Areas
     * with zero or negative width or height are not allowed.</p>
     *
     * <p>The weight must range from 1 to 1000, and represents a weight for
     * every pixel in the area. This means that a large metering area with
     * the same weight as a smaller area will have more effect in the
     * metering result.  Metering areas can overlap and the driver
     * will add the weights in the overlap region.</p>
     *
     * @see Parameters#setFocusAreas(List)
     * @see Parameters#getFocusAreas()
     * @see Parameters#getMaxNumFocusAreas()
     * @see Parameters#setMeteringAreas(List)
     * @see Parameters#getMeteringAreas()
     * @see Parameters#getMaxNumMeteringAreas()
     *
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    //@Deprecated
    class Area
        : public Object
        , public ICameraArea
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * Create an area with specified rectangle and weight.
         *
         * @param rect the bounds of the area.
         * @param weight the weight of the area.
         */
        Area(
            /* [in] */ IRect* rect,
            /* [in] */ Int32 weight);

        /**
         * Compares {@code obj} to this area.
         *
         * @param obj the object to compare this area with.
         * @return {@code TRUE} if the rectangle and weight of {@code obj} is
         *         the same as those of this area. {@code FALSE} otherwise.
         */
        CARAPI Equals(
            /* [in] */ IInterface* obj,
            /* [out] */ Boolean* e);

        CARAPI GetHashCode(
            /* [out] */ Int32* hash);

        CARAPI GetRect(
            /* [out] */ IRect** rect);

        CARAPI GetWeight(
            /* [out] */ Int32* weight);
    public:
        /**
         * Bounds of the area. (-1000, -1000) represents the top-left of the
         * camera field of view, and (1000, 1000) represents the bottom-right of
         * the field of view. Setting bounds outside that range is not
         * allowed. Bounds with zero or negative width or height are not
         * allowed.
         *
         * @see Parameters#getFocusAreas()
         * @see Parameters#getMeteringAreas()
         */
        AutoPtr<IRect> mRect;

        /**
         * Weight of the area. The weight must range from 1 to 1000, and
         * represents a weight for every pixel in the area. This means that a
         * large metering area with the same weight as a smaller area will have
         * more effect in the metering result.  Metering areas can overlap and
         * the driver will add the weights in the overlap region.
         *
         * @see Parameters#getFocusAreas()
         * @see Parameters#getMeteringAreas()
         */
        Int32 mWeight;
    };

    /**
     * Camera service settings.
     *
     * <p>To make camera parameters take effect, applications have to call
     * {@link Camera#setParameters(Camera.Parameters)}. For example, after
     * {@link Camera.Parameters#setWhiteBalance} is called, white balance is not
     * actually changed until {@link Camera#setParameters(Camera.Parameters)}
     * is called with the changed parameters object.
     *
     * <p>Different devices may have different camera capabilities, such as
     * picture size or flash modes. The application should query the camera
     * capabilities before setting parameters. For example, the application
     * should call {@link Camera.Parameters#getSupportedColorEffects()} before
     * calling {@link Camera.Parameters#setColorEffect(String)}. If the
     * camera does not support color effects,
     * {@link Camera.Parameters#getSupportedColorEffects()} will return null.
     *
     * @deprecated We recommend using the new {@link android.hardware.camera2} API for new
     *             applications.
     */
    //@Deprecated
    class Parameters
        : public Object
        , public IParameters
    {
    public:
        CAR_INTERFACE_DECL()

        Parameters(
            /* [in] */ IHardwareCamera* camera);

        /**
         * Overwrite existing parameters with a copy of the ones from {@code other}.
         *
         * <b>For use by the legacy shim only.</b>
         *
         * @hide
         */
        CARAPI CopyFrom(
            /* [in] */ IParameters* other);

        /**
         * Value equality check.
         *
         * @hide
         */
        CARAPI Same(
            /* [in] */ IParameters* other,
            /* [out] */ Boolean* result);

        /**
         * Writes the current Parameters to the log.
         * @hide
         * @deprecated
         */
        //@Deprecated
        // public void dump() {
        //     Log.e(TAG, "dump: size=" + mMap.size());
        //     for (String k : mMap.keySet()) {
        //         Log.e(TAG, "dump: " + k + "=" + mMap.get(k));
        //     }
        // }

        CARAPI Flatten(
            /* [out] */ String* content);

        CARAPI Unflatten(
            /* [in] */ const String& flattened);

        CARAPI Remove(
            /* [in] */ const String& key);

        CARAPI Set(
            /* [in] */ const String& key,
            /* [in] */ const String& value);

        CARAPI Set(
            /* [in] */ const String& key,
            /* [in] */ Int32 value);

        CARAPI Get(
            /* [in] */ const String& key,
            /* [out] */ String* value);

        CARAPI GetInt(
            /* [in] */ const String& key,
            /* [out] */ Int32* value);

        CARAPI SetPreviewSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI GetPreviewSize(
            /* [out] */ ICameraSize** size);

        CARAPI GetSupportedPreviewSizes(
            /* [out, callee] */ ArrayOf<ICameraSize*>** sizes);

        CARAPI GetSupportedVideoSizes(
            /* [out, callee] */ ArrayOf<ICameraSize*>** sizes);

        CARAPI GetPreferredPreviewSizeForVideo(
            /* [out] */ ICameraSize** size);

        CARAPI SetJpegThumbnailSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI GetJpegThumbnailSize(
            /* [out] */ ICameraSize** size);

        CARAPI GetSupportedJpegThumbnailSizes(
            /* [out, callee] */ ArrayOf<ICameraSize*>** sizes);

        CARAPI SetJpegThumbnailQuality(
            /* [in] */ Int32 quality);

        CARAPI GetJpegThumbnailQuality(
            /* [out] */ Int32* quality);

        CARAPI SetJpegQuality(
            /* [in] */ Int32 quality);

        CARAPI GetJpegQuality(
            /* [out] */ Int32* quality);

        CARAPI SetPreviewFrameRate(
            /* [in] */ Int32 fps);

        CARAPI GetPreviewFrameRate(
            /* [out] */ Int32* rate);

        CARAPI GetSupportedPreviewFrameRates(
            /* [out, callee] */ ArrayOf<IInteger32*>** rates);

        CARAPI SetPreviewFpsRange(
            /* [in] */ Int32 min,
            /* [in] */ Int32 max);

        CARAPI GetPreviewFpsRange(
            /* [out] */ ArrayOf<Int32>* range);

        CARAPI GetSupportedPreviewFpsRange(
            /* [out] */ IList** lists);

        CARAPI SetPreviewFormat(
            /* [in] */ Int32 pixel_format);

        CARAPI GetPreviewFormat(
            /* [out] */ Int32* format);

        CARAPI GetSupportedPreviewFormats(
            /* [out, callee] */ ArrayOf<IInteger32*>** formats);

        CARAPI SetPictureSize(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI GetPictureSize(
            /* [out] */ ICameraSize** size);

        CARAPI GetSupportedPictureSizes(
            /* [out, callee] */ ArrayOf<ICameraSize*>** sizes);

        CARAPI SetPictureFormat(
            /* [in] */ Int32 pixel_format);

        CARAPI GetPictureFormat(
            /* [out] */ Int32* format);

        CARAPI GetSupportedPictureFormats(
            /* [out, callee] */ ArrayOf<IInteger32*>** formats);

        CARAPI SetRotation(
            /* [in] */ Int32 rotation);

        CARAPI SetGpsLatitude(
            /* [in] */ Double latitude);

        CARAPI SetGpsLongitude(
            /* [in] */ Double longitude);

        CARAPI SetGpsAltitude(
            /* [in] */ Double altitude);

        CARAPI SetGpsTimestamp(
            /* [in] */ Int64 timestamp);

        CARAPI SetGpsProcessingMethod(
            /* [in] */ const String& processing_method);

        CARAPI RemoveGpsData();

        CARAPI GetWhiteBalance(
            /* [out] */ String* balance);

        CARAPI SetWhiteBalance(
            /* [in] */ const String& value);

        CARAPI GetSupportedWhiteBalance(
            /* [out, callee] */ ArrayOf<String>** balance);

        CARAPI GetColorEffect(
            /* [out] */ String* effect);

        CARAPI SetColorEffect(
            /* [in] */ const String& value);

        CARAPI GetSupportedColorEffects(
            /* [out, callee] */ ArrayOf<String>** effects);

        CARAPI GetAntibanding(
            /* [out] */ String* banding);

        CARAPI SetAntibanding(
            /* [in] */ const String& antibanding);

        CARAPI GetSupportedAntibanding(
            /* [out, callee] */ ArrayOf<String>** values);

        CARAPI GetSceneMode(
            /* [out] */ String* mode);

        CARAPI SetSceneMode(
            /* [in] */ const String& value);

        CARAPI GetSupportedSceneModes(
            /* [out, callee] */ ArrayOf<String>** modes);

        CARAPI GetFlashMode(
            /* [out] */ String* mode);

        CARAPI SetFlashMode(
            /* [in] */ const String& value);

        CARAPI GetSupportedFlashModes(
            /* [out, callee] */ ArrayOf<String>** modes);

        CARAPI GetFocusMode(
            /* [out] */ String* mode);

        CARAPI SetFocusMode(
            /* [in] */ const String& value);

        CARAPI GetSupportedFocusModes(
            /* [out, callee] */ ArrayOf<String>** modes);

        CARAPI GetFocalLength(
            /* [out] */ Float* len);

        CARAPI GetHorizontalViewAngle(
            /* [out] */ Float* angle);

        CARAPI GetVerticalViewAngle(
            /* [out] */ Float* angle);

        CARAPI GetExposureCompensation(
            /* [out] */ Int32* value);

        CARAPI SetExposureCompensation(
            /* [out] */ Int32 value);

        CARAPI GetMaxExposureCompensation(
            /* [out] */ Int32* compensation);

        CARAPI GetMinExposureCompensation(
            /* [out] */ Int32* compensation);

        CARAPI GetExposureCompensationStep(
            /* [out] */ Float* step);

        CARAPI SetAutoExposureLock(
            /* [in] */ Boolean toggle);

        CARAPI GetAutoExposureLock(
            /* [out] */ Boolean* state);

        CARAPI IsAutoExposureLockSupported(
            /* [out] */ Boolean* state);

        CARAPI SetAutoWhiteBalanceLock(
            /* [in] */ Boolean toggle);

        CARAPI GetAutoWhiteBalanceLock(
            /* [out] */ Boolean* lock);

        CARAPI IsAutoWhiteBalanceLockSupported(
            /* [out] */ Boolean* supported);

        CARAPI GetZoom(
            /* [out] */ Int32* zoom);

        CARAPI SetZoom(
            /* [in] */ Int32 value);

        CARAPI IsZoomSupported(
            /* [out] */ Boolean* supported);

        CARAPI GetMaxZoom(
            /* [out] */ Int32* zoom);

        CARAPI GetZoomRatios(
            /* [out, callee] */ ArrayOf<IInteger32*>** ratios);

        CARAPI IsSmoothZoomSupported(
            /* [out] */ Boolean* supported);

        CARAPI GetFocusDistances(
            /* [in] */ ArrayOf<Float>* output);

        CARAPI GetMaxNumFocusAreas(
            /* [out] */ Int32* value);

        CARAPI GetFocusAreas(
            /* [out, callee] */ ArrayOf<ICameraArea*>** areas);

        CARAPI SetFocusAreas(
            /* [in] */ ArrayOf<ICameraArea*>* focusAreas);

        CARAPI GetMaxNumMeteringAreas(
            /* [out] */ Int32* value);

        CARAPI GetMeteringAreas(
            /* [out, callee] */ ArrayOf<ICameraArea*>** areas);

        CARAPI SetMeteringAreas(
            /* [in] */ ArrayOf<ICameraArea*>* meteringAreas);

        CARAPI GetMaxNumDetectedFaces(
            /* [out] */ Int32* faces);

        CARAPI SetRecordingHint(
            /* [in] */ Boolean hint);

        CARAPI IsVideoSnapshotSupported(
            /* [out] */ Boolean* supported);

        CARAPI SetVideoStabilization(
            /* [in] */ Boolean toggle);

        CARAPI GetVideoStabilization(
            /* [out] */ Boolean* value);

        CARAPI IsVideoStabilizationSupported(
            /* [out] */ Boolean* supported);

    private:
        friend class HardwareCamera;

        CARAPI_(AutoPtr<IHardwareCamera>) GetOuter();

        void Set(
            /* [in] */ const String& key,
            /* [in] */ ArrayOf<ICameraArea*>* areas);

        void Put(
            /* [in] */ const String& key,
            /* [in] */ const String& value);

        CARAPI CameraFormatForPixelFormat(
            /* [in] */ Int32 pixel_format,
            /* [out] */ String* format);

        Int32 PixelFormatForCameraFormat(
            /* [in] */ const String& format);

        // Splits a comma delimited string to an ArrayList of String.
        // Return NULL if the passing string is NULL or the size is 0.
        AutoPtr<ArrayOf<String> > Split(
            /* [in] */ const String& str);

        // Splits a comma delimited string to an ArrayList of Integer.
        // Return NULL if the passing string is NULL or the size is 0.
        AutoPtr<ArrayOf<IInteger32*> > SplitInt(
            /* [in] */ const String& str);

        void SplitInt(
            /* [in] */ const String& str,
            /* [in] */ ArrayOf<Int32>* output);

        // Splits a comma delimited string to an ArrayList of Float.
        void SplitFloat(
            /* [in] */ const String& str,
            /* [in] */ ArrayOf<Float>* output);

        // Returns the value of a Float parameter.
        Float GetFloat(
            /* [in] */ const String& key,
            /* [in] */ Float defaultValue);

        // Returns the value of a integer parameter.
        Int32 GetInt(
            /* [in] */ const String& key,
            /* [in] */ Int32 defaultValue);

        // Splits a comma delimited string to an ArrayList of Size.
        // Return NULL if the passing string is NULL or the size is 0.
        AutoPtr<ArrayOf<ICameraSize*> > SplitSize(
            /* [in] */ const String& str);

        // Parses a string (ex: "480x320") to Size object.
        // Return NULL if the passing string is NULL.
        AutoPtr<ICameraSize> StrToSize(
            /* [in] */ const String& str);

        // Splits a comma delimited string to an ArrayList of Int32 array.
        // Example string: "(10000,26623),(10000,30000)". Return NULL if the
        // passing string is NULL or the size is 0.
        AutoPtr<IList> SplitRange(
            /* [in] */ const String& str);

        // Splits a comma delimited string to an ArrayList of Area objects.
        // Example string: "(-10,-10,0,0,300),(0,0,10,10,700)". Return NULL if
        // the passing string is NULL or the size is 0 or (0,0,0,0,0).
        AutoPtr<ArrayOf<ICameraArea*> > SplitArea(
            /* [in] */ const String& str);

        Boolean Same(
            /* [in] */ const String& s1,
            /* [in] */ const String& s2);

    private:
        // Parameter keys to communicate with the camera driver.
        static String KEY_PREVIEW_SIZE;
        static String KEY_PREVIEW_FORMAT;
        static String KEY_PREVIEW_FRAME_RATE;
        static String KEY_PREVIEW_FPS_RANGE;
        static String KEY_PICTURE_SIZE;
        static String KEY_PICTURE_FORMAT;
        static String KEY_JPEG_THUMBNAIL_SIZE;
        static String KEY_JPEG_THUMBNAIL_WIDTH;
        static String KEY_JPEG_THUMBNAIL_HEIGHT;
        static String KEY_JPEG_THUMBNAIL_QUALITY;
        static String KEY_JPEG_QUALITY;
        static String KEY_ROTATION;
        static String KEY_GPS_LATITUDE;
        static String KEY_GPS_LONGITUDE;
        static String KEY_GPS_ALTITUDE;
        static String KEY_GPS_TIMESTAMP;
        static String KEY_GPS_PROCESSING_METHOD;
        static String KEY_WHITE_BALANCE;
        static String KEY_EFFECT;
        static String KEY_ANTIBANDING;
        static String KEY_SCENE_MODE;
        static String KEY_FLASH_MODE;
        static String KEY_FOCUS_MODE;
        static String KEY_FOCUS_AREAS;
        static String KEY_MAX_NUM_FOCUS_AREAS;
        static String KEY_FOCAL_LENGTH;
        static String KEY_HORIZONTAL_VIEW_ANGLE;
        static String KEY_VERTICAL_VIEW_ANGLE;
        static String KEY_EXPOSURE_COMPENSATION;
        static String KEY_MAX_EXPOSURE_COMPENSATION;
        static String KEY_MIN_EXPOSURE_COMPENSATION;
        static String KEY_EXPOSURE_COMPENSATION_STEP;
        static String KEY_AUTO_EXPOSURE_LOCK;
        static String KEY_AUTO_EXPOSURE_LOCK_SUPPORTED;
        static String KEY_AUTO_WHITEBALANCE_LOCK;
        static String KEY_AUTO_WHITEBALANCE_LOCK_SUPPORTED;
        static String KEY_METERING_AREAS;
        static String KEY_MAX_NUM_METERING_AREAS;
        static String KEY_ZOOM;
        static String KEY_MAX_ZOOM;
        static String KEY_ZOOM_RATIOS;
        static String KEY_ZOOM_SUPPORTED;
        static String KEY_SMOOTH_ZOOM_SUPPORTED;
        static String KEY_FOCUS_DISTANCES;
        static String KEY_VIDEO_SIZE;
        static String KEY_PREFERRED_PREVIEW_SIZE_FOR_VIDEO;
        static String KEY_MAX_NUM_DETECTED_FACES_HW;
        static String KEY_MAX_NUM_DETECTED_FACES_SW;
        static String KEY_RECORDING_HINT;
        static String KEY_VIDEO_SNAPSHOT_SUPPORTED;
        static String KEY_VIDEO_STABILIZATION;
        static String KEY_VIDEO_STABILIZATION_SUPPORTED;

        // Parameter key suffix for supported values.
        static String SUPPORTED_VALUES_SUFFIX;

        static String TRUESTR;
        static String FALSESTR;

        // Formats for setPreviewFormat and setPictureFormat.
        static String PIXEL_FORMAT_YUV422SP;
        static String PIXEL_FORMAT_YUV420SP;
        static String PIXEL_FORMAT_YUV422I;
        static String PIXEL_FORMAT_YUV420P;
        static String PIXEL_FORMAT_RGB565;
        static String PIXEL_FORMAT_JPEG;
        static String PIXEL_FORMAT_BAYER_RGGB;

        /**
         * Order matters: Keys that are {@link #set(String, String) set} later
         * will take precedence over keys that are set earlier (if the two keys
         * conflict with each other).
         *
         * <p>One example is {@link #setPreviewFpsRange(int, int)} , since it
         * conflicts with {@link #setPreviewFrameRate(int)} whichever key is set later
         * is the one that will take precedence.
         * </p>
         */
        //HashMap<String, String> mMap;
        AutoPtr<IHashMap> mMap;

        AutoPtr<IHardwareCamera> mOuter;
    };

    class EventHandler
        : public Handler
    {
    public:
        TO_STRING_IMPL("HardwareCamera::EventHandler")

        EventHandler(
            /* [in] */ HardwareCamera* c);

        // @Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* message);

    private:
        HardwareCamera* mCamera;
    };

    // provides persistent context for calls from native code to Java
    class JNICameraContext: public android::CameraListener
    {
    public:
        JNICameraContext(
            /* [in] */ HardwareCamera* weak_this,
            /* [in] */ HardwareCamera* clazz,
            /* [in] */ const android::sp<android::Camera>& camera);

        ~JNICameraContext();

        virtual void notify(
            /* [in] */ int32_t msgType,
            /* [in] */ int32_t ext1,
            /* [in] */ int32_t ext2);

        virtual void postData(
            /* [in] */ int32_t msgType,
            /* [in] */ const android::sp<android::IMemory>& dataPtr,
            /* [in] */ camera_frame_metadata_t *metadata);

        virtual void postDataTimestamp(
            /* [in] */ nsecs_t timestamp,
            /* [in] */ int32_t msgType,
            /* [in] */ const android::sp<android::IMemory>& dataPtr);

        void postMetadata(
            /* [in] */ int32_t msgType,
            /* [in] */ camera_frame_metadata_t *metadata);

        void addCallbackBuffer(
            /* [in] */ ArrayOf<Byte>* cbb,
            /* [in] */ int msgType);

        void setCallbackMode(
            /* [in] */ bool installed,
            /* [in] */ bool manualMode);

        android::sp<android::Camera> getCamera();

        bool isRawImageCallbackBufferAvailable() const;

        void release();

    private:
        void copyAndPost(
            /* [in] */ const android::sp<android::IMemory>& dataPtr,
            /* [in] */ int msgType);

        void clearCallbackBuffers_l(
            /* [in] */ android::Vector<ArrayOf<Byte>*>* buffers);

        void clearCallbackBuffers_l();

        CARAPI_(AutoPtr<ArrayOf<Byte> >) getCallbackBuffer(
            /* [in] */ android::Vector<ArrayOf<Byte>*> *buffers,
            /* [in] */ size_t bufferSize);

        HardwareCamera*     mCameraJObjectWeak;     // weak reference to java object
        // HardwareCamera*      mCameraJClass;          // strong reference to java class
        android::sp<android::Camera>  mCamera;                // strong reference to native object
        // /*jclass*/AutoPtr<Face>      mFaceClass;  // strong reference to Face class
        // jclass      mRectClass;  // strong reference to Rect class
        Object       mLock;

        /*
         * Global reference application-managed raw image buffer queue.
         *
         * Manual-only mode is supported for raw image callbacks, which is
         * set whenever method addCallbackBuffer() with msgType =
         * CAMERA_MSG_RAW_IMAGE is called; otherwise, null is returned
         * with raw image callbacks.
         */
        android::Vector<ArrayOf<Byte>*> mRawImageCallbackBuffers;

        /*
         * Application-managed preview buffer queue and the flags
         * associated with the usage of the preview buffer callback.
         */
        android::Vector<ArrayOf<Byte>*> mCallbackBuffers; // Global reference application managed byte[]
        bool mManualBufferMode;              // Whether to use application managed buffers.
        bool mManualCameraCallbackSet;       // Whether the callback has been set, used to
                                             // reduce unnecessary calls to set the callback.
    };

public:
    CAR_INTERFACE_DECL()

    HardwareCamera();

    virtual ~HardwareCamera();

    CARAPI ReleaseResources();

    /**
     * An empty Camera for testing purpose.
     */
    CARAPI constructor();

    /** used by Camera#open, Camera#open(int) */
    CARAPI constructor(
        /* [in] */ Int32 cameraId);

    /**
     * Create a legacy camera object.
     *
     * @param cameraId The hardware camera to access, between 0 and
     * {@link #getNumberOfCameras()}-1.
     * @param halVersion The HAL API version this camera device to be opened as.
     */
    CARAPI constructor(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 halVersion);

    /**
     * Creates a new Camera object to access a particular hardware camera with
     * given hal API version. If the same camera is opened by other applications
     * or the hal API version is not supported by this device, this will throw a
     * RuntimeException.
     * <p>
     * You must call {@link #release()} when you are done using the camera,
     * otherwise it will remain locked and be unavailable to other applications.
     * <p>
     * Your application should only have one Camera object active at a time for
     * a particular hardware camera.
     * <p>
     * Callbacks from other methods are delivered to the event loop of the
     * thread which called open(). If this thread has no event loop, then
     * callbacks are delivered to the main application event loop. If there is
     * no main application event loop, callbacks are not delivered.
     * <p class="caution">
     * <b>Caution:</b> On some devices, this method may take a long time to
     * complete. It is best to call this method from a worker thread (possibly
     * using {@link android.os.AsyncTask}) to avoid blocking the main
     * application UI thread.
     *
     * @param cameraId The hardware camera to access, between 0 and
     * {@link #getNumberOfCameras()}-1.
     * @param halVersion The HAL API version this camera device to be opened as.
     * @return a new Camera object, connected, locked and ready for use.
     *
     * @throws IllegalArgumentException if the {@code halVersion} is invalid
     *
     * @throws RuntimeException if opening the camera fails (for example, if the
     * camera is in use by another process or device policy manager has disabled
     * the camera).
     *
     * @see android.app.admin.DevicePolicyManager#getCameraDisabled(android.content.ComponentName)
     * @see #CAMERA_HAL_API_VERSION_1_0
     *
     * @hide
     */
    static CARAPI OpenLegacy(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 halVersion,
        /* [out] */ IHardwareCamera** camera);

    /**
     * @hide
     */
    static CARAPI_(Boolean) CheckInitErrors(
        /* [in] */ Int32 err);

    /**
     * @hide
     */
    static CARAPI_(AutoPtr<IHardwareCamera>) OpenUninitialized();

    /**
     * Returns the number of physical cameras available on this device.
     */
    /*public native*/
    static CARAPI GetNumberOfCameras(
        /* [out] */ Int32* num);

    /**
     * Returns the information about a particular camera.
     * If {@link #getNumberOfCameras()} returns N, the valid id is 0 to N-1.
     */
    static CARAPI GetCameraInfo(
        /* [in] */ Int32 cameraId,
        /* [in] */ IHardwareCameraInfo* cameraInfo);

    /**
     * Creates a new Camera object to access a particular hardware camera. If
     * the same camera is opened by other applications, this will throw a
     * RuntimeException.
     *
     * <p>You must call {@link #release()} when you are done using the camera,
     * otherwise it will remain locked and be unavailable to other applications.
     *
     * <p>Your application should only have one Camera object active at a time
     * for a particular hardware camera.
     *
     * <p>Callbacks from other methods are delivered to the event loop of the
     * thread which called open().  If this thread has no event loop, then
     * callbacks are delivered to the main application event loop.  If there
     * is no main application event loop, callbacks are not delivered.
     *
     * <p class="caution"><b>Caution:</b> On some devices, this method may
     * take a long time to complete.  It is best to call this method from a
     * worker thread (possibly using {@link android.os.AsyncTask}) to avoid
     * blocking the main application UI thread.
     *
     * @param cameraId the hardware camera to access, between 0 and
     *     {@link #getNumberOfCameras()}-1.
     * @return a new Camera object, connected, locked and ready for use.
     * @throws RuntimeException if opening the camera fails (for example, if the
     *     camera is in use by another process or device policy manager has
     *     disabled the camera).
     * @see android.app.admin.DevicePolicyManager#getCameraDisabled(android.content.ComponentName)
     */
    static CARAPI Open(
        /* [in] */ Int32 cameraId,
        /* [out] */ IHardwareCamera** camera);

    /**
     * Creates a new Camera object to access the first back-facing camera on the
     * device. If the device does not have a back-facing camera, this returns
     * null.
     * @see #open(int)
     */
    static CARAPI Open(
        /* [out] */ IHardwareCamera** camera);

    /**
     * Connect to the camera service using #connectLegacy
     *
     * <p>
     * This acts the same as normal except that it will return
     * the detailed error code if open fails instead of
     * converting everything into {@code NO_INIT}.</p>
     *
     * <p>Intended to use by the camera2 shim only, do <i>not</i> use this for other code.</p>
     *
     * @return a detailed errno error code, or {@code NO_ERROR} on success
     *
     * @hide
     */
    CARAPI CameraInitUnspecified(
        /* [in] */ Int32 cameraId,
        /* [out] */  Int32* result);

    /**
     * Sets the {@link Surface} to be used for live preview.
     * Either a surface or surface texture is necessary for preview, and
     * preview is necessary to take pictures.  The same surface can be re-set
     * without harm.  Setting a preview surface will un-set any preview surface
     * texture that was set via {@link #setPreviewTexture}.
     *
     * <p>The {@link SurfaceHolder} must already contain a surface when this
     * method is called.  If you are using {@link android.view.SurfaceView},
     * you will need to register a {@link SurfaceHolder.Callback} with
     * {@link SurfaceHolder#addCallback(SurfaceHolder.Callback)} and wait for
     * {@link SurfaceHolder.Callback#surfaceCreated(SurfaceHolder)} before
     * calling setPreviewDisplay() or starting preview.
     *
     * <p>This method must be called before {@link #startPreview()}.  The
     * one exception is that if the preview surface is not set (or set to NULL)
     * before startPreview() is called, then this method may be called once
     * with a non-NULL parameter to set the preview surface.  (This allows
     * camera setup and surface creation to happen in parallel, saving time.)
     * The preview surface may not otherwise change while preview is running.
     *
     * @param holder containing the Surface on which to place the preview,
     *     or NULL to remove the preview surface
     * @throws IOException if the method fails (for example, if the surface
     *     is unavailable or unsuitable).
     */
    CARAPI SetPreviewDisplay(
        /* [in] */ ISurfaceHolder* holder);

    /**
     * @hide
     */
    CARAPI SetPreviewSurface(
        /* [in] */ ISurface* surface);

    /**
     * Stops capturing and drawing preview frames to the surface, and
     * resets the camera for a future call to {@link #startPreview()}.
     */
    CARAPI StopPreview();

    /**
     * <p>Installs a callback to be invoked for every preview frame in addition
     * to displaying them on the screen.  The callback will be repeatedly called
     * for as Int64 as preview is active.  This method can be called at any time,
     * even while preview is live.  Any other preview callbacks are
     * overridden.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of each preview frame,
     *     or NULL to stop receiving callbacks.
     * @see android.media.MediaActionSound
     */
    CARAPI SetPreviewCallback(
        /* [in] */ IPreviewCallback* cb);

    /**
     * <p>Installs a callback to be invoked for the next preview frame in
     * addition to displaying it on the screen.  After one invocation, the
     * callback is cleared. This method can be called any time, even when
     * preview is live.  Any other preview callbacks are overridden.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of the next preview frame,
     *     or NULL to stop receiving callbacks.
     * @see android.media.MediaActionSound
     */
    CARAPI SetOneShotPreviewCallback(
        /* [in] */ IPreviewCallback* cb);

    /**
     * <p>Installs a callback to be invoked for every preview frame, using
     * buffers supplied with {@link #addCallbackBuffer(byte[])}, in addition to
     * displaying them on the screen.  The callback will be repeatedly called
     * for as Int64 as preview is active and buffers are available.  Any other
     * preview callbacks are overridden.</p>
     *
     * <p>The purpose of this method is to improve preview efficiency and frame
     * rate by allowing preview frame memory reuse.  You must call
     * {@link #addCallbackBuffer(byte[])} at some point -- before or after
     * calling this method -- or no callbacks will received.</p>
     *
     * <p>The buffer queue will be cleared if this method is called with a NULL
     * callback, {@link #setPreviewCallback(Camera.PreviewCallback)} is called,
     * or {@link #setOneShotPreviewCallback(Camera.PreviewCallback)} is
     * called.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param cb a callback object that receives a copy of the preview frame,
     *     or NULL to stop receiving callbacks and clear the buffer queue.
     * @see #addCallbackBuffer(byte[])
     * @see android.media.MediaActionSound
     */
    CARAPI SetPreviewCallbackWithBuffer(
        /* [in] */ IPreviewCallback* cb);

    /**
     * Adds a pre-allocated buffer to the preview callback buffer queue.
     * Applications can add one or more buffers to the queue. When a preview
     * frame arrives and there is still at least one available buffer, the
     * buffer will be used and removed from the queue. Then preview callback is
     * invoked with the buffer. If a frame arrives and there is no buffer left,
     * the frame is discarded. Applications should add buffers back when they
     * finish processing the data in them.
     *
     * <p>For formats besides YV12, the size of the buffer is determined by
     * multiplying the preview image width, height, and bytes per pixel. The
     * width and height can be read from
     * {@link Camera.Parameters#getPreviewSize()}. Bytes per pixel can be
     * computed from {@link android.graphics.ImageFormat#getBitsPerPixel(Int32)} /
     * 8, using the image format from
     * {@link Camera.Parameters#getPreviewFormat()}.
     *
     * <p>If using the {@link android.graphics.ImageFormat#YV12} format, the
     * size can be calculated using the equations listed in
     * {@link Camera.Parameters#setPreviewFormat}.
     *
     * <p>This method is only necessary when
     * {@link #setPreviewCallbackWithBuffer(PreviewCallback)} is used. When
     * {@link #setPreviewCallback(PreviewCallback)} or
     * {@link #setOneShotPreviewCallback(PreviewCallback)} are used, buffers
     * are automatically allocated. When a supplied buffer is too small to
     * hold the preview frame data, preview callback will return NULL and
     * the buffer will be removed from the buffer queue.
     *
     * @param callbackBuffer the buffer to add to the queue. The size of the
     *   buffer must match the values described above.
     * @see #setPreviewCallbackWithBuffer(PreviewCallback)
     */
    CARAPI AddCallbackBuffer(
        /* [in] */ ArrayOf<Byte>* callbackBuffer);

    /**
     * Adds a pre-allocated buffer to the raw image callback buffer queue.
     * Applications can add one or more buffers to the queue. When a raw image
     * frame arrives and there is still at least one available buffer, the
     * buffer will be used to hold the raw image data and removed from the
     * queue. Then raw image callback is invoked with the buffer. If a raw
     * image frame arrives but there is no buffer left, the frame is
     * discarded. Applications should add buffers back when they finish
     * processing the data in them by calling this method again in order
     * to avoid running out of raw image callback buffers.
     *
     * <p>The size of the buffer is determined by multiplying the raw image
     * width, height, and bytes per pixel. The width and height can be
     * read from {@link Camera.Parameters#getPictureSize()}. Bytes per pixel
     * can be computed from
     * {@link android.graphics.ImageFormat#getBitsPerPixel(Int32)} / 8,
     * using the image format from {@link Camera.Parameters#getPreviewFormat()}.
     *
     * <p>This method is only necessary when the PictureCallbck for raw image
     * is used while calling {@link #takePicture(Camera.ShutterCallback,
     * Camera.PictureCallback, Camera.PictureCallback, Camera.PictureCallback)}.
     *
     * <p>Please note that by calling this method, the mode for
     * application-managed callback buffers is triggered. If this method has
     * never been called, NULL will be returned by the raw image callback since
     * there is no image callback buffer available. Furthermore, When a supplied
     * buffer is too small to hold the raw image data, raw image callback will
     * return NULL and the buffer will be removed from the buffer queue.
     *
     * @param callbackBuffer the buffer to add to the raw image callback buffer
     *     queue. The size should be width * height * (bits per pixel) / 8. An
     *     NULL callbackBuffer will be ignored and won't be added to the queue.
     *
     * @see #takePicture(Camera.ShutterCallback,
     * Camera.PictureCallback, Camera.PictureCallback, Camera.PictureCallback)}.
     *
     * {@hide}
     */
    CARAPI AddRawImageCallbackBuffer(
        /* [in] */ ArrayOf<Byte>* callbackBuffer);

    /**
     * <p>Create a {@link android.renderscript RenderScript}
     * {@link android.renderscript.Allocation Allocation} to use as a
     * destination of preview callback frames. Use
     * {@link #setPreviewCallbackAllocation setPreviewCallbackAllocation} to use
     * the created Allocation as a destination for camera preview frames.</p>
     *
     * <p>The Allocation will be created with a YUV type, and its contents must
     * be accessed within Renderscript with the {@code rsGetElementAtYuv_*}
     * accessor methods. Its size will be based on the current
     * {@link Parameters#getPreviewSize preview size} configured for this
     * camera.</p>
     *
     * @param rs the RenderScript context for this Allocation.
     * @param usage additional usage flags to set for the Allocation. The usage
     *   flag {@link android.renderscript.Allocation#USAGE_IO_INPUT} will always
     *   be set on the created Allocation, but additional flags may be provided
     *   here.
     * @return a new YUV-type Allocation with dimensions equal to the current
     *   preview size.
     * @throws RSIllegalArgumentException if the usage flags are not compatible
     *   with an YUV Allocation.
     * @see #setPreviewCallbackAllocation
     * @hide
     */
    // CARAPI CreatePreviewAllocation(
    //     /* [in] */ IRenderScript* rs,
    //     /* [in] */ Int32 usage,
    //     /* [out] */ IRenderScriptAllocation** result);

    /**
     * <p>Set an {@link android.renderscript.Allocation Allocation} as the
     * target of preview callback data. Use this method for efficient processing
     * of camera preview data with RenderScript. The Allocation must be created
     * with the {@link #createPreviewAllocation createPreviewAllocation }
     * method.</p>
     *
     * <p>Setting a preview allocation will disable any active preview callbacks
     * set by {@link #setPreviewCallback setPreviewCallback} or
     * {@link #setPreviewCallbackWithBuffer setPreviewCallbackWithBuffer}, and
     * vice versa. Using a preview allocation still requires an active standard
     * preview target to be set, either with
     * {@link #setPreviewTexture setPreviewTexture} or
     * {@link #setPreviewDisplay setPreviewDisplay}.</p>
     *
     * <p>To be notified when new frames are available to the Allocation, use
     * {@link android.renderscript.Allocation#setIoInputNotificationHandler Allocation.setIoInputNotificationHandler}. To
     * update the frame currently accessible from the Allocation to the latest
     * preview frame, call
     * {@link android.renderscript.Allocation#ioReceive Allocation.ioReceive}.</p>
     *
     * <p>To disable preview into the Allocation, call this method with a
     * {@code null} parameter.</p>
     *
     * <p>Once a preview allocation is set, the preview size set by
     * {@link Parameters#setPreviewSize setPreviewSize} cannot be changed. If
     * you wish to change the preview size, first remove the preview allocation
     * by calling {@code setPreviewCallbackAllocation(null)}, then change the
     * preview size, create a new preview Allocation with
     * {@link #createPreviewAllocation createPreviewAllocation}, and set it as
     * the new preview callback allocation target.</p>
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @param previewAllocation the allocation to use as destination for preview
     * @throws IOException if configuring the camera to use the Allocation for
     *   preview fails.
     * @throws IllegalArgumentException if the Allocation's dimensions or other
     *   parameters don't meet the requirements.
     * @see #createPreviewAllocation
     * @see #setPreviewCallback
     * @see #setPreviewCallbackWithBuffer
     * @hide
     */
    //CARAPI SetPreviewCallbackAllocation(
    //    /* [in] */ IRenderScriptAllocation* previewAllocation);

    /**
     * Starts camera auto-focus and registers a callback function to run when
     * the camera is focused.  This method is only valid when preview is active
     * (between {@link #startPreview()} and before {@link #stopPreview()}).
     *
     * <p>Callers should check
     * {@link android.hardware.Camera.Parameters#getFocusMode()} to determine if
     * this method should be called. If the camera does not support auto-focus,
     * it is a no-op and {@link AutoFocusCallback#onAutoFocus(Boolean, Camera)}
     * callback will be called immediately.
     *
     * <p>If your application should not be installed
     * on devices without auto-focus, you must declare that your application
     * uses auto-focus with the
     * <a href="{@docRoot}guide/topics/manifest/uses-feature-element.html">&lt;uses-feature></a>
     * manifest element.</p>
     *
     * <p>If the current flash mode is not
     * {@link android.hardware.Camera.Parameters#FLASH_MODE_OFF}, flash may be
     * fired during auto-focus, depending on the driver and camera hardware.<p>
     *
     * <p>Auto-exposure lock {@link android.hardware.Camera.Parameters#getAutoExposureLock()}
     * and auto-white balance locks {@link android.hardware.Camera.Parameters#getAutoWhiteBalanceLock()}
     * do not change during and after autofocus. But auto-focus routine may stop
     * auto-exposure and auto-white balance transiently during focusing.
     *
     * <p>Stopping preview with {@link #stopPreview()}, or triggering still
     * image capture with {@link #takePicture(Camera.ShutterCallback,
     * Camera.PictureCallback, Camera.PictureCallback)}, will not change the
     * the focus position. Applications must call cancelAutoFocus to reset the
     * focus.</p>
     *
     * <p>If autofocus is successful, consider using
     * {@link android.media.MediaActionSound} to properly play back an autofocus
     * success sound to the user.</p>
     *
     * @param cb the callback to run
     * @see #cancelAutoFocus()
     * @see android.hardware.Camera.Parameters#setAutoExposureLock(Boolean)
     * @see android.hardware.Camera.Parameters#setAutoWhiteBalanceLock(Boolean)
     * @see android.media.MediaActionSound
     */
    CARAPI AutoFocus(
        /* [in] */ IAutoFocusCallback* cb);

    /**
     * Cancels any auto-focus function in progress.
     * Whether or not auto-focus is currently in progress,
     * this function will return the focus position to the default.
     * If the camera does not support auto-focus, this is a no-op.
     *
     * @see #autoFocus(Camera.AutoFocusCallback)
     */
    CARAPI CancelAutoFocus();

    /**
     * Sets camera auto-focus move callback.
     *
     * @param cb the callback to run
     */
    CARAPI SetAutoFocusMoveCallback(
        /* [in] */ IAutoFocusMoveCallback* cb);

    /**
     * Equivalent to takePicture(shutter, raw, NULL, jpeg).
     *
     * @see #takePicture(ShutterCallback, PictureCallback, PictureCallback, PictureCallback)
     */
    CARAPI TakePicture(
        /* [in] */ IShutterCallback* shutter,
        /* [in] */ IPictureCallback* raw,
        /* [in] */ IPictureCallback* jpeg);

    /**
     * Triggers an asynchronous image capture. The camera service will initiate
     * a series of callbacks to the application as the image capture progresses.
     * The shutter callback occurs after the image is captured. This can be used
     * to trigger a sound to let the user know that image has been captured. The
     * raw callback occurs when the raw image data is available (NOTE: the data
     * will be NULL if there is no raw image callback buffer available or the
     * raw image callback buffer is not large enough to hold the raw image).
     * The postview callback occurs when a scaled, fully processed postview
     * image is available (NOTE: not all hardware supports this). The jpeg
     * callback occurs when the compressed image is available. If the
     * application does not need a particular callback, a NULL can be passed
     * instead of a callback method.
     *
     * <p>This method is only valid when preview is active (after
     * {@link #startPreview()}).  Preview will be stopped after the image is
     * taken; callers must call {@link #startPreview()} again if they want to
     * re-start preview or take more pictures. This should not be called between
     * {@link android.media.MediaRecorder#start()} and
     * {@link android.media.MediaRecorder#stop()}.
     *
     * <p>After calling this method, you must not call {@link #startPreview()}
     * or take another picture until the JPEG callback has returned.
     *
     * @param shutter   the callback for image capture moment, or NULL
     * @param raw       the callback for raw (uncompressed) image data, or NULL
     * @param postview  callback with postview image data, may be NULL
     * @param jpeg      the callback for JPEG image data, or NULL
     */
    CARAPI TakePicture(
        /* [in] */ IShutterCallback* shutter,
        /* [in] */ IPictureCallback* raw,
        /* [in] */ IPictureCallback* postview,
        /* [in] */ IPictureCallback* jpeg);

    /**
     * <p>Enable or disable the default shutter sound when taking a picture.</p>
     *
     * <p>By default, the camera plays the system-defined camera shutter sound
     * when {@link #takePicture} is called. Using this method, the shutter sound
     * can be disabled. It is strongly recommended that an alternative shutter
     * sound is played in the {@link ShutterCallback} when the system shutter
     * sound is disabled.</p>
     *
     * <p>Note that devices may not always allow disabling the camera shutter
     * sound. If the shutter sound state cannot be set to the desired value,
     * this method will return FALSE. {@link CameraInfo#canDisableShutterSound}
     * can be used to determine whether the device will allow the shutter sound
     * to be disabled.</p>
     *
     * @param enabled whether the camera should play the system shutter sound
     *                when {@link #takePicture takePicture} is called.
     * @return {@code TRUE} if the shutter sound state was successfully
     *         changed. {@code FALSE} if the shutter sound state could not be
     *         changed. {@code TRUE} is also returned if shutter sound playback
     *         is already set to the requested state.
     * @see #takePicture
     * @see CameraInfo#canDisableShutterSound
     * @see ShutterCallback
     */
    CARAPI EnableShutterSound(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* success);

    /**
     * Disable the shutter sound unconditionally.
     *
     * <p>
     * This is only guaranteed to work for legacy cameras
     * (i.e. initialized with {@link #cameraInitUnspecified}). Trying to call this on
     * a regular camera will force a conditional check in the camera service.
     * </p>
     *
     * @return {@code true} if the shutter sound state was successfully
     *         changed. {@code false} if the shutter sound state could not be
     *         changed. {@code true} is also returned if shutter sound playback
     *         is already set to the requested state.
     *
     * @hide
     */
    CARAPI DisableShutterSound(
        /* [out] */ Boolean* success);

    /**
     * Registers a listener to be notified when the zoom value is updated by the
     * camera driver during smooth zoom.
     *
     * @param listener the listener to notify
     * @see #startSmoothZoom(Int32)
     */
    CARAPI SetZoomChangeListener(
        /* [in] */ IOnZoomChangeListener* listener);

    /**
     * Registers a listener to be notified about the faces detected in the
     * preview frame.
     *
     * @param listener the listener to notify
     * @see #startFaceDetection()
     */
    CARAPI SetFaceDetectionListener(
        /* [in] */ IFaceDetectionListener* listener);

    /**
     * Starts the face detection. This should be called after preview is started.
     * The camera will notify {@link FaceDetectionListener} of the detected
     * faces in the preview frame. The detected faces may be the same as the
     * previous ones. Applications should call {@link #stopFaceDetection} to
     * stop the face detection. This method is supported if {@link
     * Parameters#getMaxNumDetectedFaces()} returns a number larger than 0.
     * If the face detection has started, apps should not call this again.
     *
     * <p>When the face detection is running, {@link Parameters#setWhiteBalance(String)},
     * {@link Parameters#setFocusAreas(List)}, and {@link Parameters#setMeteringAreas(List)}
     * have no effect. The camera uses the detected faces to do auto-white balance,
     * auto exposure, and autofocus.
     *
     * <p>If the apps call {@link #autoFocus(AutoFocusCallback)}, the camera
     * will stop sending face callbacks. The last face callback indicates the
     * areas used to do autofocus. After focus completes, face detection will
     * resume sending face callbacks. If the apps call {@link
     * #cancelAutoFocus()}, the face callbacks will also resume.</p>
     *
     * <p>After calling {@link #takePicture(Camera.ShutterCallback, Camera.PictureCallback,
     * Camera.PictureCallback)} or {@link #stopPreview()}, and then resuming
     * preview with {@link #startPreview()}, the apps should call this method
     * again to resume face detection.</p>
     *
     * @throws IllegalArgumentException if the face detection is unsupported.
     * @throws RuntimeException if the method fails or the face detection is
     *         already running.
     * @see FaceDetectionListener
     * @see #stopFaceDetection()
     * @see Parameters#getMaxNumDetectedFaces()
     */
    CARAPI StartFaceDetection();

    /**
     * Stops the face detection.
     *
     * @see #startFaceDetection()
     */
    CARAPI StopFaceDetection();

    // Error codes match the enum in include/ui/Camera.h

    /**
     * Registers a callback to be invoked when an error occurs.
     * @param cb The callback to run
     */
    CARAPI SetErrorCallback(
        /* [in] */ ICameraErrorCallback* cb);

    /**
     * Changes the settings for this Camera service.
     *
     * @param params the Parameters to use for this Camera service
     * @throws RuntimeException if any parameter is invalid or not supported.
     * @see #getParameters()
     */
    CARAPI SetParameters(
        /* [in] */ IParameters* params);

    /**
     * Returns the current settings for this Camera service.
     * If modifications are made to the returned Parameters, they must be passed
     * to {@link #setParameters(Camera.Parameters)} to take effect.
     *
     * @see #setParameters(Camera.Parameters)
     */
    CARAPI GetParameters(
        /* [out] */ IParameters** para);

    static CARAPI GetEmptyParameters(
        /* [out] */ IParameters** para);

    /**
     * Returns a copied {@link Parameters}; for shim use only.
     *
     * @param parameters a non-{@code null} parameters
     * @return a Parameter object, with all the parameters copied from {@code parameters}.
     *
     * @throws NullPointerException if {@code parameters} was {@code null}
     * @hide
     */
    static CARAPI GetParametersCopy(
        /* [in] */ IParameters* parameters,
        /* [out] */ IParameters** para);

public:
    /**
     * Unlocks the camera to allow another process to access it.
     * Normally, the camera is locked to the process with an active Camera
     * object until {@link #release()} is called.  To allow rapid handoff
     * between processes, you can call this method to release the camera
     * temporarily for another process to use; once the other process is done
     * you can call {@link #reconnect()} to reclaim the camera.
     *
     * <p>This must be done before calling
     * {@link android.media.MediaRecorder#setCamera(Camera)}. This cannot be
     * called after recording starts.
     *
     * <p>If you are not recording video, you probably do not need this method.
     *
     * @throws RuntimeException if the camera cannot be unlocked.
     */
    /*public native final */CARAPI Unlock();

    /**
     * Re-locks the camera to prevent other processes from accessing it.
     * Camera objects are locked by default unless {@link #unlock()} is
     * called.  Normally {@link #reconnect()} is used instead.
     *
     * <p>Since API level 14, camera is automatically locked for applications in
     * {@link android.media.MediaRecorder#start()}. Applications can use the
     * camera (ex: zoom) after recording starts. There is no need to call this
     * after recording starts or stops.
     *
     * <p>If you are not recording video, you probably do not need this method.
     *
     * @throws RuntimeException if the camera cannot be re-locked (for
     *     example, if the camera is still in use by another process).
     */
    /*public native final*/ CARAPI Lock();

    /**
     * Reconnects to the camera service after another process used it.
     * After {@link #unlock()} is called, another process may use the
     * camera; when the process is done, you must reconnect to the camera,
     * which will re-acquire the lock and allow you to continue using the
     * camera.
     *
     * <p>Since API level 14, camera is automatically locked for applications in
     * {@link android.media.MediaRecorder#start()}. Applications can use the
     * camera (ex: zoom) after recording starts. There is no need to call this
     * after recording starts or stops.
     *
     * <p>If you are not recording video, you probably do not need this method.
     *
     * @throws IOException if a connection cannot be re-established (for
     *     example, if the camera is still in use by another process).
     */
    /*public native final*/ CARAPI Reconnect()/* throws IOException*/;

    /**
     * Sets the {@link SurfaceTexture} to be used for live preview.
     * Either a surface or surface texture is necessary for preview, and
     * preview is necessary to take pictures.  The same surface texture can be
     * re-set without harm.  Setting a preview surface texture will un-set any
     * preview surface that was set via {@link #setPreviewDisplay}.
     *
     * <p>This method must be called before {@link #startPreview()}.  The
     * one exception is that if the preview surface texture is not set (or set
     * to NULL) before startPreview() is called, then this method may be called
     * once with a non-NULL parameter to set the preview surface.  (This allows
     * camera setup and surface creation to happen in parallel, saving time.)
     * The preview surface texture may not otherwise change while preview is
     * running.
     *
     * <p>The timestamps provided by {@link SurfaceTexture#getTimestamp()} for a
     * SurfaceTexture set as the preview texture have an unspecified zero point,
     * and cannot be directly compared between different cameras or different
     * instances of the same camera, or across multiple runs of the same
     * program.
     *
     * <p>If you are using the preview data to create video or still images,
     * strongly consider using {@link android.media.MediaActionSound} to
     * properly indicate image capture or recording start/stop to the user.</p>
     *
     * @see android.media.MediaActionSound
     * @see android.graphics.SurfaceTexture
     * @see android.view.TextureView
     * @param surfaceTexture the {@link SurfaceTexture} to which the preview
     *     images are to be sent or NULL to remove the current preview surface
     *     texture
     * @throws IOException if the method fails (for example, if the surface
     *     texture is unavailable or unsuitable).
     */
    CARAPI SetPreviewTexture(
        /* [in] */ ISurfaceTexture* surfaceTexture)/* throws IOException*/;

    /**
     * Starts capturing and drawing preview frames to the screen.
     * Preview will not actually start until a surface is supplied
     * with {@link #setPreviewDisplay(SurfaceHolder)} or
     * {@link #setPreviewTexture(SurfaceTexture)}.
     *
     * <p>If {@link #setPreviewCallback(Camera.PreviewCallback)},
     * {@link #setOneShotPreviewCallback(Camera.PreviewCallback)}, or
     * {@link #setPreviewCallbackWithBuffer(Camera.PreviewCallback)} were
     * called, {@link Camera.PreviewCallback#onPreviewFrame(byte[], Camera)}
     * will be called when preview data becomes available.
     */
    /*public native final*/ CARAPI StartPreview();

    /**
     * Return current preview state.
     *
     * FIXME: Unhide before release
     * @hide
     */
    /*public native final*/ CARAPI PreviewEnabled(
        /* [out] */ Boolean* enabled);

    /**
     * Zooms to the requested value smoothly. The driver will notify {@link
     * OnZoomChangeListener} of the zoom value and whether zoom is stopped at
     * the time. For example, suppose the current zoom is 0 and startSmoothZoom
     * is called with value 3. The
     * {@link Camera.OnZoomChangeListener#onZoomChange(Int32, Boolean, Camera)}
     * method will be called three times with zoom values 1, 2, and 3.
     * Applications can call {@link #stopSmoothZoom} to stop the zoom earlier.
     * Applications should not call startSmoothZoom again or change the zoom
     * value before zoom stops. If the supplied zoom value equals to the current
     * zoom value, no zoom callback will be generated. This method is supported
     * if {@link android.hardware.Camera.Parameters#isSmoothZoomSupported}
     * returns TRUE.
     *
     * @param value zoom value. The valid range is 0 to {@link
     *              android.hardware.Camera.Parameters#getMaxZoom}.
     * @throws IllegalArgumentException if the zoom value is invalid.
     * @throws RuntimeException if the method fails.
     * @see #setZoomChangeListener(OnZoomChangeListener)
     */
    /*public native final*/ CARAPI StartSmoothZoom(
        /* [in] */ Int32 value);

    /**
     * Stops the smooth zoom. Applications should wait for the {@link
     * OnZoomChangeListener} to know when the zoom is actually stopped. This
     * method is supported if {@link
     * android.hardware.Camera.Parameters#isSmoothZoomSupported} is TRUE.
     *
     * @throws RuntimeException if the method fails.
     */
    /*public native final*/ CARAPI StopSmoothZoom();

    /**
     * Set the clockwise rotation of preview display in degrees. This affects
     * the preview frames and the picture displayed after snapshot. This method
     * is useful for portrait mode applications. Note that preview display of
     * front-facing cameras is flipped horizontally before the rotation, that
     * is, the image is reflected along the central vertical axis of the camera
     * sensor. So the users can see themselves as looking into a mirror.
     *
     * <p>This does not affect the order of byte array passed in {@link
     * PreviewCallback#onPreviewFrame}, JPEG pictures, or recorded videos. This
     * method is not allowed to be called during preview.
     *
     * <p>If you want to make the camera image show in the same orientation as
     * the display, you can use the following code.
     * <pre>
     * public static void setCameraDisplayOrientation(Activity activity,
     *         Int32 cameraId, android.hardware.Camera camera) {
     *     android.hardware.Camera.CameraInfo info =
     *             new android.hardware.Camera.CameraInfo();
     *     android.hardware.Camera.getCameraInfo(cameraId, info);
     *     Int32 rotation = activity.getWindowManager().getDefaultDisplay()
     *             .getRotation();
     *     Int32 degrees = 0;
     *     switch (rotation) {
     *         case Surface.ROTATION_0: degrees = 0; break;
     *         case Surface.ROTATION_90: degrees = 90; break;
     *         case Surface.ROTATION_180: degrees = 180; break;
     *         case Surface.ROTATION_270: degrees = 270; break;
     *     }
     *
     *     Int32 result;
     *     if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
     *         result = (info.orientation + degrees) % 360;
     *         result = (360 - result) % 360;  // compensate the mirror
     *     } else {  // back-facing
     *         result = (info.orientation - degrees + 360) % 360;
     *     }
     *     camera.setDisplayOrientation(result);
     * }
     * </pre>
     *
     * <p>Starting from API level 14, this method can be called when preview is
     * active.
     *
     * @param degrees the angle that the picture will be rotated clockwise.
     *                Valid values are 0, 90, 180, and 270. The starting
     *                position is 0 (landscape).
     * @see #setPreviewDisplay(SurfaceHolder)
     */
    CARAPI SetDisplayOrientation(
        /* [in] */ Int32 degrees);

    static android::sp<android::Camera> get_native_camera(
        /* [in] */ HardwareCamera* thiz,
        /* [in] */ JNICameraContext** pContext);

private:
    CARAPI_(Int32) CameraInitVersion(
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 halVersion);

    CARAPI_(Int32) CameraInitNormal(
        /* [in] */ Int32 cameraId);

    CARAPI setPreviewCallbackSurface(
        /* [in] */ ISurface* s);

    //private native method.
    CARAPI native_setup(
        /* [in] */ HardwareCamera* camera_this,
        /* [in] */ Int32 cameraId,
        /* [in] */ Int32 halVersion,
        /* [in] */ const String& packageName,
        /* [out] */ Int32* result);

    void native_release();

    void _stopPreview();

    void setHasPreviewCallback(
        /* [in] */ Boolean installed,
        /* [in] */ Boolean manualBuffer);

    void _addCallbackBuffer(
        /* [in] */ ArrayOf<Byte>* callbackBuffer,
        /* [in] */ Int32 msgType);

    CARAPI native_autoFocus();

    CARAPI native_cancelAutoFocus();

    CARAPI enableFocusMoveCallback(
        /* [in] */ Int32 enable);

    CARAPI native_takePicture(
        /* [in] */ Int32 msgType);

    CARAPI _enableShutterSound(
        /* [in] */ Boolean enabled,
        /* [out] */ Boolean* result);

    CARAPI _startFaceDetection(
        /* [in] */ Int32 type);

    CARAPI _stopFaceDetection();

    CARAPI native_setParameters(
        /* [in] */ const String& params);

    CARAPI native_getParameters(
        /* [out] */ String* para);

    /*native*/ static CARAPI _getCameraInfo(
        /* [in] */ Int32 cameraId,
        /* [in] */ IHardwareCameraInfo* cameraInfo);


private:
    CARAPI AddCallbackBuffer(
        /* [in] */ ArrayOf<Byte>* callbackBuffer,
        /* [in] */ Int32 msgType);

    static void PostEventFromNative(
        /* [in] */ HardwareCamera* camera_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ const void* data);

private:
    static String TAG;

    // These match the enums in frameworks/base/include/camera/Camera.h
    static const Int32 CAMERA_MSG_ERROR;
    static const Int32 CAMERA_MSG_SHUTTER;
    static const Int32 CAMERA_MSG_FOCUS;
    static const Int32 CAMERA_MSG_ZOOM;
    static const Int32 CAMERA_MSG_PREVIEW_FRAME;
    static const Int32 CAMERA_MSG_VIDEO_FRAME;
    static const Int32 CAMERA_MSG_POSTVIEW_FRAME;
    static const Int32 CAMERA_MSG_RAW_IMAGE;
    static const Int32 CAMERA_MSG_COMPRESSED_IMAGE;
    static const Int32 CAMERA_MSG_RAW_IMAGE_NOTIFY;
    static const Int32 CAMERA_MSG_PREVIEW_METADATA;
    static const Int32 CAMERA_MSG_FOCUS_MOVE;

    /**
     * A constant meaning the normal camera connect/open will be used.
     */
    static const Int32 CAMERA_HAL_API_VERSION_NORMAL_CONNECT;

    /**
     * Used to indicate HAL version un-specified.
     */
    static const Int32 CAMERA_HAL_API_VERSION_UNSPECIFIED;

    /**
     * Hardware face detection. It does not use much CPU.
     */
    static const Int32 CAMERA_FACE_DETECTION_HW;

    /**
     * Software face detection. It uses some CPU.
     */
    static const Int32 CAMERA_FACE_DETECTION_SW;

    Int64 mNativeContext; // accessed by native methods
    // EventHandler mEventHandler;
    AutoPtr<EventHandler> mEventHandler;
    AutoPtr<IShutterCallback> mShutterCallback;
    AutoPtr<IPictureCallback> mRawImageCallback;
    AutoPtr<IPictureCallback> mJpegCallback;
    AutoPtr<IPreviewCallback> mPreviewCallback;
    Boolean mUsingPreviewAllocation;
    AutoPtr<IPictureCallback> mPostviewCallback;
    AutoPtr<IAutoFocusCallback> mAutoFocusCallback;
    AutoPtr<IAutoFocusMoveCallback> mAutoFocusMoveCallback;
    AutoPtr<IOnZoomChangeListener> mZoomListener;
    AutoPtr<IFaceDetectionListener> mFaceListener;
    AutoPtr<ICameraErrorCallback> mErrorCallback;
    Boolean mOneShot;
    Boolean mWithBuffer;
    Boolean mFaceDetectionRunning;
    Object mAutoFocusCallbackLock;

    static const Int32 HardwareCamera_NO_ERROR;
    static const Int32 HardwareCamera_EACCESS;
    static const Int32 HardwareCamera_ENODEV;
    static const Int32 HardwareCamera_EBUSY;
    static const Int32 HardwareCamera_EINVAL;
    static const Int32 HardwareCamera_ENOSYS;
    static const Int32 HardwareCamera_EUSERS;
    static const Int32 HardwareCamera_EOPNOTSUPP;
};

} // namespace Hardware
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Hardware::HardwareCamera::Face, IInterface)

#endif  //__ELASTOS_DROID_HARDWARE_HARDWARECAMERA_H__
