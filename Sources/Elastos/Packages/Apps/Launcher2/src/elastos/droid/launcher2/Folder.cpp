
#include "elastos/droid/launcher2/Folder.h"
#include "elastos/droid/launcher2/FolderInfo.h"
#include "elastos/droid/launcher2/LauncherModel.h"
#include "elastos/droid/launcher2/LauncherAnimUtils.h"
#include "elastos/droid/launcher2/ItemInfo.h"
#include "elastos/droid/launcher2/FastBitmapDrawable.h"
#include "elastos/droid/launcher2/Alarm.h"
#include "elastos/droid/launcher2/DropTarget.h"
#include "elastos/droid/launcher2/FocusHelper.h"
#include "elastos/droid/launcher2/CellLayout.h"
#include "elastos/droid/launcher2/DragLayer.h"
#include "elastos/droid/view/LayoutInflater.h"
#include "elastos/droid/view/View.h"
#include "Elastos.Droid.Service.h"
#include "Elastos.Droid.Text.h"
#include <elastos/core/Math.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Slogger.h>
#include "R.h"

using Elastos::Droid::Animation::IObjectAnimator;
using Elastos::Droid::Animation::IPropertyValuesHolderHelper;
using Elastos::Droid::Animation::CPropertyValuesHolderHelper;
using Elastos::Droid::Animation::IPropertyValuesHolder;
using Elastos::Droid::Graphics::CRect;
using Elastos::Droid::Graphics::IRect;
using Elastos::Droid::Text::IInputType;
using Elastos::Droid::Text::CSelection;
using Elastos::Droid::Text::ISelection;
using Elastos::Droid::Text::ISpannable;
using Elastos::Droid::View::View;
using Elastos::Droid::View::LayoutInflater;
using Elastos::Droid::View::EIID_IActionModeCallback;
using Elastos::Droid::View::EIID_IViewOnClickListener;
using Elastos::Droid::View::EIID_IViewOnLongClickListener;
using Elastos::Droid::View::EIID_IViewOnFocusChangeListener;
using Elastos::Droid::View::Accessibility::IAccessibilityRecord;
using Elastos::Droid::View::Accessibility::IAccessibilityManager;
using Elastos::Droid::View::Accessibility::CAccessibilityEventHelper;
using Elastos::Droid::View::Accessibility::IAccessibilityEventHelper;
using Elastos::Droid::Widget::ITextView;
using Elastos::Droid::Widget::EIID_IOnEditorActionListener;
using Elastos::Core::EIID_IComparator;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Slogger;
using Elastos::Utility::IArrayList;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollections;

namespace Elastos {
namespace Droid {
namespace Launcher2 {

CAR_INTERFACE_IMPL(Folder::MyActionModeCallback, Object, IActionModeCallback);

ECode Folder::MyActionModeCallback::OnCreateActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);

    *res = FALSE;
    return NOERROR;
}

ECode Folder::MyActionModeCallback::OnPrepareActionMode(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenu* menu,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode Folder::MyActionModeCallback::OnActionItemClicked(
    /* [in] */ IActionMode* mode,
    /* [in] */ IMenuItem* item,
    /* [out] */ Boolean* res)
{
    VALIDATE_NOT_NULL(res);
    *res = FALSE;
    return NOERROR;
}

ECode Folder::MyActionModeCallback::OnDestroyActionMode(
    /* [in] */ IActionMode* mode)
{
    return NOERROR;
}

CAR_INTERFACE_IMPL(Folder::GridComparator, Object, IComparator);

Folder::GridComparator::GridComparator(
    /* [in] */ Int32 numCols)
    : mNumCols(numCols)
{
}

ECode Folder::GridComparator::Compare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ShortcutInfo> _lhs = (ShortcutInfo*)IShortcutInfo::Probe(lhs);
    AutoPtr<ShortcutInfo> _rhs = (ShortcutInfo*)IShortcutInfo::Probe(rhs);

    Int32 lhIndex = _lhs->mCellY * mNumCols + _lhs->mCellX;
    Int32 rhIndex = _rhs->mCellY * mNumCols + _rhs->mCellX;
    *result = (lhIndex - rhIndex);
    return NOERROR;
}

Folder::MyAnimatorListenerAdapter::MyAnimatorListenerAdapter(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyAnimatorListenerAdapter::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    String str;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::folder_opened, &str);
    Int32 x;
    mHost->mContent->GetCountX(&x);
    Int32 y;
    mHost->mContent->GetCountY(&y);

    String tmp;
    tmp.AppendFormat(str, x, y);
    mHost->SendCustomAccessibilityEvent(
            IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED, tmp);
    mHost->mState = STATE_ANIMATING;
    return NOERROR;
}

ECode Folder::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->mState = STATE_OPEN;
    mHost->SetLayerType(LAYER_TYPE_NONE, NULL);
    AutoPtr<ICling> cling;
    mHost->mLauncher->ShowFirstRunFoldersCling((ICling**)&cling);
    if (cling != NULL) {
        IView::Probe(cling)->BringToFront();
    }
    mHost->SetFocusOnFirstChild();
    return NOERROR;
}

Folder::MyAnimatorListenerAdapter2::MyAnimatorListenerAdapter2(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyAnimatorListenerAdapter2::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mHost->OnCloseComplete();
    mHost->SetLayerType(LAYER_TYPE_NONE, NULL);
    mHost->mState = STATE_SMALL;
    return NOERROR;
}

ECode Folder::MyAnimatorListenerAdapter2::OnAnimationStart(
    /* [in] */ IAnimator* animation)
{
    AutoPtr<IContext> context;
    mHost->GetContext((IContext**)&context);
    String str;
    context->GetString(
            Elastos::Droid::Launcher2::R::string::folder_closed, &str);
    mHost->SendCustomAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED, str);
    mHost->mState = STATE_ANIMATING;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Folder::MyOnAlarmListener, Object, IAlarmOnAlarmListener);

Folder::MyOnAlarmListener::MyOnAlarmListener(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyOnAlarmListener::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    mHost->RealTimeReorder(mHost->mEmptyCell, mHost->mTargetCell);
    return NOERROR;
}

CAR_INTERFACE_IMPL(Folder::MyOnAlarmListener2, Object, IAlarmOnAlarmListener);

Folder::MyOnAlarmListener2::MyOnAlarmListener2(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyOnAlarmListener2::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    return mHost->CompleteDragExit();
}

