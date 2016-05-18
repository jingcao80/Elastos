
#ifndef __ELASTOS_DROID_GRAPHICS_CINTERPOLATOR_H__
#define __ELASTOS_DROID_GRAPHICS_CINTERPOLATOR_H__

#include "_Elastos_Droid_Graphics_CInterpolator.h"
#include <elastos/core/Object.h>


namespace Elastos {
namespace Droid {
namespace Graphics {

CarClass(CInterpolator)
    , public Object
    , public IInterpolator
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CARAPI constructor(
        /* [in] */ Int32 valueCount);

    CARAPI constructor(
        /* [in] */ Int32 valueCount,
        /* [in] */ Int32 frameCount);

    CARAPI GetKeyFrameCount(
        /* [out] */ Int32* count);

    CARAPI GetValueCount(
        /* [out] */ Int32* count);

    /**
     * Reset the Interpolator to have the specified number of values and an
     * implicit keyFrame count of 2 (just a start and end). After this call the
     * values for each keyFrame must be assigned using setKeyFrame().
     */
    CARAPI Reset(
        /* [in] */ Int32 valueCount);

    /**
     * Reset the Interpolator to have the specified number of values and
     * keyFrames. After this call the values for each keyFrame must be assigned
     * using setKeyFrame().
     */
    CARAPI Reset(
        /* [in] */ Int32 valueCount,
        /* [in] */ Int32 frameCount);

    /**
     * Assign the keyFrame (specified by index) a time value and an array of key
     * values (with an implicity blend array of [0, 0, 1, 1] giving linear
     * transition to the next set of key values).
     *
     * @param index The index of the key frame to assign
     * @param msec The time (in mililiseconds) for this key frame. Based on the
     *        SystemClock.uptimeMillis() clock
     * @param values Array of values associated with theis key frame
     */
    CARAPI SetKeyFrame(
        /* [in] */ Int32 index,
        /* [in] */ Int32 msec,
        /* [in] */ ArrayOf<Float>* values);

    /**
     * Assign the keyFrame (specified by index) a time value and an array of key
     * values and blend array.
     *
     * @param index The index of the key frame to assign
     * @param msec The time (in mililiseconds) for this key frame. Based on the
     *        SystemClock.uptimeMillis() clock
     * @param values Array of values associated with theis key frame
     * @param blend (may be null) Optional array of 4 blend values
     */
    CARAPI SetKeyFrame(
        /* [in] */ Int32 index,
        /* [in] */ Int32 msec,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ ArrayOf<Float>* blendArray);

    /**
     * Set a repeat count (which may be fractional) for the interpolator, and
     * whether the interpolator should mirror its repeats. The default settings
     * are repeatCount = 1, and mirror = false.
     */
    CARAPI SetRepeatMirror(
        /* [in] */ Float repeatCount,
        /* [in] */ Boolean mirror);

    /**
     * Calls timeToValues(msec, values) with the msec set to now (by calling
     * (Int32)SystemClock.uptimeMillis().)
     */
    CARAPI TimeToValues(
        /* [out] */ ArrayOf<Float>* values,
        /* [out] */ InterpolatorResult* result);

    /**
     * Given a millisecond time value (msec), return the interpolated values and
     * return whether the specified time was within the range of key times
     * (NORMAL), was before the first key time (FREEZE_START) or after the last
     * key time (FREEZE_END). In any event, computed values are always returned.
     *
     * @param msec The time (in milliseconds) used to sample into the
     *        Interpolator. Based on the SystemClock.uptimeMillis() clock
     * @param values Where to write the computed values (may be NULL).
     * @return how the values were computed (even if values == null)
     */
    CARAPI TimeToValues(
        /* [in] */ Int32 msec,
        /* [out] */ ArrayOf<Float>* values,
        /* [out] */ InterpolatorResult* result);

    ~CInterpolator();

private:
    static CARAPI_(Int64) NativeConstructor(
        /* [in] */ Int32 valueCount,
        /* [in] */ Int32 frameCount);

    static CARAPI_(void) NativeDestructor(
        /* [in] */ Int64 native_instance);

    static CARAPI_(void) NativeReset(
        /* [in] */ Int64 native_instance,
        /* [in] */ Int32 valueCount,
        /* [in] */ Int32 frameCount);

    static CARAPI_(void) NativeSetKeyFrame(
        /* [in] */ Int64 native_instance,
        /* [in] */ Int32 index,
        /* [in] */ Int32 msec,
        /* [in] */ ArrayOf<Float>* values,
        /* [in] */ ArrayOf<Float>* blend);

    static CARAPI_(void) NativeSetRepeatMirror(
        /* [in] */ Int64 native_instance,
        /* [in] */ Float repeatCount,
        /* [in] */ Boolean mirror);

    static CARAPI_(Int32) NativeTimeToValues(
        /* [in] */ Int64 native_instance,
        /* [in] */ Int32 msec,
        /* [in] */ ArrayOf<Float>* values);

private:
    Int32 mValueCount;
    Int32 mFrameCount;
    Int32 mSkInstance;
    Int64 mNativeInstance;
};

} // namespace Graphics
} // namepsace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_GRAPHICS_CINTERPOLATOR_H__
