#ifndef  __ELASTOS_DROID_LAUNCHER2_FOLDERICON_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOLDERICON_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class FolderIcon
    : public LinearLayout
    , public IFolderListener
{
public:
    class FolderRingAnimator
        : public Object
    {
    public:
        FolderRingAnimator(
            /* [in] */ ILauncher* launcher,
            /* [in] */ FolderIcon* folderIcon);

        CARAPI AnimateToAcceptState();

        CARAPI AnimateToNaturalState();

        // Location is expressed in window coordinates
        CARAPI GetCell(
            /* [in] */ ArrayOf<Int32>* loc);

        // Location is expressed in window coordinates
        CARAPI SetCell(
            /* [in] */ Int32 x,
            /* [in] */ Int32 y);

        CARAPI SetCellLayout(
            /* [in] */ ICellLayout* layout);

        CARAPI GetOuterRingSize(
            /* [out] */ Float* size);

        CARAPI GetInnerRingSize(
            /* [out] */ Float* size);

    public:
        Int32 mCellX;
        Int32 mCellY;

        Float mOuterRingSize;
        Float mInnerRingSize;
        AutoPtr<FolderIcon> mFolderIcon;
        AutoPtr<IDrawable> mOuterRingDrawable;
        AutoPtr<IDrawable> mInnerRingDrawable;
        static AutoPtr<IDrawable> sSharedOuterRingDrawable;
        static AutoPtr<IDrawable> sSharedInnerRingDrawable;
        static Int32 sPreviewSize;
        static Int32 sPreviewPadding;

    private:
        AutoPtr<ICellLayout> mCellLayout;
        AutoPtr<IValueAnimator> mAcceptAnimator;
        AutoPtr<IValueAnimator> mNeutralAnimator;
    };

    class PreviewItemDrawingParams
        : public Object
    {
    public:
        PreviewItemDrawingParams(
            /* [in] */ Float transX,
            /* [in] */ Float transY,
            /* [in] */ Float scale,
            /* [in] */ Int32 overlayAlpha);

    private:
        Float mTransX;
        Float mTransY;
        Float mScale;
        Int32 mOverlayAlpha;
        AutoPtr<IDrawable> mDrawable;
    };

private:
    class MyAnimatorUpdateListener
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener(
            /* [in] */ FolderIcon::FolderRingAnimator* host,
            /* [in] */ Int32 previewSize);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<FolderIcon::FolderRingAnimator> mHost;
        Int32 mPreviewSize;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ FolderIcon::FolderRingAnimator* host);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<FolderIcon::FolderRingAnimator> mHost;
    };

    class MyAnimatorUpdateListener2
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener2(
            /* [in] */ FolderIcon::FolderRingAnimator* host,
            /* [in] */ Int32 previewSize);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<FolderIcon::FolderRingAnimator> mHost;
        Int32 mPreviewSize;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ FolderIcon::FolderRingAnimator* host);

        CARAPI onAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<FolderIcon::FolderRingAnimator> mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ FolderIcon* host,
            /* [in] */  IShortcutInfo* item);

        CARAPI Run();

    private:
        AutoPtr<FolderIcon> mHost;
        AutoPtr<IShortcutInfo> mItem;
    };

    class MyAnimatorUpdateListener3
        : public Object
        , public IAnimatorUpdateListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyAnimatorUpdateListener3(
            /* [in] */ FolderIcon* host,
            /* [in] */ Boolean reverse,
            /* [in] */ Float transX0,
            /* [in] */ Float transY0,
            /* [in] */ Float scale0,
            /* [in] */ PreviewItemDrawingParams* finalParams);

        CARAPI OnAnimationUpdate(
            /* [in] */ IValueAnimator* animation);

    private:
        AutoPtr<FolderIcon> mHost;
        Boolean mReverse;
        Float mTransX0;
        Float mTransY0;
        Float mScale0;
        AutoPtr<PreviewItemDrawingParams> mFinalParams;
    };

    class MyAnimatorListenerAdapter3
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter3(
            /* [in] */ FolderIcon* host,
            /* [in] */ IRunnable* onCompleteRunnable);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        AutoPtr<FolderIcon> mHost;
        AutoPtr<IRunnable> mOnCompleteRunnable;
    };