Folder::MyRunnable::MyRunnable(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyRunnable::Run()
{
    FolderInfo* _Info = (FolderInfo*)(mHost->mInfo.Get());
    AutoPtr<ICellLayout> cellLayout;
    mHost->mLauncher->GetCellLayout(_Info->mContainer, _Info->mScreen,
            (ICellLayout**)&cellLayout);

    AutoPtr<IView> child;
    // Move the item from the folder to the workspace, in the position of the folder
    Int32 count;
    mHost->GetItemCount(&count);
    if (count == 1) {
        AutoPtr<IInterface> obj;
        _Info->mContents->Get(0, (IInterface**)&obj);
        AutoPtr<IShortcutInfo> finalItem = IShortcutInfo::Probe(obj);
        mHost->mLauncher->CreateShortcut(
                Elastos::Droid::Launcher2::R::layout::application, IViewGroup::Probe(cellLayout),
                finalItem, (IView**)&child);
        LauncherModel::AddOrMoveItemInDatabase(IContext::Probe(mHost->mLauncher),
                IItemInfo::Probe(finalItem), _Info->mContainer, _Info->mScreen,
                _Info->mCellX, _Info->mCellY);
    }
    mHost->GetItemCount(&count);
    if (count <= 1) {
        // Remove the folder
        LauncherModel::DeleteItemFromDatabase(IContext::Probe(mHost->mLauncher),
                (ItemInfo*)(IItemInfo::Probe(mHost->mInfo)));
        IViewGroup::Probe(cellLayout)->RemoveView(IView::Probe(mHost->mFolderIcon));
        if (IDropTarget::Probe(mHost->mFolderIcon) != NULL) {
            mHost->mDragController->RemoveDropTarget(IDropTarget::Probe(mHost->mFolderIcon));
        }
        mHost->mLauncher->RemoveFolder(mHost->mInfo);
    }
    // We add the child after removing the folder to prevent both from existing at
    // the same time in the CellLayout.
    if (child != NULL) {
        AutoPtr<IWorkspace> workspace;
        mHost->mLauncher->GetWorkspace((IWorkspace**)&workspace);
        workspace->AddInScreen(child, _Info->mContainer, _Info->mScreen,
                _Info->mCellX, _Info->mCellY, _Info->mSpanX, _Info->mSpanY);
    }
    return NOERROR;
}

const String Folder::TAG("Launcher.Folder");
const Int32 Folder::REORDER_ANIMATION_DURATION = 230;
const Int32 Folder::ON_EXIT_CLOSE_DELAY = 800;
String Folder::sDefaultFolderName;
String Folder::sHintText;

CAR_INTERFACE_IMPL_8(Folder, LinearLayout, IFolder, IDragSource, IViewOnClickListener,
        IViewOnLongClickListener, IDropTarget, IFolderListener,
        IOnEditorActionListener, IViewOnFocusChangeListener);

Folder::Folder()
    : mItemsInvalidated(FALSE)
    , mSuppressOnAdd(FALSE)
    , mExpandDuration(0)
    , mState(STATE_NONE)
    , mRearrangeOnClose(FALSE)
    , mMaxCountX(0)
    , mMaxCountY(0)
    , mMaxNumItems(0)
    , mFolderNameHeight(0)
    , mDragInProgress(FALSE)
    , mDeleteFolderOnDropCompleted(FALSE)
    , mSuppressFolderDeletion(FALSE)
    , mItemAddedBackToSelfViaIcon(FALSE)
    , mFolderIconPivotX(0.0f)
    , mFolderIconPivotY(0.0f)
    , mIsEditingName(FALSE)
    , mDestroyed(FALSE)
{
    CArrayList::New((IArrayList**)&mItemsInReadingOrder);
    mTargetCell = ArrayOf<Int32>::Alloc(2);
    mPreviousTargetCell = ArrayOf<Int32>::Alloc(2);
    mEmptyCell = ArrayOf<Int32>::Alloc(2);
    mReorderAlarm = new Alarm();
    mOnExitAlarm = new Alarm();
    CRect::New((IRect**)&mTempRect);

    mReorderAlarmListener = new MyOnAlarmListener(this);
    mOnExitAlarmListener = new MyOnAlarmListener2(this);

    mActionModeCallback = new MyActionModeCallback();
}

ECode Folder::constructor()
{
    return NOERROR;
}

ECode Folder::constructor(
    /* [in] */ IContext* context,
    /* [in] */ IAttributeSet* attrs)
{
    LinearLayout::constructor(context, attrs);
    SetAlwaysDrawnWithCacheEnabled(FALSE);
    LayoutInflater::From(context, (ILayoutInflater**)&mInflater);

    AutoPtr<IContext> ctx;
    context->GetApplicationContext((IContext**)&ctx);
    ILauncherApplication::Probe(ctx)->GetIconCache((IIconCache**)&mIconCache);

    AutoPtr<IResources> res;
    GetResources((IResources**)&res);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::folder_max_count_x,
            &mMaxCountX);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::folder_max_count_y,
            &mMaxCountY);
    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::folder_max_num_items,
            &mMaxNumItems);
    if (mMaxCountX < 0 || mMaxCountY < 0 || mMaxNumItems < 0) {
        LauncherModel::GetCellCountX(&mMaxCountX);
        LauncherModel::GetCellCountY(&mMaxCountY);
        mMaxNumItems = mMaxCountX * mMaxCountY;
    }

    AutoPtr<IContext> _context;
    GetContext((IContext**)&_context);
    AutoPtr<IInterface> obj;
    _context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInputMethodManager = IInputMethodManager::Probe(obj);

    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_folderAnimDuration,
            &mExpandDuration);

    if (sDefaultFolderName == NULL) {
        res->GetString(
                Elastos::Droid::Launcher2::R::string::folder_name, &sDefaultFolderName);
    }
    if (sHintText == NULL) {
        res->GetString(
                Elastos::Droid::Launcher2::R::string::folder_hint_text, &sHintText);
    }
    mLauncher = ILauncher::Probe(context);
    // We need this view to be focusable in touch mode so that when text editing of the folder
    // name is complete, we have something to focus on, thus hiding the cursor and giving
    // reliable behvior when clicking the text field (since it will always gain focus on click).
    return SetFocusableInTouchMode(TRUE);
}

ECode Folder::OnFinishInflate()
{
    LinearLayout::OnFinishInflate();
    AutoPtr<IView> view;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::folder_content,
            (IView**)&view);
    mContent = ICellLayout::Probe(view);
    mContent->SetGridSize(0, 0);
    AutoPtr<IShortcutAndWidgetContainer> container;
    mContent->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    IViewGroup::Probe(container)->SetMotionEventSplittingEnabled(FALSE);
    mContent->SetInvertIfRtl(TRUE);
    AutoPtr<IView> view2;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::folder_name,
            (IView**)&view2);
    mFolderName = IFolderEditText::Probe(view2);
    mFolderName->SetFolder(this);
    IView::Probe(mFolderName)->SetOnFocusChangeListener(this);

    // We find out how tall the text view wants to be (it is set to wrap_content), so that
    // we can allocate the appropriate amount of space for it.
    Int32 measureSpec = View::MeasureSpec::UNSPECIFIED;
    IView::Probe(mFolderName)->Measure(measureSpec, measureSpec);
    IView::Probe(mFolderName)->GetMeasuredHeight(&mFolderNameHeight);

    // We disable action mode for now since it messes up the view on phones
    ITextView::Probe(mFolderName)->SetCustomSelectionActionModeCallback(mActionModeCallback);
    ITextView::Probe(mFolderName)->SetOnEditorActionListener(this);
    ITextView::Probe(mFolderName)->SetSelectAllOnFocus(TRUE);

    Int32 type;
    ITextView::Probe(mFolderName)->GetInputType(&type);
    return ITextView::Probe(mFolderName)->SetInputType(type |
            IInputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS | IInputType::TYPE_TEXT_FLAG_CAP_WORDS);
}

ECode Folder::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    if (IShortcutInfo::Probe(tag) != NULL) {
        // refactor this code from Folder
        AutoPtr<ShortcutInfo> item = (ShortcutInfo*)IShortcutInfo::Probe(tag);
        AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
        v->GetLocationOnScreen(pos);

        Int32 width;
        v->GetWidth(&width);
        Int32 height;
        v->GetHeight(&height);
        AutoPtr<IRect> rect;
        CRect::New((*pos)[0], (*pos)[1], (*pos)[0] + width, (*pos)[1] + height, (IRect**)&rect);
        item->mIntent->SetSourceBounds(rect);

        Boolean tmp;
        return mLauncher->StartActivitySafely(v, item->mIntent, TO_IINTERFACE(item), &tmp);
    }
    return NOERROR;
}

