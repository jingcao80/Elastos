
#ifndef __ELASTOS_DROID_ANIMATION_PATHKEYFRAMES_H__
#define __ELASTOS_DROID_ANIMATION_PATHKEYFRAMES_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Animation.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Graphics::IPointF;
using Elastos::Droid::Graphics::IPath;
using Elastos::Core::ICloneable;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Animation {

/**
 * PathKeyframes relies on approximating the Path as a series of line segments.
 * The line segments are recursively divided until there is less than 1/2 pixel error
 * between the lines and the curve. Each point of the line segment is converted
 * to a Keyframe and a linear interpolation between Keyframes creates a good approximation
 * of the curve.
 * <p>
 * PathKeyframes is optimized to reduce the number of objects created when there are
 * many keyframes for a curve.
 * </p>
 * <p>
 * Typically, the returned type is a PointF, but the individual components can be extracted
 * as either an IntKeyframes or FloatKeyframes.
 * </p>
 */
class PathKeyframes
    : public Object
    , public ICloneable
    , public IKeyframes
    , public IPathKeyframes
{
private:
    class SimpleKeyframes
        : public Object
        , public IKeyframes
    {
    public:
        CAR_INTERFACE_DECL();

        virtual ~SimpleKeyframes() = 0;

        // @Override
        virtual CARAPI SetEvaluator(
            /* [in] */ ITypeEvaluator* evaluator);

        // @Override
        virtual CARAPI InvalidateCache();

        // @Override
        virtual CARAPI GetKeyframes(
            /* [out, callee] */ ArrayOf<IKeyframe*>** list);

        // @Override
        virtual CARAPI Clone(
            /* [out] */ IInterface** values);
    };

    class Int32KeyframesBase
        : public SimpleKeyframes
        , public IInt32Keyframes
    {
    public:
        CAR_INTERFACE_DECL();

        virtual ~Int32KeyframesBase() = 0;

        // @Override
        virtual CARAPI GetType(
            /* [out] */ InterfaceID* type);

        // @Override
        virtual CARAPI GetValue(
            /* [in] */ Float fraction,
            /* [out] */ IInterface** value);
    };

    class FloatKeyframesBase
        : public SimpleKeyframes
        , public IFloatKeyframes
    {
    public:
        CAR_INTERFACE_DECL();

        virtual ~FloatKeyframesBase() = 0;

        // @Override
        virtual CARAPI GetType(
            /* [out] */ InterfaceID* type);

        // @Override
        virtual CARAPI GetValue(
            /* [in] */ Float fraction,
            /* [out] */ IInterface** value);
    };

    class FloatKeyframesBase1
        : public FloatKeyframesBase
    {
    public:
        CARAPI GetFloatValue(
            /* [in] */ Float fraction,
            /* [out] */ Float* value);
    };

    class FloatKeyframesBase2
        : public FloatKeyframesBase
    {
    public:
        CARAPI GetFloatValue(
            /* [in] */ Float fraction,
            /* [out] */ Float* value);
    };

    class Int32KeyframesBase1
        : public Int32KeyframesBase
    {
    public:
        CARAPI GetInt32Value(
            /* [in] */ Float fraction,
            /* [out] */ Int32* value);
    };

    class Int32KeyframesBase2
        : public Int32KeyframesBase
    {
    public:
        CARAPI GetInt32Value(
            /* [in] */ Float fraction,
            /* [out] */ Int32* value);
    };

public:
    CAR_INTERFACE_DECL();

    PathKeyframes(
        /* [in] */ IPath* path);

    PathKeyframes(
        /* [in] */ IPath* path,
        /* [in] */ Float error);

    // @Override
    virtual CARAPI GetKeyframes(
        /* [out, callee] */ ArrayOf<IKeyframe*>** list);

    // @Override
    virtual CARAPI GetValue(
        /* [in] */ Float fraction,
        /* [out] */ IInterface** value);

    // @Override
    virtual CARAPI InvalidateCache();

    // @Override
    virtual CARAPI SetEvaluator(
        /* [in] */ ITypeEvaluator* evaluator);

    // @Override
    virtual CARAPI GetType(
        /* [out] */ InterfaceID* type);

    // @Override
    virtual CARAPI Clone(
        /* [out] */ IInterface** frames);

    /**
     * Returns a FloatKeyframes for the X component of the Path.
     * @return a FloatKeyframes for the X component of the Path.
     */
    virtual CARAPI_(AutoPtr<IFloatKeyframes>) CreateXFloatKeyframes();

    /**
     * Returns a FloatKeyframes for the Y component of the Path.
     * @return a FloatKeyframes for the Y component of the Path.
     */
    virtual CARAPI_(AutoPtr<IFloatKeyframes>) CreateYFloatKeyframes();

    /**
     * Returns an IntKeyframes for the X component of the Path.
     * @return an IntKeyframes for the X component of the Path.
     */
    virtual CARAPI_(AutoPtr<IInt32Keyframes>) CreateXInt32Keyframes();

    /**
     * Returns an IntKeyframeSet for the Y component of the Path.
     * @return an IntKeyframeSet for the Y component of the Path.
     */
    virtual CARAPI_(AutoPtr<IInt32Keyframes>) CreateYInt32Keyframes();

private:
    CARAPI constructor(
        /* [in] */ IPath* path,
        /* [in] */ Float error);

    CARAPI InterpolateInRange(
        /* [in] */ Float fraction,
        /* [in] */ Int32 startIndex,
        /* [in] */ Int32 endIndex,
        /* [out] */ IPointF** value);

    CARAPI PointForIndex(
        /* [in] */ Int32 index,
        /* [out] */ IPointF** pf);

    static CARAPI_(Float) Interpolate(
        /* [in] */ Float fraction,
        /* [in] */ Float startValue,
        /* [in] */ Float endValue);

private:
    static const Int32 FRACTION_OFFSET;
    static const Int32 X_OFFSET;
    static const Int32 Y_OFFSET;
    static const Int32 NUM_COMPONENTS;
    static AutoPtr<ArrayOf<IKeyframe*> > EMPTY_KEYFRAMES;

    AutoPtr<IPointF> mTempPointF;
    AutoPtr<ArrayOf<Float> > mKeyframeData;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_PATHKEYFRAMES_H__
