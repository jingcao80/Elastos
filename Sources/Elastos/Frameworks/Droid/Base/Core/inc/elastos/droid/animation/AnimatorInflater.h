
#ifndef  __ELASTOS_DROID_ANIMATION_ANIMATORINFLATER_H__
#define  __ELASTOS_DROID_ANIMATION_ANIMATORINFLATER_H__

#include "Elastos.Droid.Animation.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/utility/PathParser.h"

using Org::Xmlpull::V1::IXmlPullParser;
using Elastos::Droid::Utility::IAttributeSet;
using Elastos::Droid::Utility::PathDataNode;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Content::Res::IResourcesTheme;
using Elastos::Droid::Content::Res::ITypedArray;

namespace Elastos {
namespace Droid {
namespace Animation {

class AnimatorInflater
{
private:
    /**
     * PathDataEvaluator is used to interpolate between two paths which are
     * represented in the same format but different control points' values.
     * The path is represented as an array of PathDataNode here, which is
     * fundamentally an array of floating point numbers.
     */
    //private static class PathDataEvaluator implements TypeEvaluator<PathParser.PathDataNode[]> {
    class PathDataEvaluator
        : public Object
        , public ITypeEvaluator
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * Create a PathDataEvaluator that reuses <code>nodeArray</code> for every evaluate() call.
         * Caution must be taken to ensure that the value returned from
         * {@link android.animation.ValueAnimator#getAnimatedValue()} is not cached, modified, or
         * used across threads. The value will be modified on each <code>evaluate()</code> call.
         *
         * @param nodeArray The array to modify and return from <code>evaluate</code>.
         */
        PathDataEvaluator(
            /* [in] */ ArrayOf<PathDataNode*>* nodeArray);

        CARAPI Evaluate(
            /* [in] */ Float fraction,
            /* [in] */ IInterface* startPathData,
            /* [in] */ IInterface* endPathData,
            /* [out] */ IInterface** result);

    private:
        /**
         * Create a PathParser.PathDataNode[] that does not reuse the animated value.
         * Care must be taken when using this option because on every evaluation
         * a new <code>PathParser.PathDataNode[]</code> will be allocated.
         */
        PathDataEvaluator() {}

    private:
        AutoPtr<ArrayOf<PathDataNode*> > mNodeArray;
    };

public:
    /**
     * Loads an {@link Animator} object from a resource
     *
     * @param context Application context used to access resources
     * @param id The resource id of the animation to load
     * @return The animator object reference by the specified id
     * @throws android.content.res.Resources.NotFoundException when the animation cannot be loaded
     */
    static CARAPI LoadAnimator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IAnimator** animator) /*throws NotFoundException*/;

    /**
     * Loads an {@link Animator} object from a resource
     *
     * @param resources The resources
     * @param theme The theme
     * @param id The resource id of the animation to load
     * @return The animator object reference by the specified id
     * @throws android.content.res.Resources.NotFoundException when the animation cannot be loaded
     * @hide
     */
    static CARAPI LoadAnimator(
        /* [in] */ IResources* resources,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 id,
        /* [out] */ IAnimator** animator) /*throws NotFoundException*/;

    /** @hide */
    static CARAPI LoadAnimator(
        /* [in] */ IResources* resources,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ Int32 id,
        /* [in] */ Float pathErrorScale,
        /* [out] */ IAnimator** animator) /*throws NotFoundException*/;

    static CARAPI LoadStateListAnimator(
        /* [in] */ IContext* context,
        /* [in] */ Int32 id,
        /* [out] */ IStateListAnimator** animator) /*throws NotFoundException*/;

private:
    static CARAPI CreateStateListAnimatorFromXml(
        /* [in] */ IContext* context,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attributeSet,
        /* [out] */ IStateListAnimator** animator) /*throws IOException, XmlPullParserException*/;