ECode Folder::OnLongClick(
    /* [in] */ IView* v,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // Return if global dragging is not enabled
    Boolean res;
    mLauncher->IsDraggingEnabled(&res);
    if (!res) {
        *result = TRUE;
        return NOERROR;
    }

    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    if (IShortcutInfo::Probe(tag) != NULL) {
        AutoPtr<ShortcutInfo> item = (ShortcutInfo*)IShortcutInfo::Probe(tag);
        Boolean tmp;
        v->IsInTouchMode(&tmp);
        if (!tmp) {
            *result = FALSE;
            return NOERROR;
        }
        mLauncher->DismissFolderCling(NULL);

        AutoPtr<IWorkspace> workspace;
        mLauncher->GetWorkspace((IWorkspace**)&workspace);
        workspace->OnDragStartedWithItem(v);
        workspace->BeginDragShared(v, this);
        AutoPtr<ITextView> textView = ITextView::Probe(v);
        AutoPtr<ArrayOf<IDrawable*> > array;
        textView->GetCompoundDrawables((ArrayOf<IDrawable*>**)&array);
        mIconDrawable = (*array)[1];

        mCurrentDragInfo = item;
        (*mEmptyCell)[0] = item->mCellX;
        (*mEmptyCell)[1] = item->mCellY;
        mCurrentDragView = v;

        IViewGroup::Probe(mContent)->RemoveView(mCurrentDragView);
        mInfo->Remove(mCurrentDragInfo);
        mDragInProgress = TRUE;
        mItemAddedBackToSelfViaIcon = FALSE;
    }
    *result = TRUE;
    return NOERROR;
}

ECode Folder::IsEditingName(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mIsEditingName;
    return NOERROR;
}

ECode Folder::StartEditingFolderName()
{
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
    ITextView::Probe(mFolderName)->SetHint(cchar);
    mIsEditingName = TRUE;
    return NOERROR;
}

ECode Folder::DismissEditingName()
{
    AutoPtr<IBinder> token;
    GetWindowToken((IBinder**)&token);
    Boolean ret;
    mInputMethodManager->HideSoftInputFromWindow(token, 0, &ret);
    return DoneEditingFolderName(TRUE);
}

ECode Folder::DoneEditingFolderName(
    /* [in] */ Boolean commit)
{
    AutoPtr<ICharSequence> cchar = CoreUtils::Convert(sHintText);
    ITextView::Probe(mFolderName)->SetHint(cchar);
    // Convert to a string here to ensure that no other state associated with the text field
    // gets saved.
    AutoPtr<ICharSequence> text;
    ITextView::Probe(mFolderName)->GetText((ICharSequence**)&text);
    String newTitle;
    text->ToString(&newTitle);
    mInfo->SetTitle(text);
    LauncherModel::UpdateItemInDatabase(IContext::Probe(mLauncher), (ItemInfo*)IItemInfo::Probe(mInfo));

    if (commit) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String str;
        context->GetString(
            Elastos::Droid::Launcher2::R::string::folder_renamed,
            &str);

        String arg;
        arg.AppendFormat(str, newTitle.string());
        SendCustomAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED, arg);
    }
    // In order to clear the focus from the text field, we set the focus on ourself. This
    // ensures that every time the field is clicked, focus is gained, giving reliable behavior.
    Boolean tmp;
    RequestFocus(&tmp);

    AutoPtr<ISelection> sel;
    CSelection::AcquireSingleton((ISelection**)&sel);
    sel->SetSelection(ISpannable::Probe(text), 0, 0);
    mIsEditingName = FALSE;
    return NOERROR;
}

ECode Folder::OnEditorAction(
    /* [in] */ ITextView* v,
    /* [in] */ Int32 actionId,
    /* [in] */ IKeyEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if (actionId == IEditorInfo::IME_ACTION_DONE) {
        DismissEditingName();
        *result = TRUE;
        return NOERROR;
    }
    *result = FALSE;
    return NOERROR;
}

ECode Folder::GetEditTextRegion(
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    *view = IView::Probe(mFolderName);
    REFCOUNT_ADD(*view);
    return NOERROR;
}

ECode Folder::GetDragDrawable(
    /* [out] */ IDrawable** drawable)
{
    VALIDATE_NOT_NULL(drawable);

    *drawable = mIconDrawable;
    REFCOUNT_ADD(*drawable);
    return NOERROR;
}

ECode Folder::OnTouchEvent(
    /* [in] */ IMotionEvent* ev,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode Folder::SetDragController(
    /* [in] */ IDragController* dragController)
{
    mDragController = dragController;
    return NOERROR;
}

ECode Folder::SetFolderIcon(
    /* [in] */ IFolderIcon* icon)
{
    mFolderIcon = icon;
    return NOERROR;
}

ECode Folder::DispatchPopulateAccessibilityEvent(
    /* [in] */ IAccessibilityEvent* event,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    // When the folder gets focus, we don't want to announce the list of items.
    *result = TRUE;
    return NOERROR;
}

ECode Folder::GetInfo(
    /* [out] */ IFolderInfo** info)
{
    VALIDATE_NOT_NULL(info);

    *info = mInfo;
    REFCOUNT_ADD(*info);
    return NOERROR;
}

void Folder::PlaceInReadingOrder(
    /* [in] */ IArrayList* items)
{
    Int32 maxX = 0;
    Int32 count;
    items->GetSize(&count);
    for (Int32 i = 0; i < count; i++) {
        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<ShortcutInfo> item = (ShortcutInfo*)IShortcutInfo::Probe(obj);
        if (item->mCellX > maxX) {
            maxX = item->mCellX;
        }
    }

    AutoPtr<IComparator> gridComparator = new GridComparator(maxX + 1);
    AutoPtr<ICollections> helper;
    CCollections::AcquireSingleton((ICollections**)&helper);
    helper->Sort(IList::Probe(items), gridComparator);
    Int32 countX;
    mContent->GetCountX(&countX);
    for (Int32 i = 0; i < count; i++) {
        Int32 x = i % countX;
        Int32 y = i / countX;
        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<ShortcutInfo> item = (ShortcutInfo*)IShortcutInfo::Probe(obj);
        item->mCellX = x;
        item->mCellY = y;
    }
}

ECode Folder::Bind(
    /* [in] */ IFolderInfo* info)
{
    mInfo = info;
    FolderInfo* _info = (FolderInfo*)info;
    AutoPtr<IArrayList> children  = _info->mContents;

    AutoPtr<IArrayList> overflow;
    CArrayList::New((IArrayList**)&overflow);
    Int32 size;
    children->GetSize(&size);
    SetupContentForNumItems(size);
    PlaceInReadingOrder(children);
    Int32 count = 0;
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        children->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutInfo> child = IShortcutInfo::Probe(obj);
        Boolean res;
        CreateAndAddShortcut(child, &res);
        if (!res) {
            overflow->Add(TO_IINTERFACE(child));
        }
        else {
            count++;
        }
    }

    // We rearrange the items in case there are any empty gaps
    SetupContentForNumItems(count);

    // If our folder has too many items we prune them from the list. This is an issue
    // when upgrading from the old Folders implementation which could contain an unlimited
    // number of items.
    Int32 size2;
    overflow->GetSize(&size2);
    for (Int32 i = 0; i < size2; i++) {
        AutoPtr<IInterface> obj;
        overflow->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(obj);

        mInfo->Remove(item);
        LauncherModel::DeleteItemFromDatabase(IContext::Probe(mLauncher),
                (ItemInfo*)IItemInfo::Probe(item));
    }

    mItemsInvalidated = TRUE;
    UpdateTextViewFocus();
    mInfo->AddListener(this);

    String str;
    ((FolderInfo*)mInfo.Get())->mTitle->ToString(&str);
    if (!sDefaultFolderName.Equals(str)) {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(str);
        ITextView::Probe(mFolderName)->SetText(cchar);
    }
    else {
        AutoPtr<ICharSequence> cchar = CoreUtils::Convert(String(""));
        ITextView::Probe(mFolderName)->SetText(cchar);
    }
    UpdateItemLocationsInDatabase();
    return NOERROR;
}

