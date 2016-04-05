#ifndef  __ELASTOS_DROID_LAUNCHER2_FOLDER_H__
#define  __ELASTOS_DROID_LAUNCHER2_FOLDER_H__

#include "elastos/droid/ext/frameworkext.h"

namespace Elastos {
namespace Droid {
namespace Launcher2 {

class Folder
    : public LinearLayout
    , public IFolder
    , public IDragSource
    , public IViewOnClickListener
    , public IViewOnLongClickListener
    , public IDropTarget
    , public IFolderInfoFolderListener
    , public ITextViewOnEditorActionListener
    , public IViewOnFocusChangeListener
{
private:
    class  MyActionModeCallback
        : public Object
        , public IActionModeCallback
    {
    public:
        CAR_INTERFACE_DECL();

        CARAPI OnCreateActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* res);

        CARAPI OnPrepareActionMode(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenu* menu,
            /* [out] */ Boolean* res);

        CARAPI OnActionItemClicked(
            /* [in] */ IActionMode* mode,
            /* [in] */ IMenuItem* item,
            /* [out] */ Boolean* res);

        CARAPI OnDestroyActionMode(
            /* [in] */ IActionMode* mode);
    };

    class GridComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL();

        GridComparator(
            /* [in] */ Int32 numCols);

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* lhs,
            /* [in] */ IInterface* rhs,
            /* [out] */ Int32* result);

    public:
        Int32 mNumCols;
    };

    class MyAnimatorListenerAdapter
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter(
            /* [in] */ Folder* host);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

