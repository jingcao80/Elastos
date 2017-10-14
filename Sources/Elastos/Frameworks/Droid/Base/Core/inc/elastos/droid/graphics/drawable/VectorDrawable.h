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

#ifndef __ELASTOS_DROID_GRAPHICS_DRAWABLE_VECTORDRAWABLE_H__
#define __ELASTOS_DROID_GRAPHICS_DRAWABLE_VECTORDRAWABLE_H__

#include "Elastos.Droid.Utility.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/graphics/drawable/Drawable.h"
#include "elastos/droid/utility/PathParser.h"

using Elastos::Droid::Utility::IArrayMap;
using Elastos::Droid::Utility::PathDataNode;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Graphics {
namespace Drawable {

class VectorDrawable
    : public Drawable
    , public IVectorDrawable
{
public:
    class VPathRenderer;

protected:
    class VectorDrawableState
        : public ConstantState
    {
    public:
        TO_STRING_IMPL("VectorDrawableState")

        VectorDrawableState();

        // Deep copy for mutate() or implicitly mutate.
        VectorDrawableState(
            /* [in] */ VectorDrawableState* copy);

        CARAPI DrawCachedBitmapWithRootAlpha(
            /* [in] */ ICanvas* canvas,
            /* [in] */ IColorFilter* filter);

        CARAPI_(Boolean) HasTranslucentRoot();

        /**
         * @return null when there is no need for alpha paint.
         */
        CARAPI_(AutoPtr<IPaint>) GetPaint(
            /* [in] */ IColorFilter* filter);

        CARAPI_(void) UpdateCachedBitmap(
            /* [in] */ IRect* bounds);

        CARAPI_(void) CreateCachedBitmapIfNeeded(
            /* [in] */ IRect* bounds);

        CARAPI_(Boolean) CanReuseBitmap(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        CARAPI_(Boolean) CanReuseCache();

        CARAPI_(void) UpdateCacheStates();

        // @Override
        CARAPI CanApplyTheme(
            /* [out] */ Boolean* can);

        // @Override
        CARAPI NewDrawable(
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI NewDrawable(
            /* [in] */ IResources* res,
            /* [in] */ IResourcesTheme* theme,
            /* [out] */ IDrawable** drawable);

        // @Override
        CARAPI GetChangingConfigurations(
            /* [out] */ Int32* config);

    private:
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        Int32 mChangingConfigurations;
        AutoPtr<VPathRenderer> mVPathRenderer;
        AutoPtr<IColorStateList> mTint;
        PorterDuffMode mTintMode;
        Boolean mAutoMirrored;

        AutoPtr<IBitmap> mCachedBitmap;
        AutoPtr<ArrayOf<Int32> > mCachedThemeAttrs;
        AutoPtr<IColorStateList> mCachedTint;
        PorterDuffMode mCachedTintMode;
        Int32 mCachedRootAlpha;
        Boolean mCachedAutoMirrored;
        Boolean mCacheDirty;

        /** Temporary paint object used to draw cached bitmaps. */
        AutoPtr<IPaint> mTempPaint;

        friend class VectorDrawable;
    };

public:
    class VGroup;
    class VPath;

    class VPathRenderer
        : public Object
    {
    public:
        VPathRenderer();

        VPathRenderer(
            /* [in] */ VPathRenderer* copy);

        CARAPI_(void) SetRootAlpha(
            /* [in] */ Int32 alpha);

        CARAPI_(Int32) GetRootAlpha();

        // setAlpha() and getAlpha() are used mostly for animation purpose, since
        // Animator like to use alpha from 0 to 1.
        CARAPI_(void) SetAlpha(
            /* [in] */ Float alpha);

        // @SuppressWarnings("unused")
        CARAPI_(Float) GetAlpha();

        CARAPI_(Boolean) CanApplyTheme();

        CARAPI_(void) ApplyTheme(
            /* [in] */ IResourcesTheme* t);

        CARAPI_(void) Draw(
            /* [in] */ ICanvas* canvas,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ IColorFilter* filter);

        TO_STRING_IMPL("VectorDrawable::VPathRenderer");

    private:
        CARAPI_(Boolean) RecursiveCanApplyTheme(
            /* [in] */ VGroup* currentGroup);

        CARAPI_(void) RecursiveApplyTheme(
            /* [in] */ VGroup* currentGroup,
            /* [in] */ IResourcesTheme* t);

        CARAPI_(void) DrawGroupTree(
            /* [in] */ VGroup* currentGroup,
            /* [in] */ IMatrix* currentMatrix,
            /* [in] */ ICanvas* canvas,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ IColorFilter* filter);

        CARAPI_(void) DrawPath(
            /* [in] */ VGroup* vGroup,
            /* [in] */ VPath* vPath,
            /* [in] */ ICanvas* canvas,
            /* [in] */ Int32 w,
            /* [in] */ Int32 h,
            /* [in] */ IColorFilter* filter);

    public:
        /* Right now the internal data structure is organized as a tree.
         * Each node can be a group node, or a path.
         * A group node can have groups or paths as children, but a path node has
         * no children.
         * One example can be:
         *                 Root Group
         *                /    |     \
         *           Group    Path    Group
         *          /     \             |
         *         Path   Path         Path
         *
         */
        // Variables that only used temporarily inside the draw() call, so there
        // is no need for deep copying.
        AutoPtr<IPath> mPath;
        AutoPtr<IPath> mRenderPath;
        static AutoPtr<IMatrix> IDENTITY_MATRIX;
        AutoPtr<IMatrix> mFinalPathMatrix;

        AutoPtr<IPaint> mStrokePaint;
        AutoPtr<IPaint> mFillPaint;
        AutoPtr<IPathMeasure> mPathMeasure;

        /////////////////////////////////////////////////////
        // Variables below need to be copied (deep copy if applicable) for mutation.
        Int32 mChangingConfigurations;
        AutoPtr<VGroup> mRootGroup;
        Float mBaseWidth;
        Float mBaseHeight;
        Float mViewportWidth;
        Float mViewportHeight;
        Int32 mRootAlpha;
        String mRootName;

        // final ArrayMap<String, Object> mVGTargetsMap = new ArrayMap<String, Object>();
        AutoPtr<IArrayMap> mVGTargetsMap;
    };

    class VGroup
        : public Object
        , public IVGroup
    {
    public:
        CAR_INTERFACE_DECL()

        //VGroup();

        //VGroup(
        //    /* [in] */ VGroup* copy,
        //    /* [in] */ IArrayMap/*<String, Object>*/* targetsMap);

        CARAPI constructor(
            /* [in] */ IVGroup* copy,
            /* [in] */ IArrayMap/*<String, Object>*/* targetsMap);

        CARAPI constructor();

        CARAPI_(String) GetGroupName();

        CARAPI_(AutoPtr<IMatrix>) GetLocalMatrix();

        CARAPI_(void) Inflate(
            /* [in] */ IResources* res,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ IResourcesTheme* theme);

        CARAPI_(Boolean) CanApplyTheme();

        CARAPI_(void) ApplyTheme(
            /* [in] */ IResourcesTheme* t);

        /* Setters and Getters, used by animator from AnimatedVectorDrawable. */
        // @SuppressWarnings("unused")
        CARAPI GetRotation(
            /* [out] */ Float* rotation);

        // @SuppressWarnings("unused")
        CARAPI SetRotation(
            /* [in] */ Float rotation);

        // @SuppressWarnings("unused")
        CARAPI GetPivotX(
            /* [out] */ Float* pivotX);

        // @SuppressWarnings("unused")
        CARAPI SetPivotX(
            /* [in] */ Float pivotX);

        // @SuppressWarnings("unused")
        CARAPI GetPivotY(
            /* [out] */ Float* pivotY);

        // @SuppressWarnings("unused")
        CARAPI SetPivotY(
            /* [in] */ Float pivotY);

        // @SuppressWarnings("unused")
        CARAPI GetScaleX(
            /* [out] */ Float* scaleX);

        // @SuppressWarnings("unused")
        CARAPI SetScaleX(
            /* [in] */ Float scaleX);

        // @SuppressWarnings("unused")
        CARAPI GetScaleY(
            /* [out] */ Float* scaleY);

        // @SuppressWarnings("unused")
        CARAPI SetScaleY(
            /* [in] */ Float scaleY);

        // @SuppressWarnings("unused")
        CARAPI GetTranslateX(
            /* [out] */ Float* translateX);

        // @SuppressWarnings("unused")
        CARAPI SetTranslateX(
            /* [in] */ Float translateX);

        // @SuppressWarnings("unused")
        CARAPI GetTranslateY(
            /* [out] */ Float* translateY);

        // @SuppressWarnings("unused")
        CARAPI SetTranslateY(
            /* [in] */ Float translateY);

    private:
        CARAPI_(void) UpdateStateFromTypedArray(
            /* [in] */ ITypedArray* a);

        CARAPI_(void) UpdateLocalMatrix();

    public:
        // mStackedMatrix is only used temporarily when drawing, it combines all
        // the parents' local matrices with the current one.
        AutoPtr<IMatrix> mStackedMatrix;

        /////////////////////////////////////////////////////
        // Variables below need to be copied (deep copy if applicable) for mutation.
        AutoPtr<IArrayList>/*<Object>*/ mChildren;

        Float mRotate;
        Float mPivotX;
        Float mPivotY;
        Float mScaleX;
        Float mScaleY;
        Float mTranslateX;
        Float mTranslateY;

        // mLocalMatrix is updated based on the update of transformation information,
        // either parsed from the XML or by animation.
        AutoPtr<IMatrix> mLocalMatrix;
        Int32 mChangingConfigurations;
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;
        String mGroupName;
    };

    /**
     * Common Path information for clip path and normal path.
     */
    class VPath
        : public Object
        , public IVPath
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("VectorDrawable::VPath");

        VPath();

        CARAPI constructor(
            /* [in] */ VPath* copy);

        virtual CARAPI_(void) ToPath(
            /* [in] */ IPath* path);

        virtual CARAPI_(String) GetPathName();

        virtual CARAPI_(Boolean) CanApplyTheme();

        virtual CARAPI_(void) ApplyTheme(
            /* [in] */ IResourcesTheme* t);

        virtual CARAPI_(Boolean) IsClipPath();

        /* Setters and Getters, used by animator from AnimatedVectorDrawable. */
        // @SuppressWarnings("unused")
        virtual CARAPI_(AutoPtr<ArrayOf<PathDataNode*> >) GetPathData();

        // @SuppressWarnings("unused")
        virtual CARAPI_(void) SetPathData(
            /* [in] */ ArrayOf<PathDataNode*>* nodes);

    public:
        AutoPtr<ArrayOf<PathDataNode*> > mNodes;
        String mPathName;
        Int32 mChangingConfigurations;
    };

    /**
     * Clip path, which only has name and pathData.
     */
    class VClipPath
        : public VPath
        , public IVClipPath
    {
    public:
        CAR_INTERFACE_DECL()

        TO_STRING_IMPL("VectorDrawable::VClipPath");

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IVClipPath* copy);

        CARAPI_(void) Inflate(
            /* [in] */ IResources* r,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ IResourcesTheme* theme);

        // @Override
        CARAPI_(Boolean) IsClipPath();

    private:
        CARAPI_(void) UpdateStateFromTypedArray(
            /* [in] */ ITypedArray* a);
    };

    /**
     * Normal path, which contains all the fill / paint information.
     */
    class VFullPath
        : public VPath
        , public IVFullPath
    {
    public:
        CAR_INTERFACE_DECL()

        VFullPath();

        CARAPI constructor();

        CARAPI constructor(
            /* [in] */ IVFullPath* copy);

        // @Override
        CARAPI_(Boolean) CanApplyTheme();

        CARAPI_(void) Inflate(
            /* [in] */ IResources* r,
            /* [in] */ IAttributeSet* attrs,
            /* [in] */ IResourcesTheme* theme);

        // @Override
        CARAPI_(void) ApplyTheme(
            /* [in] */ IResourcesTheme* t);

        /* Setters and Getters, used by animator from AnimatedVectorDrawable. */
        // @SuppressWarnings("unused")
        CARAPI GetStrokeColor(
            /* [out] */ Int32* strokeColor);

        // @SuppressWarnings("unused")
        CARAPI SetStrokeColor(
            /* [in] */ Int32 strokeColor);

        // @SuppressWarnings("unused")
        CARAPI GetStrokeWidth(
            /* [out] */ Float* strokeWidth);

        // @SuppressWarnings("unused")
        CARAPI SetStrokeWidth(
            /* [in] */ Float strokeWidth);

        // @SuppressWarnings("unused")
        CARAPI GetStrokeAlpha(
            /* [out] */ Float* strokeAlpha);

        // @SuppressWarnings("unused")
        CARAPI SetStrokeAlpha(
            /* [in] */ Float strokeAlpha);

        // @SuppressWarnings("unused")
        CARAPI GetFillColor(
            /* [out] */ Int32* fillColor);

        // @SuppressWarnings("unused")
        CARAPI SetFillColor(
            /* [in] */ Int32 fillColor);

        // @SuppressWarnings("unused")
        CARAPI GetFillAlpha(
            /* [out] */ Float* fillAlpha);

        // @SuppressWarnings("unused")
        CARAPI SetFillAlpha(
            /* [in] */ Float fillAlpha);

        // @SuppressWarnings("unused")
        CARAPI GetTrimPathStart(
            /* [out] */ Float* trimPathStart);

        // @SuppressWarnings("unused")
        CARAPI SetTrimPathStart(
            /* [in] */ Float trimPathStart);

        // @SuppressWarnings("unused")
        CARAPI GetTrimPathEnd(
            /* [out] */ Float* trimPathEnd);

        // @SuppressWarnings("unused")
        CARAPI SetTrimPathEnd(
            /* [in] */ Float trimPathEnd);

        // @SuppressWarnings("unused")
        CARAPI GetTrimPathOffset(
            /* [out] */ Float* trimPathOffset);

        // @SuppressWarnings("unused")
        CARAPI SetTrimPathOffset(
            /* [in] */ Float trimPathOffset);

        TO_STRING_IMPL("VectorDrawable::VFullPath");

    private:
        CARAPI_(PaintCap) GetStrokeLineCap(
            /* [in] */ Int32 id,
            /* [in] */ PaintCap defValue);

        CARAPI_(PaintJoin) GetStrokeLineJoin(
            /* [in] */ Int32 id,
            /* [in] */ PaintJoin defValue);

        CARAPI_(void) UpdateStateFromTypedArray(
            /* [in] */ ITypedArray* a);

    public:
        /////////////////////////////////////////////////////
        // Variables below need to be copied (deep copy if applicable) for mutation.
        AutoPtr<ArrayOf<Int32> > mThemeAttrs;

        Int32 mStrokeColor;
        Float mStrokeWidth;

        Int32 mFillColor;
        Float mStrokeAlpha;
        Int32 mFillRule;
        Float mFillAlpha;
        Float mTrimPathStart;
        Float mTrimPathEnd;
        Float mTrimPathOffset;

        PaintCap mStrokeLineCap;
        PaintJoin mStrokeLineJoin;
        Float mStrokeMiterlimit;
    };


public:
    CAR_INTERFACE_DECL()

    VectorDrawable();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IDrawableConstantState* state,
        /* [in] */ IResources* res,
        /* [in] */ IResourcesTheme* theme);

    // @Override
    CARAPI Mutate();

    CARAPI_(AutoPtr<IInterface>) GetTargetByName(
        /* [in] */ const String& name);

    // @Override
    CARAPI GetConstantState(
        /* [out] */ IDrawableConstantState** state);

    // @Override
    CARAPI Draw(
        /* [in] */ ICanvas* canvas);

    // @Override
    CARAPI GetAlpha(
        /* [out] */ Int32* alpha);

    // @Override
    CARAPI SetAlpha(
        /* [in] */ Int32 alpha);

    // @Override
    CARAPI SetColorFilter(
        /* [in] */ IColorFilter* colorFilter);

    // @Override
    CARAPI SetTintList(
        /* [in] */ IColorStateList* tint);

    // @Override
    CARAPI SetTintMode(
        /* [in] */ PorterDuffMode tintMode);

    // @Override
    CARAPI IsStateful(
        /* [out] */ Boolean* isStateful);

    // @Override
    CARAPI GetOpacity(
        /* [out] */ Int32* opacity);

    // @Override
    CARAPI GetIntrinsicWidth(
        /* [out] */ Int32* width);

    // @Override
    CARAPI GetIntrinsicHeight(
        /* [out] */ Int32* height);

    // @Override
    CARAPI CanApplyTheme(
        /* [out] */ Boolean* can);

    // @Override
    CARAPI ApplyTheme(
        /* [out] */ IResourcesTheme* t);

    /**
     * The size of a pixel when scaled from the intrinsic dimension to the viewport dimension.
     * This is used to calculate the path animation accuracy.
     *
     * @hide
     */
    CARAPI GetPixelSize(
        /* [out] */ Float* size);

    /** @hide */
    static CARAPI_(AutoPtr<IVectorDrawable>) Create(
        /* [in] */ IResources* resources,
        /* [in] */ Int32 rid);

    // @Override
    CARAPI Inflate(
        /* [in] */ IResources* res,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    // @Override
    CARAPI GetChangingConfigurations(
        /* [out] */ Int32* config);

    CARAPI_(void) SetAllowCaching(
        /* [in] */ Boolean allowCaching);

    // @Override
    CARAPI SetAutoMirrored(
        /* [in] */ Boolean mirrored);

    // @Override
    CARAPI IsAutoMirrored(
        /* [out] */ Boolean* mirrored);

protected:
    // @Override
    CARAPI_(Boolean) OnStateChange(
        /* [in] */ ArrayOf<Int32>* stateSet);


private:
    static CARAPI_(Int32) ApplyAlpha(
        /* [in] */ Int32 color,
        /* [in] */ Float alpha);

    CARAPI UpdateStateFromTypedArray(
        /* [in] */ ITypedArray* a) /*throws XmlPullParserException*/;

    CARAPI InflateInternal(
        /* [in] */ IResources* res,
        /* [in] */ IXmlPullParser* parser,
        /* [in] */ IAttributeSet* attrs,
        /* [in] */ IResourcesTheme* theme) /*throws XmlPullParserException, IOException*/;

    CARAPI_(void) PrintGroupTree(
        /* [in] */ VGroup* currentGroup,
        /* [in] */ Int32 level);

    CARAPI_(Boolean) NeedMirroring();

private:
    static const String LOGTAG;
    static const String SHAPE_CLIP_PATH;
    static const String SHAPE_GROUP;
    static const String SHAPE_PATH;
    static const String SHAPE_VECTOR;
    static const Int32 LINECAP_BUTT;
    static const Int32 LINECAP_ROUND;
    static const Int32 LINECAP_SQUARE;
    static const Int32 LINEJOIN_MITER;
    static const Int32 LINEJOIN_ROUND;
    static const Int32 LINEJOIN_BEVEL;
    static const Boolean DBG_VECTOR_DRAWABLE;

    AutoPtr<VectorDrawableState> mVectorState;
    AutoPtr<IPorterDuffColorFilter> mTintFilter;
    AutoPtr<IColorFilter> mColorFilter;
    Boolean mMutated;
    // AnimatedVectorDrawable needs to turn off the cache all the time, otherwise,
    // caching the bitmap by default is allowed.
    Boolean mAllowCaching;
    friend class VFullPath;
};

} // namespace Drawable
} // namespace Graphics
} // namespace Droid
} // namespace Elastos


#endif // __ELASTOS_DROID_GRAPHICS_DRAWABLE_VECTORDRAWABLE_H__