ECode Folder::FromXml(
    /* [in] */ IContext* context,
    /* [out] */ IFolder** folder)
{
    VALIDATE_NOT_NULL(folder);

    AutoPtr<ILayoutInflater> inflater;
    LayoutInflater::From(context, (ILayoutInflater**)&inflater);
    AutoPtr<IView> view;
    inflater->Inflate(Elastos::Droid::Launcher2::R::layout::user_folder,
            NULL, (IView**)&view);
    *folder = IFolder::Probe(view);
    REFCOUNT_ADD(*folder);
    return NOERROR;
}

void Folder::PositionAndSizeAsIcon()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IDragLayer::Probe(res) == NULL) return;
    SetScaleX(0.8f);
    SetScaleY(0.8f);
    SetAlpha(0.0f);
    mState = STATE_SMALL;
    return;
}

ECode Folder::AnimateOpen()
{
    PositionAndSizeAsIcon();

    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IDragLayer::Probe(res) == NULL) return NOERROR;
    CenterAboutIcon();
    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array = ArrayOf<Float>::Alloc(1);
    (*array)[0] = 1.0f;
    AutoPtr<IPropertyValuesHolder> alpha;
    helper->OfFloat(String("alpha"), array, (IPropertyValuesHolder**)&alpha);
    AutoPtr<IPropertyValuesHolder> scaleX;
    helper->OfFloat(String("scaleX"), array, (IPropertyValuesHolder**)&scaleX);
    AutoPtr<IPropertyValuesHolder> scaleY;
    helper->OfFloat(String("scaleY"), array, (IPropertyValuesHolder**)&scaleY);
    AutoPtr<ArrayOf<IPropertyValuesHolder*> > values = ArrayOf<IPropertyValuesHolder*>::Alloc(3);
    values->Set(0, alpha);
    values->Set(1, scaleX);
    values->Set(2, scaleY);
    AutoPtr<IObjectAnimator> oa = LauncherAnimUtils::OfPropertyValuesHolder(IView::Probe(this), values);

    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter(this);
Slogger::D("Folder", "=======Folder::AnimateOpen listener=%p",listener.Get());
    IAnimator::Probe(oa)->AddListener(listener);
    IAnimator::Probe(oa)->SetDuration(mExpandDuration);
    SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    return IAnimator::Probe(oa)->Start();
}

void Folder::SendCustomAccessibilityEvent(
    /* [in] */ Int32 type,
    /* [in] */ const String& text)
{

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IAccessibilityManager> accessibilityManager =
            IAccessibilityManager::Probe(obj);

    Boolean res;
    accessibilityManager->IsEnabled(&res);
    if (res) {
        AutoPtr<IAccessibilityEvent> event;
        AutoPtr<IAccessibilityEventHelper> helper;
        CAccessibilityEventHelper::AcquireSingleton((IAccessibilityEventHelper**)&helper);
        helper->Obtain(type, (IAccessibilityEvent**)&event);
        OnInitializeAccessibilityEvent(event);

        AutoPtr<IList> list;
        IAccessibilityRecord::Probe(event)->GetText((IList**)&list);
        AutoPtr<ICharSequence> cstr = CoreUtils::Convert(text);
        list->Add(TO_IINTERFACE(cstr));
        accessibilityManager->SendAccessibilityEvent(event);
    }
}

void Folder::SetFocusOnFirstChild()
{
    AutoPtr<IView> firstChild;
    mContent->GetChildAt(0, 0, (IView**)&firstChild);
    if (firstChild != NULL) {
        Boolean tmp;
        firstChild->RequestFocus(&tmp);
    }
}

ECode Folder::AnimateClosed()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IDragLayer::Probe(res) == NULL) return NOERROR;

    AutoPtr<IPropertyValuesHolder> alpha;
    AutoPtr<IPropertyValuesHolderHelper> helper;
    CPropertyValuesHolderHelper::AcquireSingleton((IPropertyValuesHolderHelper**)&helper);
    AutoPtr<ArrayOf<Float> > array1 = ArrayOf<Float>::Alloc(1);
    (*array1)[0] = 0.0f;
    AutoPtr<ArrayOf<Float> > array2 = ArrayOf<Float>::Alloc(1);
    (*array2)[0] = 0.9f;
    helper->OfFloat(String("alpha"), array1, (IPropertyValuesHolder**)&alpha);
    AutoPtr<IPropertyValuesHolder> scaleX;
    helper->OfFloat(String("scaleX"), array2, (IPropertyValuesHolder**)&scaleX);
    AutoPtr<IPropertyValuesHolder> scaleY;
    helper->OfFloat(String("scaleY"), array2, (IPropertyValuesHolder**)&scaleY);

    AutoPtr<ArrayOf<IPropertyValuesHolder*> > values = ArrayOf<IPropertyValuesHolder*>::Alloc(3);
    values->Set(0, alpha);
    values->Set(1, scaleX);
    values->Set(2, scaleY);
    AutoPtr<IObjectAnimator> oa = LauncherAnimUtils::OfPropertyValuesHolder(this, values);

    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter2(this);
Slogger::D("Folder", "=======Folder::AnimateClosed listener=%p",listener.Get());
    IAnimator::Probe(oa)->AddListener(listener);
    IAnimator::Probe(oa)->SetDuration(mExpandDuration);
    SetLayerType(LAYER_TYPE_HARDWARE, NULL);
    return IAnimator::Probe(oa)->Start();
}

ECode Folder::NotifyDataSetChanged()
{
    // recreate all the children if the data set changes under us. We may want to do this more
    // intelligently (ie just removing the views that should no longer exist)
    IViewGroup::Probe(mContent)->RemoveAllViewsInLayout();
    return Bind(mInfo);
}

ECode Folder::AcceptDrop(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    DragObject* _d = (DragObject*)d;
    AutoPtr<ItemInfo> item = (ItemInfo*)IItemInfo::Probe(_d->mDragInfo);
    Int32 itemType = item->mItemType;
    Boolean res;
    IsFull(&res);
    *result = ((itemType == ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_APPLICATION ||
            itemType == ILauncherSettingsBaseLauncherColumns::ITEM_TYPE_SHORTCUT) &&
            !res);
    return NOERROR;
}