    /**
     * @param anim The animator, must not be null
     * @param arrayAnimator Incoming typed array for Animator's attributes.
     * @param arrayObjectAnimator Incoming typed array for Object Animator's
     *            attributes.
     * @param pixelSize The relative pixel size, used to calculate the
     *                  maximum error for path animations.
     */
    static CARAPI_(void) ParseAnimatorFromTypeArray(
        /* [in] */ IValueAnimator* anim,
        /* [in] */ ITypedArray* arrayAnimator,
        /* [in] */ ITypedArray* arrayObjectAnimator,
        /* [in] */ Float pixelSize);

    /**
     * Setup the Animator to achieve path morphing.
     *
     * @param anim The target Animator which will be updated.
     * @param arrayAnimator TypedArray for the ValueAnimator.
     * @return the PathDataEvaluator.
     */
    static CARAPI SetupAnimatorForPath(
        /* [in] */ IValueAnimator* anim,
        /* [in] */ ITypedArray* arrayAnimator,
        /* [out] */ ITypeEvaluator** te);

    /**
     * Setup ObjectAnimator's property or values from pathData.
     *
     * @param anim The target Animator which will be updated.
     * @param arrayObjectAnimator TypedArray for the ObjectAnimator.
     * @param getFloats True if the value type is float.
     * @param pixelSize The relative pixel size, used to calculate the
     *                  maximum error for path animations.
     */
    static CARAPI SetupObjectAnimator(
        /* [in] */ IValueAnimator* anim,
        /* [in] */ ITypedArray* arrayObjectAnimator,
        /* [in] */ Boolean getFloats,
        /* [in] */ Float pixelSize);

    /**
     * Setup ValueAnimator's values.
     * This will handle all of the integer, float and color types.
     *
     * @param anim The target Animator which will be updated.
     * @param arrayAnimator TypedArray for the ValueAnimator.
     * @param getFloats True if the value type is float.
     * @param hasFrom True if "valueFrom" exists.
     * @param fromType The type of "valueFrom".
     * @param hasTo True if "valueTo" exists.
     * @param toType The type of "valueTo".
     */
    static CARAPI_(void) SetupValues(
        /* [in] */ IValueAnimator* anim,
        /* [in] */ ITypedArray* arrayAnimator,
        /* [in] */ Boolean getFloats,
        /* [in] */ Boolean hasFrom,
        /* [in] */ Int32 fromType,
        /* [in] */ Boolean hasTo,
        /* [in] */ Int32 toType);

    static CARAPI CreateAnimatorFromXml(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ Float pixelSize,
        /* [out] */ IAnimator** animator) /*throws XmlPullParserException, IOException */;

    static CARAPI CreateAnimatorFromXml(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IAnimatorSet* parent,
        /* [in] */ Int32 sequenceOrdering,
        /* [in] */ Float pixelSize,
        /* [out] */ IAnimator** animator) /*throws XmlPullParserException, IOException*/;

    static CARAPI LoadObjectAnimator(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ Float pathErrorScale,
        /* [out] */ IObjectAnimator** animator) /*throws NotFoundException*/;

    /**
     * Creates a new animation whose parameters come from the specified context
     * and attributes set.
     *
     * @param res The resources
     * @param attrs The set of attributes holding the animation parameters
     * @param anim Null if this is a ValueAnimator, otherwise this is an
     *            ObjectAnimator
     */
    static CARAPI LoadAnimator(
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IValueAnimator* anim,
        /* [in] */ Float pathErrorScale,
        /* [out] */ IValueAnimator** animator) /*throws NotFoundException*/;

private:
    static const String TAG;

    /**
     * These flags are used when parsing AnimatorSet objects
     */
    static const Int32 TOGETHER;
    static const Int32 SEQUENTIALLY;

    /**
     * Enum values used in XML attributes to indicate the value for mValueType
     */
    static const Int32 VALUE_TYPE_FLOAT;
    static const Int32 VALUE_TYPE_INT;
    static const Int32 VALUE_TYPE_PATH;
    static const Int32 VALUE_TYPE_COLOR;
    static const Int32 VALUE_TYPE_CUSTOM;
    static const Boolean DBG_ANIMATOR_INFLATER;
};

}   //namespace Animation
}   //namespace Droid
}   //namespace Elastos

#endif  // __ELASTOS_DROID_ANIMATION_ANIMATORINFLATER_H__
