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

#ifndef __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CFACE_H__
#define __ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CFACE_H__

#include "_Elastos_Droid_Hardware_Camera2_Params_CFace.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Graphics::IPoint;
using Elastos::Core::Object;

namespace Elastos {
namespace Droid {
namespace Hardware {
namespace Camera2 {
namespace Params {

CarClass(CFace)
    , public Object
    , public IFace
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CFace();

    virtual ~CFace();

    CARAPI constructor();

    /**
     * Create a new face with all fields set.
     *
     * <p>The id, leftEyePosition, rightEyePosition, and mouthPosition are considered optional.
     * They are only required when the {@link CaptureResult} reports that the value of key
     * {@link CaptureResult#STATISTICS_FACE_DETECT_MODE} is
     * {@link CameraMetadata#STATISTICS_FACE_DETECT_MODE_FULL}.
     * If the id is {@value #ID_UNSUPPORTED} then the leftEyePosition, rightEyePosition, and
     * mouthPositions are guaranteed to be {@code null}. Otherwise, each of leftEyePosition,
     * rightEyePosition, and mouthPosition may be independently null or not-null.</p>
     *
     * @param bounds Bounds of the face.
     * @param score Confidence level between {@value #SCORE_MIN}-{@value #SCORE_MAX}.
     * @param id A unique ID per face visible to the tracker.
     * @param leftEyePosition The position of the left eye.
     * @param rightEyePosition The position of the right eye.
     * @param mouthPosition The position of the mouth.
     *
     * @throws IllegalArgumentException
     *             if bounds is {@code null},
     *             or if the confidence is not in the range of
     *             {@value #SCORE_MIN}-{@value #SCORE_MAX},
     *             or if id is {@value #ID_UNSUPPORTED} and
     *               leftEyePosition/rightEyePosition/mouthPosition aren't all null,
     *             or else if id is negative.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 score,
        /* [in] */ Int32 id,
        /* [in] */ IPoint* leftEyePosition,
        /* [in] */ IPoint* rightEyePosition,
        /* [in] */ IPoint* mouthPosition);
    /**
     * Create a new face without the optional fields.
     *
     * <p>The id, leftEyePosition, rightEyePosition, and mouthPosition are considered optional.
     * If the id is {@value #ID_UNSUPPORTED} then the leftEyePosition, rightEyePosition, and
     * mouthPositions are guaranteed to be {@code null}. Otherwise, each of leftEyePosition,
     * rightEyePosition, and mouthPosition may be independently null or not-null. When devices
     * report the value of key {@link CaptureResult#STATISTICS_FACE_DETECT_MODE} as
     * {@link CameraMetadata#STATISTICS_FACE_DETECT_MODE_SIMPLE} in {@link CaptureResult},
     * the face id of each face is expected to be {@value #ID_UNSUPPORTED}, the leftEyePosition,
     * rightEyePosition, and mouthPositions are expected to be {@code null} for each face.</p>
     *
     * @param bounds Bounds of the face.
     * @param score Confidence level between {@value #SCORE_MIN}-{@value #SCORE_MAX}.
     *
     * @throws IllegalArgumentException
     *             if bounds is {@code null},
     *             or if the confidence is not in the range of
     *             {@value #SCORE_MIN}-{@value #SCORE_MAX}.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IRect* bounds,
        /* [in] */ Int32 score);

    /**
     * Bounds of the face.
     *
     * <p>A rectangle relative to the sensor's
     * {@link CameraCharacteristics#SENSOR_INFO_ACTIVE_ARRAY_SIZE}, with (0,0)
     * representing the top-left corner of the active array rectangle.</p>
     *
     * <p>There is no constraints on the the Rectangle value other than it
     * is not-{@code null}.</p>
     */
    CARAPI GetBounds(
        /* [out] */ IRect** outrect);