ECode Folder::FindAndSetEmptyCells(
    /* [in] */ IShortcutInfo* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Int32> > emptyCell = ArrayOf<Int32>::Alloc(2);
    ShortcutInfo* _item = (ShortcutInfo*)item;
    Boolean res;
    mContent->FindCellForSpan(emptyCell, _item->mSpanX, _item->mSpanY, &res);
    if (res) {
        _item->mCellX = (*emptyCell)[0];
        _item->mCellY = (*emptyCell)[1];
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

ECode Folder::CreateAndAddShortcut(
    /* [in] */ IShortcutInfo* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IView> view;
    mInflater->Inflate(
            Elastos::Droid::Launcher2::R::layout::application, this, FALSE,
            (IView**)&view);
    AutoPtr<ITextView> textView = ITextView::Probe(view);
    AutoPtr<ShortcutInfo> _item = (ShortcutInfo*)item;
    AutoPtr<IBitmap> map = _item->GetIcon((IconCache*)mIconCache.Get());
    AutoPtr<FastBitmapDrawable> drawable = new FastBitmapDrawable();
    drawable->constructor(map);
    textView->SetCompoundDrawablesWithIntrinsicBounds(NULL,
            IDrawable::Probe(drawable), NULL, NULL);
    textView->SetText(_item->mTitle);
    if (_item->mContentDescription != NULL) {
        IView::Probe(textView)->SetContentDescription(_item->mContentDescription);
    }
    IView::Probe(textView)->SetTag(item);

    IView::Probe(textView)->SetOnClickListener(this);
    IView::Probe(textView)->SetOnLongClickListener(this);

    // We need to check here to verify that the given item's location isn't already occupied
    // by another item.
    AutoPtr<IView> tmp;
    mContent->GetChildAt(_item->mCellX, _item->mCellY, (IView**)&tmp);
    Int32 x;
    mContent->GetCountX(&x);
    Int32 y;
    mContent->GetCountY(&y);
    if (tmp != NULL || _item->mCellX < 0 || _item->mCellY < 0
            || _item->mCellX >= x || _item->mCellY >= y) {
        // This shouldn't happen, log it.
        Slogger::E(TAG, "Folder order not properly persisted during bind");
        Boolean res;
        FindAndSetEmptyCells(item, &res);
        if (!res) {
            *result = FALSE;
            return NOERROR;
        }
    }

    AutoPtr<CellLayout::LayoutParams> lp = new CellLayout::LayoutParams();
    lp->constructor(_item->mCellX, _item->mCellY, _item->mSpanX, _item->mSpanY);
    Boolean insert = FALSE;
    AutoPtr<IViewOnKeyListener> lis = new FolderKeyEventListener();
    IView::Probe(textView)->SetOnKeyListener(lis);
    Boolean res;
    mContent->AddViewToCellLayout(IView::Probe(textView), insert ? 0 : -1,
            (Int32)_item->mId, ICellLayoutLayoutParams::Probe(lp), TRUE, &res);
    *result = TRUE;
    return NOERROR;
}

ECode Folder::OnDragEnter(
    /* [in] */ IDropTargetDragObject* d)
{
    (*mPreviousTargetCell)[0] = -1;
    (*mPreviousTargetCell)[1] = -1;
    return mOnExitAlarm->CancelAlarm();
}

ECode Folder::ReadingOrderGreaterThan(
    /* [in] */ ArrayOf<Int32>* v1,
    /* [in] */ ArrayOf<Int32>* v2,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    if ((*v1)[1] > (*v2)[1] || ((*v1)[1] == (*v2)[1] && (*v1)[0] > (*v2)[0])) {
        *result = TRUE;
        return NOERROR;
    }
    else {
        *result = FALSE;
        return NOERROR;
    }
    return NOERROR;
}

void Folder::RealTimeReorder(
    /* [in] */ ArrayOf<Int32>* empty,
    /* [in] */ ArrayOf<Int32>* target)
{
    Boolean wrap;
    Int32 startX;
    Int32 endX;
    Int32 startY;
    Int32 delay = 0;
    Float delayAmount = 30;

    Boolean res;
    ReadingOrderGreaterThan(target, empty, &res);
    if (res) {
        Int32 countX;
        mContent->GetCountX(&countX);
        wrap = (*empty)[0] >= countX - 1;
        startY = wrap ? (*empty)[1] + 1 : (*empty)[1];
        for (Int32 y = startY; y <= (*target)[1]; y++) {
            startX = y == (*empty)[1] ? (*empty)[0] + 1 : 0;
            endX = y < (*target)[1] ? countX - 1 : (*target)[0];
            for (Int32 x = startX; x <= endX; x++) {
                AutoPtr<IView> v;
                mContent->GetChildAt(x, y, (IView**)&v);
                Boolean tmp;
                mContent->AnimateChildToPosition(v, (*empty)[0], (*empty)[1],
                        REORDER_ANIMATION_DURATION, delay, TRUE, TRUE, &tmp);
                if (tmp) {
                    (*empty)[0] = x;
                    (*empty)[1] = y;
                    delay += delayAmount;
                    delayAmount *= 0.9;
                }
            }
        }
    }
    else {
        wrap = (*empty)[0] == 0;
        startY = wrap ? (*empty)[1] - 1 : (*empty)[1];
        for (Int32 y = startY; y >= (*target)[1]; y--) {
            Int32 countX;
            mContent->GetCountX(&countX);
            startX = y == (*empty)[1] ? (*empty)[0] - 1 : countX - 1;
            endX = y > (*target)[1] ? 0 : (*target)[0];
            for (Int32 x = startX; x >= endX; x--) {
                AutoPtr<IView> v;
                mContent->GetChildAt(x, y, (IView**)&v);
                Boolean tmp;
                mContent->AnimateChildToPosition(v, (*empty)[0], (*empty)[1],
                        REORDER_ANIMATION_DURATION, delay, TRUE, TRUE, &tmp);
                if (tmp) {
                    (*empty)[0] = x;
                    (*empty)[1] = y;
                    delay += delayAmount;
                    delayAmount *= 0.9;
                }
            }
        }
    }
}

ECode Folder::IsLayoutRtl(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 direction;
    GetLayoutDirection(&direction);
    *result = (direction == LAYOUT_DIRECTION_RTL);
    return NOERROR;
}

ECode Folder::OnDragOver(
    /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    AutoPtr<ArrayOf<Float> > r = GetDragViewVisualCenter(_d->mX, _d->mY, _d->mXOffset,
            _d->mYOffset, _d->mDragView, NULL);
    mContent->FindNearestArea((Int32)(*r)[0],
            (Int32)(*r)[1], 1, 1, mTargetCell, (ArrayOf<Int32>**)&mTargetCell);

    Boolean res;
    IsLayoutRtl(&res);
    if (res) {
        Int32 x;
        mContent->GetCountX(&x);
        (*mTargetCell)[0] = x - (*mTargetCell)[0] - 1;
    }

    if ((*mTargetCell)[0] != (*mPreviousTargetCell)[0] ||
            (*mTargetCell)[1] != (*mPreviousTargetCell)[1]) {
        mReorderAlarm->CancelAlarm();
        mReorderAlarm->SetOnAlarmListener(mReorderAlarmListener);
        mReorderAlarm->SetAlarm(150);
        (*mPreviousTargetCell)[0] = (*mTargetCell)[0];
        (*mPreviousTargetCell)[1] = (*mTargetCell)[1];
    }
    return NOERROR;
}

AutoPtr<ArrayOf<Float> > Folder::GetDragViewVisualCenter(
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ Int32 xOffset,
    /* [in] */ Int32 yOffset,
    /* [in] */ IDragView* dragView,
    /* [in] */ ArrayOf<Float>* recycle)
{
    AutoPtr<ArrayOf<Float> >  res;
    if (recycle == NULL) {
        res = ArrayOf<Float>::Alloc(2);
    }
    else {
        res = recycle;
    }

    // These represent the visual top and left of drag view if a dragRect was provided.
    // If a dragRect was not provided, then they correspond to the actual view left and
    // top, as the dragRect is in that case taken to be the entire dragView.
    // R.dimen.dragViewOffsetY.
    Int32 left = x - xOffset;
    Int32 top = y - yOffset;

    // In order to find the visual center, we shift by half the dragRect
    AutoPtr<IRect> rect;
    dragView->GetDragRegion((IRect**)&rect);
    Int32 width;
    rect->GetWidth(&width);
    Int32 height;
    rect->GetHeight(&height);
    (*res)[0] = left + width / 2;
    (*res)[1] = top + height / 2;

    return res;
}

ECode Folder::CompleteDragExit()
{
    mLauncher->CloseFolder();
    mCurrentDragInfo = NULL;
    mCurrentDragView = NULL;
    mSuppressOnAdd = FALSE;
    mRearrangeOnClose = TRUE;
    return NOERROR;
}

ECode Folder::OnDragExit(
    /* [in] */ IDropTargetDragObject* d)
{
    // We only close the folder if this is a true drag exit, ie. not because a drop
    // has occurred above the folder.
    DragObject* _d = (DragObject*)d;
    if (!_d->mDragComplete) {
        mOnExitAlarm->SetOnAlarmListener(mOnExitAlarmListener);
        mOnExitAlarm->SetAlarm(ON_EXIT_CLOSE_DELAY);
    }
    return mReorderAlarm->CancelAlarm();
}

ECode Folder::OnDropCompleted(
    /* [in] */ IView* target,
    /* [in] */ IDropTargetDragObject* d,
    /* [in] */ Boolean isFlingToDelete,
    /* [in] */ Boolean success)
{
    if (success) {
        if (mDeleteFolderOnDropCompleted && !mItemAddedBackToSelfViaIcon) {
            ReplaceFolderWithFinalItem();
        }
    }
    else {
        Int32 count;
        GetItemCount(&count);
        SetupContentForNumItems(count);
        // The drag failed, we need to return the item to the folder
        mFolderIcon->OnDrop(d);
    }

    if (TO_IINTERFACE(target) != TO_IINTERFACE(this)) {
        Boolean res;
        mOnExitAlarm->AlarmPending(&res);
        if (res) {
            mOnExitAlarm->CancelAlarm();
            if (!success) {
                mSuppressFolderDeletion = TRUE;
            }
            CompleteDragExit();
        }
    }

    mDeleteFolderOnDropCompleted = FALSE;
    mDragInProgress = FALSE;
    mItemAddedBackToSelfViaIcon = FALSE;
    mCurrentDragInfo = NULL;
    mCurrentDragView = NULL;
    mSuppressOnAdd = FALSE;

    // Reordering may have occured, and we need to save the new item locations. We do this once
    // at the end to prevent unnecessary database operations.
    UpdateItemLocationsInDatabase();
    return NOERROR;
}

ECode Folder::SupportsFlingToDelete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode Folder::OnFlingToDelete(
    /* [in] */ IDropTargetDragObject* d,
    /* [in] */ Int32 x,
    /* [in] */ Int32 y,
    /* [in] */ IPointF* vec)
{
    // Do nothing
    return NOERROR;
}

ECode Folder::OnFlingToDeleteCompleted()
{
    // Do nothing
    return NOERROR;
}

void Folder::UpdateItemLocationsInDatabase()
{
    AutoPtr<IArrayList> list;
    GetItemsInReadingOrder((IArrayList**)&list);
    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);
        AutoPtr<IInterface> obj2;
        v->GetTag((IInterface**)&obj2);
        AutoPtr<ItemInfo> info = (ItemInfo*)IItemInfo::Probe(obj2);
        LauncherModel::MoveItemInDatabase(IContext::Probe(mLauncher), info,
                ((FolderInfo*)mInfo.Get())->mId, 0, info->mCellX, info->mCellY);
    }
}