    private:
        Folder* mHost;
    };

    class MyAnimatorListenerAdapter2
        : public AnimatorListenerAdapter
    {
    public:
        MyAnimatorListenerAdapter2(
            /* [in] */ Folder* host);

        CARAPI OnAnimationEnd(
            /* [in] */ IAnimator* animation);

        CARAPI OnAnimationStart(
            /* [in] */ IAnimator* animation);

    private:
        Folder* mHost;
    };

    class MyOnAlarmListener
        : public Object
        , public IOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnAlarmListener(
            /* [in] */ Folder* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    private:
        Folder* mHost;
    };

    class MyOnAlarmListener2
        : public Object
        , public IOnAlarmListener
    {
    public:
        CAR_INTERFACE_DECL();

        MyOnAlarmListener2(
            /* [in] */ Folder* host);

        CARAPI OnAlarm(
            /* [in] */ IAlarm* alarm);

    private:
        Folder* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ Folder* host);

        CARAPI Run();

    private:
        Folder* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    Folder();

    /**
     * Used to inflate the Workspace from XML.
     *
     * @param context The application's context.
     * @param attrs The attribtues set containing the Workspace's customization values.
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IAttributeSet* attrs);

    CARAPI OnClick(
        /* [in] */ IView* v);

    CARAPI OnLongClick(
        /* [in] */ IView* v,
        /* [out] */ Boolean* result);

    CARAPI IsEditingName(
        /* [out] */ Boolean* result);

    CARAPI StartEditingFolderName();

    CARAPI DismissEditingName();

    CARAPI DoneEditingFolderName(
        /* [in] */ Boolean commit);

    CARAPI OnEditorAction(
        /* [in] */ ITextView* v,
        /* [in] */ Int32 actionId,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    CARAPI GetEditTextRegion(
        /* [out] */ IView** view);

    CARAPI GetDragDrawable(
        /* [out] */ IDrawable** drawable);

    /**
     * We need to handle touch events to prevent them from falling through to the workspace below.
     */
    //@Override
    CARAPI OnTouchEvent(
        /* [in] */ IMotionEvent* ev,);
        /* [out] */ Boolean* result

    CARAPI SetDragController(
        /* [in] */ IDragController* dragController);

    CARAPI SetFolderIcon(
        /* [in] */ IFolderIcon* icon);

    //@Override
    CARAPI DispatchPopulateAccessibilityEvent(
        /* [in] */ IAccessibilityEvent* event,
        /* [out] */ Boolean* result);

    /**
     * @return the FolderInfo object associated with this folder
     */
    CARAPI GetInfo(
        /* [out] */ IFolderInfo** info);

    CARAPI Bind(
        /* [in] */ IFolderInfo* info);

    /**
     * Creates a new UserFolder, inflated from R.layout.user_folder.
     *
     * @param context The application's context.
     *
     * @return A new UserFolder.
     */
    static CARAPI FromXml(
        /* [in] */ IContext* context,
        /* [out] */ IFolder** folder);

    CARAPI AnimateOpen();

    CARAPI AnimateClosed();

    CARAPI NotifyDataSetChanged();

    CARAPI AcceptDrop(
        /* [in] */ DragObject* d,
        /* [out] */ Boolean* result);

    CARAPI OnDragEnter(
        /* [in] */ DragObject* d);

    CARAPI ReadingOrderGreaterThan(
        /* [in] */ ArrayOf<Int32>* v1,
        /* [in] */ ArrayOf<Int32>* v2,
        /* [out] */ Boolean* result);

    CARAPI IsLayoutRtl(
        /* [out] */ Boolean* result);

    CARAPI OnDragOver(
        /* [in] */ DragObject* d);

    CARAPI CompleteDragExit();

    CARAPI OnDragExit(
        /* [in] */ DragObject* d);

    CARAPI OnDropCompleted(
        /* [in] */ IView* target,
        /* [in] */ DragObject* d,
        /* [in] */ Boolean isFlingToDelete,
        /* [in] */ Boolean success);

    //@Override
    CARAPI SupportsFlingToDelete(
        /* [out] */ Boolean* result);

    CARAPI OnFlingToDelete(
        /* [in] */ DragObject* d,
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ IPointF* vec);

    //@Override
    CARAPI OnFlingToDeleteCompleted();

    CARAPI NotifyDrop();

    CARAPI IsDropEnabled(
        /* [out] */ Boolean* result);

    CARAPI GetDropTargetDelegate(
        /* [in] */ DragObject* d,
        /* [out] */ IDropTarget** target);

    CARAPI IsFull(
        /* [out] */ Boolean* result);

    CARAPI GetPivotXForIconAnimation(
        /* [out] */ Float* res);

    CARAPI GetPivotYForIconAnimation(
        /* [out] */ Float* res);

    CARAPI GetItemCount(
        /* [out] */ Int32* count);

    CARAPI GetItemAt(
        /* [in] */ Int32 index,
        /* [out] */ IView** view);

    CARAPI IsDestroyed(
        /* [out] */ Boolean* result);

    CARAPI OnDrop(
        /* [in] */ DragObject* d);

    // This is used so the item doesn't immediately appear in the folder when added. In one case
    // we need to create the illusion that the item isn't added back to the folder yet, to
    // to correspond to the animation of the icon back into the folder. This is
    CARAPI HideItem(
        /* [in] */ IShortcutInfo* info);

    CARAPI ShowItem(
        /* [in] */ IShortcutInfo* info);

    CARAPI OnAdd(
        /* [in] */ IShortcutInfo* item);

    CARAPI OnRemove(
        /* [in] */ IShortcutInfo* item);

    CARAPI OnItemsChanged();

    CARAPI OnTitleChanged(
        /* [in] */ ICharSequence* title);

    CARAPI GetItemsInReadingOrder(
        /* [out] */ IArrayList** list);

    CARAPI GetLocationInDragLayer(
        /* [in] */ ArrayOf<Int32>* loc);

    CARAPI OnFocusChange(
        /* [in] */ IView* v,
        /* [in] */ Boolean hasFocus);

protected:
    //@Override
    CARAPI OnFinishInflate();

    CARAPI FindAndSetEmptyCells(
        /* [in] */ IShortcutInfo* item,
        /* [out] */ Boolean* result);

    CARAPI CreateAndAddShortcut(
        /* [in] */ IShortcutInfo* item,
        /* [out] */ Boolean* result);

    CARAPI OnMeasure(
        /* [in] */ Int32 widthMeasureSpec,
        /* [in] */ Int32 heightMeasureSpec);

private:
    CARAPI_(void) PlaceInReadingOrder(
        /* [in] */ IArrayList* items);

    /**
     * This method is intended to make the UserFolder to be visually identical in size and position
     * to its associated FolderIcon. This allows for a seamless transition into the expanded state.
     */
    CARAPI_(void) PositionAndSizeAsIcon();

    CARAPI_(void) SendCustomAccessibilityEvent(
        /* [in] */ Int32 type,
        /* [in] */ const String& text);

    CARAPI_(void) SetFocusOnFirstChild();

    CARAPI_(void) RealTimeReorder(
        /* [in] */ ArrayOf<Int32>* empty,
        /* [in] */ ArrayOf<Int32>* target);

    // This is used to compute the visual center of the dragView. The idea is that
    // the visual center represents the user's interpretation of where the item is, and hence
    // is the appropriate point to use when determining drop location.
    CARAPI_(AutoPtr<ArrayOf<Float> >) GetDragViewVisualCenter(
        /* [in] */ Int32 x,
        /* [in] */ Int32 y,
        /* [in] */ Int32 xOffset,
        /* [in] */ Int32 yOffset,
        /* [in] */ IDragView* dragView,
        /* [in] */ ArrayOf<Float>* recycle);

    CARAPI_(void) UpdateItemLocationsInDatabase();

    CARAPI_(void) SetupContentDimensions(
        /* [in] */ Int32 count);

    CARAPI_(void) CenterAboutIcon();

    CARAPI_(void) SetupContentForNumItems(
        /* [in] */ Int32 count);

    CARAPI_(void) ArrangeChildren(
        /* [in] */ IArrayList* list);

    CARAPI_(void) OnCloseComplete();

    CARAPI_(void) ReplaceFolderWithFinalItem();

    // This method keeps track of the last item in the folder for the purposes
    // of keyboard focus
    CARAPI_(void) UpdateTextViewFocus();

    CARAPI_(AutoPtr<IView>) GetViewForInfo(
        /* [in] */ IShortcutInfo* item);

public:
    Boolean mItemsInvalidated;
    Boolean mSuppressOnAdd;
    AutoPtr<IFolderEditText> mFolderName;

    AutoPtr<IOnAlarmListener> mReorderAlarmListener;
    AutoPtr<IOnAlarmListener> mOnExitAlarmListener;

protected:
    AutoPtr<IDragController> mDragController;
    AutoPtr<ILauncher> mLauncher;
    AutoPtr<IFolderInfo> mInfo;
    AutoPtr<ICellLayout> mContent;

private:
    static const String TAG;
    Int32 mExpandDuration;
    AutoPtr<ILayoutInflater> mInflater;
    AutoPtr<IIconCache> mIconCache;
    Int32 mState;
    static const Int32 REORDER_ANIMATION_DURATION;
    static const Int32 ON_EXIT_CLOSE_DELAY;
    Boolean mRearrangeOnClose;
    AutoPtr<IFolderIcon> mFolderIcon;
    Int32 mMaxCountX;
    Int32 mMaxCountY;
    Int32 mMaxNumItems;
    AutoPtr<IDrawable> mIconDrawable;
    AutoPtr<IShortcutInfo> mCurrentDragInfo;
    AutoPtr<IView> mCurrentDragView;
    Int32 mFolderNameHeight;
    Boolean mDragInProgress;
    Boolean mDeleteFolderOnDropCompleted;
    Boolean mSuppressFolderDeletion;
    Boolean mItemAddedBackToSelfViaIcon;
    Float mFolderIconPivotX;
    Float mFolderIconPivotY;
    Boolean mIsEditingName;
    AutoPtr<IInputMethodManager> mInputMethodManager;
    static String sDefaultFolderName;
    static String sHintText;
    Boolean mDestroyed;

    AutoPtr<IArrayList> mItemsInReadingOrder;
    AutoPtr<ArrayOf<Int32> > mTargetCell;
    AutoPtr<ArrayOf<Int32> > mPreviousTargetCell;
    AutoPtr<ArrayOf<Int32> > mEmptyCell;
    AutoPtr<IAlarm> mReorderAlarm;
    AutoPtr<IAlarm> mOnExitAlarm;
    AutoPtr<IRect> mTempRect;
};

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_LAUNCHER2_FOLDER_H__