    /**
     * The confidence level for the detection of the face.
     *
     * <p>The range is {@value #SCORE_MIN} to {@value #SCORE_MAX}.
     * {@value #SCORE_MAX} is the highest confidence.</p>
     *
     * <p>Depending on the device, even very low-confidence faces may be
     * listed, so applications should filter out faces with low confidence,
     * depending on the use case. For a typical point-and-shoot camera
     * application that wishes to display rectangles around detected faces,
     * filtering out faces with confidence less than half of {@value #SCORE_MAX}
     * is recommended.</p>
     *
     * @see #SCORE_MAX
     * @see #SCORE_MIN
     */
    CARAPI GetScore(
        /* [out] */ Int32* value);

    /**
     * An unique id per face while the face is visible to the tracker.
     *
     * <p>
     * If the face leaves the field-of-view and comes back, it will get a new
     * id.</p>
     *
     * <p>This is an optional field, may not be supported on all devices.
     * If the id is {@value #ID_UNSUPPORTED} then the leftEyePosition, rightEyePosition, and
     * mouthPositions are guaranteed to be {@code null}. Otherwise, each of leftEyePosition,
     * rightEyePosition, and mouthPosition may be independently null or not-null. When devices
     * report the value of key {@link CaptureResult#STATISTICS_FACE_DETECT_MODE} as
     * {@link CameraMetadata#STATISTICS_FACE_DETECT_MODE_SIMPLE} in {@link CaptureResult},
     * the face id of each face is expected to be {@value #ID_UNSUPPORTED}.</p>
     *
     * <p>This value will either be {@value #ID_UNSUPPORTED} or
     * otherwise greater than {@code 0}.</p>
     *
     * @see #ID_UNSUPPORTED
     */
    CARAPI GetId(
        /* [out] */ Int32* value);

    /**
     * The coordinates of the center of the left eye.
     *
     * <p>The coordinates are in
     * the same space as the ones for {@link #getBounds}. This is an
     * optional field, may not be supported on all devices. If not
     * supported, the value will always be set to null.
     * This value will  always be null only if {@link #getId()} returns
     * {@value #ID_UNSUPPORTED}.</p>
     *
     * @return The left eye position, or {@code null} if unknown.
     */
    CARAPI GetLeftEyePosition(
        /* [out] */ IPoint** outpoint);

    /**
     * The coordinates of the center of the right eye.
     *
     * <p>The coordinates are
     * in the same space as the ones for {@link #getBounds}.This is an
     * optional field, may not be supported on all devices. If not
     * supported, the value will always be set to null.
     * This value will  always be null only if {@link #getId()} returns
     * {@value #ID_UNSUPPORTED}.</p>
     *
     * @return The right eye position, or {@code null} if unknown.
     */
    CARAPI GetRightEyePosition(
        /* [out] */ IPoint** outpoint);

    /**
     * The coordinates of the center of the mouth.
     *
     * <p>The coordinates are in
     * the same space as the ones for {@link #getBounds}. This is an optional
     * field, may not be supported on all devices. If not
     * supported, the value will always be set to null.
     * This value will  always be null only if {@link #getId()} returns
     * {@value #ID_UNSUPPORTED}.</p>
     * </p>
     *
     * @return The mouth position, or {@code null} if unknown.
     */
    CARAPI GetMouthPosition(
        /* [out] */ IPoint** outpoint);

    /**
     * Represent the Face as a string for debugging purposes.
     */
    // @Override
    CARAPI ToString(
        /* [out] */ String* str);

private:
    static CARAPI CheckNotNull(
        /* [in] */ const String& name,
        /* [in] */ IInterface* obj);

    static CARAPI CheckNull(
        /* [in] */ const String& name,
        /* [in] */ IInterface* obj);

private:
    AutoPtr<IRect> mBounds;
    Int32 mScore;
    Int32 mId;
    AutoPtr<IPoint> mLeftEye;
    AutoPtr<IPoint> mRightEye;
    AutoPtr<IPoint> mMouth;
};

} // namespace Params
} // namespace Camera2
} // namespace Hardware
} // namespace Droid
} // namespace Elastos

#endif  //__ELASTOS_DROID_HARDWARE_CAMERA2_PARAMS_CFACE_H__