ECode Folder::NotifyDrop()
{
    if (mDragInProgress) {
        mItemAddedBackToSelfViaIcon = TRUE;
    }
    return NOERROR;
}

ECode Folder::IsDropEnabled(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode Folder::GetDropTargetDelegate(
    /* [in] */ IDropTargetDragObject* d,
    /* [out] */ IDropTarget** target)
{
    VALIDATE_NOT_NULL(target);

    *target = NULL;
    return NOERROR;
}

void Folder::SetupContentDimensions(
    /* [in] */ Int32 count)
{
    AutoPtr<IArrayList> list;
    GetItemsInReadingOrder((IArrayList**)&list);

    Int32 countX;
    mContent->GetCountX(&countX);
    Int32 countY;
    mContent->GetCountY(&countY);
    Boolean done = FALSE;

    while (!done) {
        Int32 oldCountX = countX;
        Int32 oldCountY = countY;
        if (countX * countY < count) {
            // Current grid is too small, expand it
            if ((countX <= countY || countY == mMaxCountY) && countX < mMaxCountX) {
                countX++;
            }
            else if (countY < mMaxCountY) {
                countY++;
            }
            if (countY == 0) countY++;
        }
        else if ((countY - 1) * countX >= count && countY >= countX) {
            countY = Elastos::Core::Math::Max(0, countY - 1);
        }
        else if ((countX - 1) * countY >= count) {
            countX = Elastos::Core::Math::Max(0, countX - 1);
        }
        done = countX == oldCountX && countY == oldCountY;
    }
    mContent->SetGridSize(countX, countY);
    ArrangeChildren(list);
}

ECode Folder::IsFull(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    Int32 count;
    GetItemCount(&count);
    *result = count >= mMaxNumItems;
    return NOERROR;
}

void Folder::CenterAboutIcon()
{
    AutoPtr<IViewGroupLayoutParams> res;
    GetLayoutParams((IViewGroupLayoutParams**)&res);
    AutoPtr<IDragLayerLayoutParams> lp = IDragLayerLayoutParams::Probe(res);

    Int32 left1;
    GetPaddingLeft(&left1);
    Int32 right;
    GetPaddingRight(&right);
    Int32 desiredWidth;
    mContent->GetDesiredWidth(&desiredWidth);
    Int32 width = left1 + right + desiredWidth;

    Int32 top1;
    GetPaddingTop(&top1);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 desiredHeight;
    mContent->GetDesiredHeight(&desiredHeight);
    Int32 height = top1 + bottom + desiredHeight + mFolderNameHeight;

    AutoPtr<IView> view;
    IView::Probe(mLauncher)->FindViewById(
            Elastos::Droid::Launcher2::R::id::drag_layer,
            (IView**)&view);
    AutoPtr<IDragLayer> parent = IDragLayer::Probe(view);
    Float scale;
    parent->GetDescendantRectRelativeToSelf(IView::Probe(mFolderIcon), mTempRect, &scale);

    Int32 left2;
    mTempRect->GetLeft(&left2);
    Int32 width2;
    mTempRect->GetWidth(&width2);
    Int32 centerX = (Int32)(left2 + width2 * scale / 2);
    Int32 top2;
    mTempRect->GetTop(&top2);
    Int32 height2;
    mTempRect->GetHeight(&height2);
    Int32 centerY = (Int32)(top2 + height2 * scale / 2);
    Int32 centeredLeft = centerX - width / 2;
    Int32 centeredTop = centerY - height / 2;

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    Int32 currentPage;
    IPagedView::Probe(workspace)->GetCurrentPage(&currentPage);
    // In case the workspace is scrolling, we need to use the final scroll to compute
    // the folders bounds.
    workspace->SetFinalScrollForPageChange(currentPage);
    // We first fetch the currently visible CellLayoutChildren
    AutoPtr<IView> view2;
    IViewGroup::Probe(workspace)->GetChildAt(currentPage, (IView**)&view2);
    AutoPtr<ICellLayout> currentLayout = ICellLayout::Probe(view2);
    AutoPtr<IShortcutAndWidgetContainer> boundingLayout;
    currentLayout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&boundingLayout);
    AutoPtr<IRect> bounds;
    CRect::New((IRect**)&bounds);
    Float tmp;
    parent->GetDescendantRectRelativeToSelf(IView::Probe(boundingLayout), bounds, &tmp);
    // We reset the workspaces scroll
    workspace->ResetFinalScrollForPageChange(currentPage);

    // We need to bound the folder to the currently visible CellLayoutChildren

    Int32 left3;
    bounds->GetLeft(&left3);
    Int32 width3;
    bounds->GetWidth(&width3);
    Int32 left = Elastos::Core::Math::Min(Elastos::Core::Math::Max(left3, centeredLeft),
            left3 + width3 - width);

    Int32 top3;
    bounds->GetTop(&top3);
    Int32 height3;
    bounds->GetHeight(&height3);
    Int32 top = Elastos::Core::Math::Min(Elastos::Core::Math::Max(top3, centeredTop),
            top3 + height3 - height);
    // If the folder doesn't fit within the bounds, center it about the desired bounds
    if (width >= width3) {
        left = left3 + (width3 - width) / 2;
    }
    if (height >= height3) {
        top = top3 + (height3 - height) / 2;
    }

    Int32 folderPivotX = width / 2 + (centeredLeft - left);
    Int32 folderPivotY = height / 2 + (centeredTop - top);
    SetPivotX(folderPivotX);
    SetPivotY(folderPivotY);
    Int32 measuredWidth;
    IView::Probe(mFolderIcon)->GetMeasuredWidth(&measuredWidth);
    mFolderIconPivotX = (Int32)(measuredWidth *
            (1.0f * folderPivotX / width));
    Int32 measuredHeight;
    IView::Probe(mFolderIcon)->GetMeasuredHeight(&measuredHeight);
    mFolderIconPivotY = (Int32)(measuredHeight *
            (1.0f * folderPivotY / height));

    lp->SetWidth(width);
    lp->SetHeight(height);
    lp->SetX(left);
    lp->SetY(top);
}