public:
    CAR_INTERFACE_DECL();

    FolderIcon();

    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI constructor(
        /* [in] */ IContext* context);

    CARAPI IsDropEnabled(
        /* [out] */ Boolean* result);

    static CARAPI FromXml(
        /* [in] */ Int32 resId,
        /* [in] */ ILauncher* launcher,
        /* [in] */ IViewGroup* group,
        /* [in] */ IFolderInfo* folderInfo,
        /* [in] */ IIconCache* iconCache,
        /* [out] */ IFolderIcon** icon);

    CARAPI GetFolder(
        /* [out] */ IFolder** folder);

    CARAPI GetFolderInfo(
        /* [out] */ IFolderInfo** info);

    CARAPI AcceptDrop(
        /* [in] */ IInterface* dragInfo,
        /* [out] */ Boolean* result);

    CARAPI AddItem(
        /* [in] */ IShortcutInfo* item);

    CARAPI OnDragEnter(
        /* [in] */ IInterface* dragInfo);

    CARAPI OnDragOver(
        /* [in] */ IInterface* dragInfo);

    CARAPI PerformCreateAnimation(
        /* [in] */ IShortcutInfo* destInfo,
        /* [in] */ IView* destView,
        /* [in] */ IShortcutInfo* srcInfo,
        /* [in] */ IDragView* srcView,
        /* [in] */ IRect* dstRect,
        /* [in] */ Float scaleRelativeToDragLayer,
        /* [in] */ IRunnable* postAnimationRunnable);

    CARAPI PerformDestroyAnimation(
        /* [in] */ IView* finalView,
        /* [in] */ IRunnable* onCompleteRunnable);

    CARAPI OnDragExit(
        /* [in] */ IInterface* dragInfo);

    CARAPI OnDragExit();

    CARAPI OnDrop(
        /* [in] */ IDragObject* d);

    CARAPI GetDropTargetDelegate(
        /* [in] */ IDragObject* d,
        /* [out] */ IDropTarget** target);

    CARAPI SetTextVisible(
        /* [in] */ Boolean visible);

    CARAPI GetTextVisible(
        /* [out] */ Boolean* result);

    CARAPI OnItemsChanged();

    CARAPI OnAdd(
        /* [in] */ IShortcutInfo* item);

    CARAPI OnRemove(
        /* [in] */ IShortcutInfo* item);

    CARAPI OnTitleChanged(
        /* [in] */ ICharSequence* title);

    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI CancelLongPress();

protected:
    //@Override
    CARAPI OnSaveInstanceState(
        /* [out] */ IParcelable** p);

    //@Override
    CARAPI DispatchDraw(
        /* [in] */ ICanvas* canvas);

private:
    CARAPI_(void) Init();

    CARAPI_(Boolean) WillAcceptItem(
        /* [in] */ IItemInfo* item);

    CARAPI_(void) OnDrop(
        /* [in] */ IShortcutInfo* item,
        /* [in] */ IDragView* animateView,
        /* [in] */ IRect* finalRect,
        /* [in] */ Float scaleRelativeToDragLayer,
        /* [in] */ Int32 index,
        /* [in] */ IRunnable* postAnimationRunnable,
        /* [in] */ IDragObject* d);

    CARAPI_(void) ComputePreviewDrawingParams(
        /* [in] */ Int32 drawableSize,
        /* [in] */ Int32 totalSize);

    CARAPI_(void) ComputePreviewDrawingParams(
        /* [in] */ IDrawable* d);

    CARAPI_(Float) GetLocalCenterForIndex(
        /* [in] */ Int32 index,
        /* [in] */ ArrayOf<Int32>* center);

    CARAPI_(AutoPtr<PreviewItemDrawingParams>) ComputePreviewItemDrawingParams(
        /* [in] */ Int32 index,
        /* [in] */ PreviewItemDrawingParams* params);

    CARAPI_(void) DrawPreviewItem(
        /* [in] */ ICanvas* canvas,
        /* [in] */ PreviewItemDrawingParams* params);

    CARAPI_(void) AnimateFirstItem(
        /* [in] */ IDrawable* d,
        /* [in] */ Int32 duration,
        /* [in] */ Boolean reverse,
        /* [in] */ IRunnable* onCompleteRunnable);

public:
    static AutoPtr<IDrawable> sSharedFolderLeaveBehind;

private:
    AutoPtr<ILauncher> mLauncher;
    AutoPtr<IFolder> mFolder;
    AutoPtr<IFolderInfo> mInfo;
    static Boolean sStaticValuesDirty;

    AutoPtr<CheckLongPressHelper> mLongPressHelper;

    // The number of icons to display in the
    static const Int32 NUM_ITEMS_IN_PREVIEW;
    static const Int32 CONSUMPTION_ANIMATION_DURATION;
    static const Int32 DROP_IN_ANIMATION_DURATION;
    static const Int32 INITIAL_ITEM_ANIMATION_DURATION;
    static const Int32 FINAL_ITEM_ANIMATION_DURATION;

    // The degree to which the inner ring grows when accepting drop
    static const Float INNER_RING_GROWTH_FACTOR;

    // The degree to which the outer ring is scaled in its natural state
    static const Float OUTER_RING_GROWTH_FACTOR;

    // The amount of vertical spread between items in the stack [0...1]
    static const Float PERSPECTIVE_SHIFT_FACTOR;

    // The degree to which the item in the back of the stack is scaled [0...1]
    // (0 means it's not scaled at all, 1 means it's scaled to nothing)
    static const Float PERSPECTIVE_SCALE_FACTOR;

    AutoPtr<IImageView> mPreviewBackground;
    AutoPtr<IBubbleTextView> mFolderName;

    AutoPtr<IFolderRingAnimator> mFolderRingAnimator;

    // These variables are all associated with the drawing of the preview; they are stored
    // as member variables for shared usage and to avoid computation on each frame
    Int32 mIntrinsicIconSize;
    Float mBaselineIconScale;
    Int32 mBaselineIconSize;
    Int32 mAvailableSpaceInPreview;
    Int32 mTotalWidth = -1;
    Int32 mPreviewOffsetX;
    Int32 mPreviewOffsetY;
    Float mMaxPerspectiveShift;
    Boolean mAnimating;

    AutoPtr<PreviewItemDrawingParams> mParams;
    AutoPtr<PreviewItemDrawingParams> mAnimParams;
    AutoPtr<IArrayList> mHiddenItems;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOLDERICON_H__