ECode Folder::GetPivotXForIconAnimation(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res);

    *res = mFolderIconPivotX;
    return NOERROR;
}

ECode Folder::GetPivotYForIconAnimation(
    /* [out] */ Float* res)
{
    VALIDATE_NOT_NULL(res);

    *res = mFolderIconPivotY;
    return NOERROR;
}

void Folder::SetupContentForNumItems(
    /* [in] */ Int32 count)
{
    SetupContentDimensions(count);

    AutoPtr<IViewGroupLayoutParams> res;
    GetLayoutParams((IViewGroupLayoutParams**)&res);
    AutoPtr<IDragLayerLayoutParams> lp = IDragLayerLayoutParams::Probe(res);

    if (lp == NULL) {
        AutoPtr<DragLayer::LayoutParams> _lp = new DragLayer::LayoutParams();
        _lp->constructor(0, 0);
        _lp->mCustomPosition = TRUE;
        lp = IDragLayerLayoutParams::Probe(_lp);
        SetLayoutParams(IViewGroupLayoutParams::Probe(lp));
    }
    CenterAboutIcon();
}

void Folder::OnMeasure(
    /* [in] */ Int32 widthMeasureSpec,
    /* [in] */ Int32 heightMeasureSpec)
{
    Int32 left;
    GetPaddingLeft(&left);
    Int32 right;
    GetPaddingRight(&right);
    Int32 desiredWidth;
    mContent->GetDesiredWidth(&desiredWidth);
    Int32 width = left + right + desiredWidth;

    Int32 top;
    GetPaddingTop(&top);
    Int32 bottom;
    GetPaddingBottom(&bottom);
    Int32 desiredHeight;
    mContent->GetDesiredHeight(&desiredHeight);
    Int32 height = top + bottom + desiredHeight + mFolderNameHeight;

    Int32 contentWidthSpec = MeasureSpec::MakeMeasureSpec(desiredWidth,
            View::MeasureSpec::EXACTLY);
    Int32 contentHeightSpec = MeasureSpec::MakeMeasureSpec(desiredHeight,
            View::MeasureSpec::EXACTLY);
    IView::Probe(mContent)->Measure(contentWidthSpec, contentHeightSpec);

    IView::Probe(mFolderName)->Measure(contentWidthSpec,
            MeasureSpec::MakeMeasureSpec(mFolderNameHeight, View::MeasureSpec::EXACTLY));
    SetMeasuredDimension(width, height);
}

void Folder::ArrangeChildren(
    /* [in] */ IArrayList* list)
{
    AutoPtr<ArrayOf<Int32> > vacant = ArrayOf<Int32>::Alloc(2);
    if (list == NULL) {
        GetItemsInReadingOrder((IArrayList**)&list);
    }
    IViewGroup::Probe(mContent)->RemoveAllViews();

    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);

        Boolean tmp;
        mContent->GetVacantCell(vacant, 1, 1, &tmp);
        AutoPtr<IViewGroupLayoutParams> res;
        v->GetLayoutParams((IViewGroupLayoutParams**)&res);
        CellLayout::LayoutParams* lp =
                (CellLayout::LayoutParams*)ICellLayoutLayoutParams::Probe(res);
        lp->mCellX = (*vacant)[0];
        lp->mCellY = (*vacant)[1];
        AutoPtr<IInterface> obj2;
        v->GetTag((IInterface**)&obj2);
        ItemInfo* info = (ItemInfo*)IItemInfo::Probe(obj2);
        if (info->mCellX != (*vacant)[0] || info->mCellY != (*vacant)[1]) {
            info->mCellX = (*vacant)[0];
            info->mCellY = (*vacant)[1];
            LauncherModel::AddOrMoveItemInDatabase(IContext::Probe(mLauncher),
                    IItemInfo::Probe(info), ((FolderInfo*)mInfo.Get())->mId, 0,
                    info->mCellX, info->mCellY);
        }
        Boolean insert = FALSE;
        Boolean success;
        mContent->AddViewToCellLayout(v, insert ? 0 : -1, (Int32)(info->mId), lp, TRUE, &success);
    }
    mItemsInvalidated = TRUE;
}

ECode Folder::GetItemCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr<IShortcutAndWidgetContainer> container;
    mContent->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    return IViewGroup::Probe(container)->GetChildCount(count);
}

ECode Folder::GetItemAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IShortcutAndWidgetContainer> container;
    mContent->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    return IViewGroup::Probe(container)->GetChildAt(index, view);
}

void Folder::OnCloseComplete()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    AutoPtr<IDragLayer> parent = IDragLayer::Probe(res);
    if (parent != NULL) {
        IViewGroup::Probe(parent)->RemoveView(this);
    }
    mDragController->RemoveDropTarget(IDropTarget::Probe(this));
    ClearFocus();
    Boolean tmp;
    IView::Probe(mFolderIcon)->RequestFocus(&tmp);

    if (mRearrangeOnClose) {
        Int32 count;
        GetItemCount(&count);
        SetupContentForNumItems(count);
        mRearrangeOnClose = FALSE;
    }
    Int32 count;
    GetItemCount(&count);
    if (count <= 1) {
        if (!mDragInProgress && !mSuppressFolderDeletion) {
            ReplaceFolderWithFinalItem();
        }
        else if (mDragInProgress) {
            mDeleteFolderOnDropCompleted = TRUE;
        }
    }
    mSuppressFolderDeletion = FALSE;
}

void Folder::ReplaceFolderWithFinalItem()
{
    // Add the last remaining child to the workspace in place of the folder
    AutoPtr<IRunnable> onCompleteRunnable  = new MyRunnable(this);
    AutoPtr<IView> finalChild;
    GetItemAt(0, (IView**)&finalChild);
    if (finalChild != NULL) {
        mFolderIcon->PerformDestroyAnimation(finalChild, onCompleteRunnable);
    }
    mDestroyed = TRUE;
}

ECode Folder::IsDestroyed(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = mDestroyed;
    return NOERROR;
}

void Folder::UpdateTextViewFocus()
{
    Int32 count;
    GetItemCount(&count);
    AutoPtr<IView> lastChild;
    GetItemAt(count - 1, (IView**)&lastChild);
    if (lastChild != NULL) {
        Int32 id;
        lastChild->GetId(&id);
        IView::Probe(mFolderName)->SetNextFocusDownId(id);
        IView::Probe(mFolderName)->SetNextFocusRightId(id);
        IView::Probe(mFolderName)->SetNextFocusLeftId(id);
        IView::Probe(mFolderName)->SetNextFocusUpId(id);
    }
}

ECode Folder::OnDrop(
        /* [in] */ IDropTargetDragObject* d)
{
    DragObject* _d = (DragObject*)d;
    AutoPtr<IShortcutInfo> item;
    if (IApplicationInfo::Probe(_d->mDragInfo) != NULL) {
        // Came from all apps -- make a copy
        IApplicationInfo::Probe(_d->mDragInfo)->MakeShortcut((IShortcutInfo**)&item);
        ShortcutInfo* _item = (ShortcutInfo*)item.Get();
        _item->mSpanX = 1;
        _item->mSpanY = 1;
    }
    else {
        item = IShortcutInfo::Probe(_d->mDragInfo);
    }
    // Dragged from self onto self, currently this is the only path possible, however
    // we keep this as a distinct code path.
    if (TO_IINTERFACE(item) == TO_IINTERFACE(mCurrentDragInfo)) {
        AutoPtr<IInterface> obj;
        mCurrentDragView->GetTag((IInterface**)&obj);
        ShortcutInfo* si = (ShortcutInfo*)IShortcutInfo::Probe(obj);

        AutoPtr<IViewGroupLayoutParams> res;
        mCurrentDragView->GetLayoutParams((IViewGroupLayoutParams**)&res);
        CellLayout::LayoutParams* lp = (CellLayout::LayoutParams*)ICellLayoutLayoutParams::Probe(res);
        lp->mCellX = (*mEmptyCell)[0];
        si->mCellX = (*mEmptyCell)[0];
        lp->mCellY = (*mEmptyCell)[1];
        assert(0 && "not mCellY???");
        si->mCellX = (*mEmptyCell)[1];

        ShortcutInfo* _item = (ShortcutInfo*)item.Get();
        Boolean tmp;
        mContent->AddViewToCellLayout(mCurrentDragView, -1, (Int32)(_item->mId), lp, TRUE, &tmp);

        Boolean success;
        _d->mDragView->HasDrawn(&success);
        if (success) {
            AutoPtr<IDragLayer> dragLayer;
            mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
            dragLayer->AnimateViewIntoPosition(_d->mDragView, mCurrentDragView);
        }
        else {
            _d->mDeferDragViewCleanupPostAnimation = FALSE;
            mCurrentDragView->SetVisibility(VISIBLE);
        }
        mItemsInvalidated = TRUE;
        Int32 count;
        GetItemCount(&count);
        SetupContentDimensions(count);
        mSuppressOnAdd = TRUE;
    }
    return mInfo->Add(item);
}

ECode Folder::HideItem(
    /* [in] */ IShortcutInfo* info)
{
    AutoPtr<IView> v = GetViewForInfo(info);
    return v->SetVisibility(INVISIBLE);
}

ECode Folder::ShowItem(
    /* [in] */ IShortcutInfo* info)
{
    AutoPtr<IView> v = GetViewForInfo(info);
    return v->SetVisibility(VISIBLE);
}

ECode Folder::OnAdd(
    /* [in] */ IShortcutInfo* item)
{
    mItemsInvalidated = true;
    // If the item was dropped onto this open folder, we have done the work associated
    // with adding the item to the folder, as indicated by mSuppressOnAdd being set
    if (mSuppressOnAdd) return NOERROR;
    Boolean res;
    FindAndSetEmptyCells(item, &res);
    if (!res) {
        // The current layout is full, can we expand it?
        Int32 count;
        GetItemCount(&count);
        SetupContentForNumItems(count + 1);
        FindAndSetEmptyCells(item, &res);
    }
    CreateAndAddShortcut(item, &res);
    ShortcutInfo* _item = (ShortcutInfo*)item;
    return LauncherModel::AddOrMoveItemInDatabase(IContext::Probe(mLauncher),
            IItemInfo::Probe(item),
            ((FolderInfo*)mInfo.Get())->mId, 0, _item->mCellX, _item->mCellY);
}

ECode Folder::OnRemove(
    /* [in] */ IShortcutInfo* item)
{
    mItemsInvalidated = TRUE;
    // If this item is being dragged from this open folder, we have already handled
    // the work associated with removing the item, so we don't have to do anything here.
    if (TO_IINTERFACE(item) == TO_IINTERFACE(mCurrentDragInfo)) return NOERROR;
    AutoPtr<IView> v = GetViewForInfo(item);
    IViewGroup::Probe(mContent)->RemoveView(v);
    if (mState == STATE_ANIMATING) {
        mRearrangeOnClose = TRUE;
    }
    else {
        Int32 count;
        GetItemCount(&count);
        SetupContentForNumItems(count);
    }
    Int32 count;
    GetItemCount(&count);
    if (count <= 1) {
        ReplaceFolderWithFinalItem();
    }
    return NOERROR;
}

AutoPtr<IView> Folder::GetViewForInfo(
    /* [in] */ IShortcutInfo* item)
{
    Int32 y;
    mContent->GetCountY(&y);
    Int32 x;
    mContent->GetCountX(&x);
    for (Int32 j = 0; j < y; j++) {
        for (Int32 i = 0; i < x; i++) {
            AutoPtr<IView> v;
            mContent->GetChildAt(i, j, (IView**)&v);
            AutoPtr<IInterface> obj;
            v->GetTag((IInterface**)&obj);
            if (TO_IINTERFACE(obj) == TO_IINTERFACE(item)) {
                return v;
            }
        }
    }
    return NULL;
}

ECode Folder::OnItemsChanged()
{
    UpdateTextViewFocus();
    return NOERROR;
}

ECode Folder::OnTitleChanged(
    /* [in] */ ICharSequence* title)
{
    return NOERROR;
}

ECode Folder::GetItemsInReadingOrder(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list);

    if (mItemsInvalidated) {
        mItemsInReadingOrder->Clear();
        Int32 y;
        mContent->GetCountY(&y);
        Int32 x;
        mContent->GetCountX(&x);
        for (Int32 j = 0; j < y; j++) {
            for (Int32 i = 0; i < x; i++) {
                AutoPtr<IView> v;
                mContent->GetChildAt(i, j, (IView**)&v);
                if (v != NULL) {
                    mItemsInReadingOrder->Add(TO_IINTERFACE(v));
                }
            }
        }
        mItemsInvalidated = FALSE;
    }
    *list = mItemsInReadingOrder;
    REFCOUNT_ADD(*list);
    return NOERROR;
}

ECode Folder::GetLocationInDragLayer(
    /* [in] */ ArrayOf<Int32>* loc)
{
    AutoPtr<IDragLayer> dragLayer;
    mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
    Float tmp;
    return dragLayer->GetLocationInDragLayer(IView::Probe(this), loc, &tmp);
}

ECode Folder::OnFocusChange(
    /* [in] */ IView* v,
    /* [in] */ Boolean hasFocus)
{
    if (TO_IINTERFACE(v) == TO_IINTERFACE(mFolderName) && hasFocus) {
        StartEditingFolderName();
    }
    return NOERROR;
}

} // namespace Launcher2
} // namespace Droid
} // namespace Elastos