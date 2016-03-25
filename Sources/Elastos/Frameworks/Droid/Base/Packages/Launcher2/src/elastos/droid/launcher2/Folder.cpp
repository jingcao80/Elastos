
#include "elastos/droid/launcher2/PackageChangedReceiver.h"
#include "Elastos.Droid.Service.h"
#include "R.h"

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

ECode Folder::GridComparatorCompare(
    /* [in] */ IInterface* lhs,
    /* [in] */ IInterface* rhs,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IShortcutInfo> _lhs = IShortcutInfo::Probe(lhs);
    AutoPtr<IShortcutInfo> _rhs = IShortcutInfo::Probe(rhs);

    Int32 lhsCellY;
    lhs->GetCellY(&lhsCellY);
    Int32 lhsCellX;
    lhs->GetCellX(&lhsCellX);
    Int32 lhIndex = lhsCellY * mNumCols + lhsCellX;

    Int32 rhsCellY;
    rhs->GetCellY(&rhsCellY);
    Int32 rhsCellX;
    rhs->GetCellX(&rhsCellX);
    Int32 rhIndex = rhsCellY * mNumCols + rhsCellX;
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
    mHost->SendCustomAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED,
            String::Format(str, x, y));
    mHost->mState = STATE_ANIMATING;
    return NOERROR;
}

ECode Folder::MyAnimatorListenerAdapter::OnAnimationEnd(
    /* [in] */ IAnimator* animation)
{
    mState = STATE_OPEN;
    SetLayerType(LAYER_TYPE_NONE, NULL);
    AutoPtr<ICling> cling;
    mLauncher->ShowFirstRunFoldersCling((ICling**)&cling);
    if (cling != NULL) {
        cling->BringToFront();
    }
    return SetFocusOnFirstChild();
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
    mState = STATE_SMALL;
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
    mState = STATE_ANIMATING;
    return NOERROR;
}

CAR_INTERFACE_IMPL(Folder::MyOnAlarmListener, Object, IOnAlarmListener);

Folder::MyOnAlarmListener::MyOnAlarmListener(
    /* [in] */ Folder* host)
    : mHost(host)
{
}

ECode Folder::MyOnAlarmListener::OnAlarm(
    /* [in] */ IAlarm* alarm)
{
    return mHost->RealTimeReorder(mHost->mEmptyCell, mHost->mTargetCell);
}

CAR_INTERFACE_IMPL(Folder::MyOnAlarmListener2, Object, IOnAlarmListener);

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
    FolderInfo* _Info = (FolderInfo*)mHost->mInfo;
    AutoPtr<ICellLayout> cellLayout;
    mHost->mLauncher->GetCellLayout(_Info->mContainer, _Info->mScreen,
            (ICellLayout)&cellLayout);

    AutoPtr<IView> child;
    // Move the item from the folder to the workspace, in the position of the folder
    Int32 count;
    mHost->GetItemCount(&count);
    if (count == 1) {
        AutoPtr<IShortcutInfo> finalItem;
        _Info->mContents->Get(0, (IShortcutInfo**)&finalItem);
        mHost->mLauncher->CreateShortcut(
                Elastos::Droid::Launcher2::R::layout::application, cellLayout,
                finalItem, (IView**)&child);
        LauncherModel::AddOrMoveItemInDatabase(mHost->mLauncher, finalItem, _Info->mContainer,
                _Info->mScreen, _Info->mCellX, _Info->mCellY);
    }
    mHost->GetItemCount(&count);
    if (count <= 1) {
        // Remove the folder
        LauncherModel::DeleteItemFromDatabase(mHost->mLauncher, mHost->mInfo);
        cellLayout->RemoveView(mHost->mFolderIcon);
        if (IDropTarget::Probe(mHost->mFolderIcon) != NULL) {
            mHost->mDragController->RemoveDropTarget(IDropTarget::Probe(mHost->mFolderIcon));
        }
        mHost->mLauncher->RemoveFolder(mInfo);
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
        IViewOnLongClickListener, IDropTarget, IFolderInfoFolderListener,
        ITextViewOnEditorActionListener, IViewOnFocusChangeListener);

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
    CAlarm::New((IAlarm**)&mReorderAlarm);
    CAlarm::New((IAlarm**)&mOnExitAlarm);
    CRect::New((IRect**)&mTempRect);

    mReorderAlarmListener = new MyOnAlarmListener(this);
    mOnExitAlarmListener = new MyOnAlarmListener2(this);
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

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::INPUT_METHOD_SERVICE, (IInterface**)&obj);
    mInputMethodManager = IInputMethodManager::Probe(obj);

    res->GetInteger(
            Elastos::Droid::Launcher2::R::integer::config_folderAnimDuration,
            &mExpandDuration);

    if (sDefaultFolderName == NULL) {
        sDefaultFolderName = res->GetString(
                Elastos::Droid::Launcher2::R::string::folder_name);
    }
    if (sHintText == NULL) {
        sHintText = res->GetString(
                Elastos::Droid::Launcher2::R::string::folder_hint_text);
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
    container->SetMotionEventSplittingEnabled(FALSE);
    mContent->SetInvertIfRtl(TRUE);
    AutoPtr<IView> view2;
    FindViewById(
            Elastos::Droid::Launcher2::R::id::folder_name
            (IView**)&view2);
    mFolderName = IFolderEditText::Probe(view2);
    mFolderName->SetFolder(this);
    mFolderName->SetOnFocusChangeListener(this);

    // We find out how tall the text view wants to be (it is set to wrap_content), so that
    // we can allocate the appropriate amount of space for it.
    Int32 measureSpec = IMeasureSpec::UNSPECIFIED;
    mFolderName->Measure(measureSpec, measureSpec);
    mFolderName->GetMeasuredHeight(&mFolderNameHeight);

    // We disable action mode for now since it messes up the view on phones
    mFolderName->SetCustomSelectionActionModeCallback(mActionModeCallback);
    mFolderName->SetOnEditorActionListener(this);
    mFolderName->SetSelectAllOnFocus(TRUE);

    Int32 type;
    mFolderName->GetInputType(&type);
    return mFolderName->SetInputType(type |
            IInputType::TYPE_TEXT_FLAG_NO_SUGGESTIONS | IInputType::TYPE_TEXT_FLAG_CAP_WORDS);
}

ECode Folder::OnClick(
    /* [in] */ IView* v)
{
    AutoPtr<IInterface> tag;
    v->GetTag((IInterface**)&tag);
    if (IShortcutInfo::Probe(tag) != NULL) {
        // refactor this code from Folder
        AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(tag);
        AutoPtr<ArrayOf<Int32> > pos = ArrayOf<Int32>::Alloc(2);
        v->GetLocationOnScreen(pos);

        Int32 width;
        v->GetWidth(&width);
        Int32 height;
        v->GetHeight(&height);
        AutoPtr<IRect> rect;
        CRect::New((*pos)[0], (*pos)[1], (*pos)[0] + width, (*pos)[1] + height, (IRect**)&rect);
        AutoPtr<IIntent> intent;
        item->GetIntent((IIntent**)&intent);
        intent->SetSourceBounds(rect);

        return mLauncher->StartActivitySafely(v, item.intent, item);
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
        AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(tag);
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
        item->GetCellX((&(*mEmptyCell)[0]));
        item->GetCellY((&(*mEmptyCell)[1]));
        mCurrentDragView = v;

        mContent->RemoveView(mCurrentDragView);
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
    mFolderName->SetHint(String(""));
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
    mFolderName->SetHint(sHintText);
    // Convert to a string here to ensure that no other state associated with the text field
    // gets saved.
    AutoPtr<ICharSequence> text;
    mFolderName->GetText((ICharSequence**)&text);
    String newTitle;
    text->ToString(&newTitle);
    mInfo->SetTitle(newTitle);
    LauncherModel::UpdateItemInDatabase(mLauncher, mInfo);

    if (commit) {
        AutoPtr<IContext> context;
        GetContext((IContext**)&context);
        String str;
        context->GetString(
            Elastos::Droid::Launcher2::R::string::folder_renamed,
            &str);
        SendCustomAccessibilityEvent(IAccessibilityEvent::TYPE_WINDOW_STATE_CHANGED,
                String::Format(str, newTitle));
    }
    // In order to clear the focus from the text field, we set the focus on ourself. This
    // ensures that every time the field is clicked, focus is gained, giving reliable behavior.
    RequestFocus();

    Selection->SetSelection(ISpannable::Probe(text), 0, 0);
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

    *view = mFolderName;
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
        AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(obj);
        Int32 cellX;
        item->GetCellX(&cellX);
        if (cellX > maxX) {
            maxX = cellX;
        }
    }

    AutoPtr<IComparator> gridComparator = new GridComparator(maxX + 1);
    Collections::Sort(items, gridComparator);
    Int32 countX;
    mContent->GetCountX(&countX);
    for (Int32 i = 0; i < count; i++) {
        Int32 x = i % countX;
        Int32 y = i / countX;
        AutoPtr<IInterface> obj;
        items->Get(i, (IInterface**)&obj);
        AutoPtr<IShortcutInfo> item = IShortcutInfo::Probe(obj);
        item->SetCellX(x);
        item->SetCellY(y);
    }
}

ECode Folder::Bind(
    /* [in] */ IFolderInfo* info)
{
    mInfo = info;
    AutoPtr<IArrayList> children;
    info->GetContents((IArrayList**)&children);
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
        if (!CreateAndAddShortcut(child)) {
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
        LauncherModel::DeleteItemFromDatabase(mLauncher, item);
    }

    mItemsInvalidated = TRUE;
    UpdateTextViewFocus();
    mInfo->AddListener(this);

    AutoPtr<ICharSequence> cstr;
    mInfo->GetTitle((ICharSequence**)&cstr);
    String str;
    cstr->ToString(&str);
    Boolean res;
    sDefaultFolderName->ContentEquals(str, &res);
    if (!res) {
        mFolderName->SetText(str);
    }
    else {
        mFolderName->SetText(String(""));
    }
    return UpdateItemLocationsInDatabase();
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
    if (IDragLayer::Probe(res) == NULL)) return;
    SetScaleX(0.8f);
    SetScaleY(0.8f);
    SetAlpha(0f);
    mState = STATE_SMALL;
    return;
}

ECode Folder::AnimateOpen()
{
    PositionAndSizeAsIcon();

    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IDragLayer::Probe(res) == NULL)) return;
    CenterAboutIcon();
    AutoPtr<IPropertyValuesHolder> alpha;
    PropertyValuesHolder::OfFloat(SString("alpha"), 1, (IPropertyValuesHolder**)&alpha);
    AutoPtr<IPropertyValuesHolder> scaleX;
    PropertyValuesHolder::OfFloat(SString("scaleX"), 1.0f, (IPropertyValuesHolder**)&scaleX);
    AutoPtr<IPropertyValuesHolder> scaleY;
    PropertyValuesHolder::OfFloat(SString("scaleY"), 1.0f, (IPropertyValuesHolder**)&scaleY);
    AutoPtr<IObjectAnimator> oa;
    LauncherAnimUtils::OfPropertyValuesHolder(this, alpha, scaleX, scaleY, (IObjectAnimator**)&oa);

    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter(this);
    IAnimator::Probe(oa)->AddListener(listener);
    oa->SetDuration(mExpandDuration);
    SetLayerType(LAYER_TYPE_HARDWARE, null);
    return oa->Start();
}

void Folder::SendCustomAccessibilityEvent(
    /* [in] */ Int32 type,
    /* [in] */ const String& text);
{

    AutoPtr<IContext> context;
    GetContext((IContext**)&context);
    AutoPtr<IInterface> obj;
    context->GetSystemService(IContext::ACCESSIBILITY_SERVICE, (IInterface**)&obj);
    AutoPtr<IAccessibilityManagerAutoPtr> accessibilityManager =
            IAccessibilityManagerAutoPtr::Probe(obj);

    Boolean res;
    accessibilityManager->IsEnabled(&res);
    if (res) {
        AutoPtr<IAccessibilityEvent> event;
        AccessibilityEvent::Obtain(type, (IAccessibilityEvent**)&event);
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
        firstChild->RequestFocus();
    }
}

ECode Folder::AnimateClosed()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    if (IDragLayer::Probe(res) == NULL)) return;

    AutoPtr<IPropertyValuesHolder> alpha;
    PropertyValuesHolder::OfFloat(String("alpha"), 0), (IPropertyValuesHolder**)&alpha);
    AutoPtr<IPropertyValuesHolder> scaleX;
    PropertyValuesHolder::OfFloat(String("scaleX"), 0.9f, (IPropertyValuesHolder**)&scaleX);
    AutoPtr<IPropertyValuesHolder> scaleY;
    PropertyValuesHolder::OfFloat(String("scaleY"), 0.9f, (IPropertyValuesHolder**)&scaleY);
    AutoPtr<IObjectAnimator> o;
    LauncherAnimUtils::OfPropertyValuesHolder(this, alpha, scaleX, scaleY, (IObjectAnimator**)&oa);

    AutoPtr<IAnimatorListener> listener = new MyAnimatorListenerAdapter2(this);
    IAnimator::Probe(oa)->AddListener(listener);
    oa->SetDuration(mExpandDuration);
    SetLayerType(LAYER_TYPE_HARDWARE, null);
    return oa->Start();
}

ECode Folder::NotifyDataSetChanged()
{
    // recreate all the children if the data set changes under us. We may want to do this more
    // intelligently (ie just removing the views that should no longer exist)
    mContent->RemoveAllViewsInLayout();
    return Bind(mInfo);
}

ECode Folder::AcceptDrop(
    /* [in] */ DragObject* d,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<IItemInfo> item = IItemInfo::Probe(d->mDragInfo);
    Int32 itemType;
    item->GetItemType(&itemType);
    Boolean res;
    IsFull(&res);
    *result = ((itemType == LauncherSettings::Favorites::ITEM_TYPE_APPLICATION ||
            itemType == LauncherSettings::Favorites::ITEM_TYPE_SHORTCUT) &&
            !res);
    return NOERROR;
}

ECode Folder::FindAndSetEmptyCells(
    /* [in] */ IShortcutInfo* item,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    AutoPtr<ArrayOf<Int32> > emptyCell = ArrayOf<Int32>::Alloc(2);
    Int32 spanX;
    item->GetSpanX(&spanX);
    Int32 spanY;
    item->GetSpanY(&spanY);
    Boolean res;
    mContent->FindCellForSpan(emptyCell, spanX, spanY, &res);
    if (res) {
        item->SetCellX((*emptyCell)[0]);
        item->SetCellY((*emptyCell)[1]);
        *result = TRUE;
        return NOERROR;
    } else {
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
    AutoPtr<IBitmap> map;
    item->GetIcon(mIconCache, (IBitmap**)&map);
    AutoPtr<IFastBitmapDrawable> drawable;
    CFastBitmapDrawable::New(map, (IFastBitmapDrawable**)&drawable);
    textView->SetCompoundDrawablesWithIntrinsicBounds(NULL,
            drawable, NULL, NULL);
    textView->SetText(_item->mTitle);
    if (_item->mContentDescription != NULL) {
        textView->SetContentDescription(item.contentDescription);
    }
    textView->SetTag(item);

    textView->SetOnClickListener(this);
    textView->SetOnLongClickListener(this);

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

    AutoPtr<ICellLayoutLayoutParams> lp =
        new CellLayout::LayoutParams(_item->mCellX, _item->mCellY, _item->mSpanX, _item->mSpanY);
    Boolean insert = FALSE;
    textView->SetOnKeyListener(new FolderKeyEventListener());
    mContent->AddViewToCellLayout(textView, insert ? 0 : -1, (Int32)item->mId, lp, TRUE);
    *result = TRUE;
    return NOERROR;
}

ECode Folder::OnDragEnter(
    /* [in] */ DragObject* d)
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
    /* [in] */ DragObject* d)
{
    AutoPtr<ArrayOf<Float> > r = GetDragViewVisualCenter(d->mX, d->mY, d->mXOffset,
            d->mYOffset, d->mDragView, NULL);
    mTargetCell = mContent->FindNearestArea((Int32)(*r)[0], (Int32)(*r)[1], 1, 1, mTargetCell);

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
    } else {
        res = recycle;
    }

    // These represent the visual top and left of drag view if a dragRect was provided.
    // If a dragRect was not provided, then they correspond to the actual view left and
    // top, as the dragRect is in that case taken to be the entire dragView.
    // R.dimen.dragViewOffsetY.
    Int32 left = x - xOffset;
    Int32 top = y - yOffset;

    // In order to find the visual center, we shift by half the dragRect
    AutoPtr<IRect> rect = dragView->GetDragRegion();
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
    /* [in] */ DragObject* d)
{
    // We only close the folder if this is a true drag exit, ie. not because a drop
    // has occurred above the folder.
    if (!d->mDragComplete) {
        mOnExitAlarm->SetOnAlarmListener(mOnExitAlarmListener);
        mOnExitAlarm->SetAlarm(ON_EXIT_CLOSE_DELAY);
    }
    return mReorderAlarm->CancelAlarm();
}

ECode Folder::OnDropCompleted(
    /* [in] */ IView* target,
    /* [in] */ DragObject* d,
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
        SetupContentForNumItems(&count);
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
    return UpdateItemLocationsInDatabase();
}

ECode Folder::SupportsFlingToDelete(
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result);

    *result = TRUE;
    return NOERROR;
}

ECode Folder::OnFlingToDelete(
    /* [in] */ DragObject* d,
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
        AutoPtr<IItemInfo> info = IItemInfo::Probe(obj2);
        Int32 id;
        mInfo->GetId(&id);
        Int32 x;
        info->GetCellX(&x);
        Int32 y;
        info->GetCellY(&y);
        LauncherModel::MoveItemInDatabase(mLauncher, info, id, 0, x, y);
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
    /* [in] */ DragObject* d,
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
            countY = Math::Max(0, countY - 1);
        }
        else if ((countX - 1) * countY >= count) {
            countX = Math::Max(0, countX - 1);
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

    Int32 left;
    GetPaddingLeft(&left);
    Int32 right;
    GetPaddingRight(&right);
    Int32 desiredWidth;
    mContent->GetDesiredWidth(&desiredWidth);
    Int32 width = left + right + desiredWidth;

    Int32 top;
    getPaddingTop(&top);
    Int32 bottom;
    getPaddingBottom(&bottom);
    Int32 desiredHeight;
    mContent->GetDesiredHeight(&desiredHeight);
    Int32 height = top + bottom + desiredHeight + mFolderNameHeight;

    AutoPtr<IView> view;
    mLauncher->FindViewById(
            Elastos::Droid::Launcher2::R::id::drag_layer,
            (IView**)&view);
    AutoPtr<IDragLayer> parent = IDragLayer::Probe(view);
    Float scale;
    parent->GetDescendantRectRelativeToSelf(mFolderIcon, mTempRect, &scale);

    Int32 left2;
    mTempRect->GetLeft(&left2);
    Int32 width2;
    mTempRect->GetWidth(&width2);
    Int32 centerX = (Int32)(left2 + width2 * scale / 2);
    Int32 top;
    mTempRect->GetTop(&top);
    Int32 height2;
    mTempRect->Getheight(&height2);
    Int32 centerY = (Int32)(top + height2 * scale / 2);
    Int32 centeredLeft = centerX - width / 2;
    Int32 centeredTop = centerY - height / 2;

    AutoPtr<IWorkspace> workspace;
    mLauncher->GetWorkspace((IWorkspace**)&workspace);
    Int32 currentPage;
    workspace->GetCurrentPage(&currentPage);
    // In case the workspace is scrolling, we need to use the final scroll to compute
    // the folders bounds.
    workspace->SetFinalScrollForPageChange(currentPage);
    // We first fetch the currently visible CellLayoutChildren
    AutoPtr<IView> view;
    workspace->GetChildAt(currentPage, (IView**)&view);
    AutoPtr<ICellLayout> currentLayout = ICellLayout::Probe(view);
    AutoPtr<IShortcutAndWidgetContainer> boundingLayout;
    currentLayout->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&boundingLayout);
    AutoPtr<IRect> bounds;
    CRect::New((IRect**)&bounds);
    parent->GetDescendantRectRelativeToSelf(boundingLayout, bounds);
    // We reset the workspaces scroll
    workspace->ResetFinalScrollForPageChange(currentPage);

    // We need to bound the folder to the currently visible CellLayoutChildren

    Int32 left3;
    bounds->GetLeft(&left3);
    Int32 width3;
    bounds->GetWidth(&width3);
    Int32 left = Math::Min(Math::Max(left3, centeredLeft),
            left3 + width3 - width);

    Int32 top3;
    bounds->GetTop(&top3);
    Int32 height3;
    bounds->GetHeight(&height3);
    Int32 top = Math::Min(Math::Max(top3, centeredTop),
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
    mFolderIcon->GetMeasuredWidth(&measuredWidth);
    mFolderIconPivotX = (Int32)(measuredWidth *
            (1.0f * folderPivotX / width));
    Int32 measuredHeight;
    mFolderIcon->GetMeasuredHeight(&measuredHeight);
    mFolderIconPivotY = (Int32)(measuredHeight *
            (1.0f * folderPivotY / height));

    lp->SetWidth(width);
    lp->SetHeight(height);
    lp->SetX(left);
    lp->SetY(top);
}

ECode Folder::GetPivotXForIconAnimation(
    /* [out] */ Float* res);
{
    VALIDATE_NOT_NULL(res);

    *res = mFolderIconPivotX;
    return NOERROR;
}

ECode Folder::GetPivotYForIconAnimation(
    /* [out] */ Float* res);
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
        lp = new DragLayer::LayoutParams(0, 0);
        lp->SetCustomPosition(TRUE);
        SetLayoutParams(lp);
    }
    CenterAboutIcon();
}

ECode Folder::OnMeasure(
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
            IMeasureSpec::EXACTLY);
    Int32 contentHeightSpec = MeasureSpec::MakeMeasureSpec(desiredHeight,
            IMeasureSpec::EXACTLY);
    mContent->Measure(contentWidthSpec, contentHeightSpec);

    mFolderName->Measure(contentWidthSpec,
            MeasureSpec::MakeMeasureSpec(mFolderNameHeight, IMeasureSpec::EXACTLY));
    return SetMeasuredDimension(width, height);
}

void Folder::ArrangeChildren(
    /* [in] */ IArrayList* list)
{
    AutoPtr<ArrayOf<Int32> > vacant = ArrayOf<Int32>::Alloc(2);
    if (list == NULL) {
        list = GetItemsInReadingOrder();
    }
    mContent->RemoveAllViews();

    Int32 size;
    list->GetSize(&size);
    for (Int32 i = 0; i < size; i++) {
        AutoPtr<IInterface> obj;
        list->Get(i, (IInterface**)&obj);
        AutoPtr<IView> v = IView::Probe(obj);
        mContent->GetVacantCell(vacant, 1, 1);
        AutoPtr<IViewGroupLayoutParams> res;
        v->GetLayoutParams((IViewGroupLayoutParams**)&res);
        AutoPtr<ICellLayoutLayoutParams> lp = ICellLayoutLayoutParams::Probe(res);
        lp->SetCellX((*vacant)[0]);
        lp->SetCellY((*vacant)[1]);
        AutoPtr<IInterface> obj2;
        v->GetTag((IInterface**)&obj2);
        AutoPtr<IItemInfo> info = IItemInfo::Probe(obj2);
        ItemInfo* _info = (ItemInfo*)info;
        if (_info->mCellX != (*vacant)[0] || _info->mCellY != (*vacant)[1]) {
            _info->mCellX = (*vacant)[0];
            _info->mCellY = (*vacant)[1];
            Int32 id;
            mInfo->GetId(&id);
            LauncherModel::AddOrMoveItemInDatabase(mLauncher, info, id, 0,
                    _info->mCellX, _info->mCellY);
        }
        Boolean insert = FALSE;
        mContent->AddViewToCellLayout(v, insert ? 0 : -1, (Int32)_infoMid, lp, TRUE);
    }
    mItemsInvalidated = TRUE;
}

ECode Folder::GetItemCount(
    /* [out] */ Int32* count)
{
    VALIDATE_NOT_NULL(count);

    AutoPtr<IShortcutAndWidgetContainer> container;
    mContent->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    return container->GetChildCount(count);
}

ECode Folder::GetItemAt(
    /* [in] */ Int32 index,
    /* [out] */ IView** view)
{
    VALIDATE_NOT_NULL(view);

    AutoPtr<IShortcutAndWidgetContainer> container;
    mContent->GetShortcutsAndWidgets((IShortcutAndWidgetContainer**)&container);
    return container->GetChildAt(index, view);
}

void Folder::OnCloseComplete()
{
    AutoPtr<IViewParent> res;
    GetParent((IViewParent**)&res);
    AutoPtr<IDragLayer> parent = IDragLayer::Probe(res);
    if (parent != NULL) {
        parent->RemoveView(this);
    }
    mDragController->RemoveDropTarget(IDropTarget::Probe(this));
    ClearFocus();
    mFolderIcon->RequestFocus();

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
    return NOERROR;
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
        mFolderName->SetNextFocusDownId(id);
        mFolderName->SetNextFocusRightId(id);
        mFolderName->SetNextFocusLeftId(id);
        mFolderName->SetNextFocusUpId(id);
    }
}

ECode Folder::OnDrop(
        /* [in] */ DragObject* d)
{
    AutoPtr<IShortcutInfo> item;
    if (IApplicationInfo::Probe(d->mDragInf) != NULL) {
        // Came from all apps -- make a copy
        IApplicationInfo::Probe(d->mDragInfo)->MakeShortcut((IShortcutInfo**)&item);
        item->SetSpanX(1);
        item->SetSpanY(1);
    }
    else {
        item = IShortcutInfo::Probe(d->mDragInfo);
    }
    // Dragged from self onto self, currently this is the only path possible, however
    // we keep this as a distinct code path.
    if (TO_IINTERFACE(item) == TO_IINTERFACE(mCurrentDragInfo)) {
        AutoPtr<IInterface> obj;
        mCurrentDragView->GetTag((IInterface**)&obj);
        AutoPtr<IShortcutInfo> si = IShortcutInfo::Probe(obj);

        AutoPtr<IViewGroupLayoutParams> res;
        mCurrentDragView->GetLayoutParams((IViewGroupLayoutParams**)&res);
        AutoPtr<ICellLayoutLayoutParams> lp = ICellLayoutLayoutParams::Probe(res);
        lp->SetCellX((*mEmptyCell)[0]);
        lp->SetCellY((*mEmptyCell)[1);
        si->SetCellX((*mEmptyCell)[0]);
        si->SetCellX((*mEmptyCell)[1]);
        Int32 id;
        item->GetId(&id);
        mContent->AddViewToCellLayout(mCurrentDragView, -1, (Int32)id, lp, TRUE);

        Boolean res;
        d->mDragView->HasDrawn(&res);
        if (res) {
            AutoPtr<IDragLayer> dragLayer;
            mLauncher->GetDragLayer((IDragLayer**)&dragLayer);
            dragLayer->AnimateViewIntoPosition(d->mDragView, mCurrentDragView);
        }
        else {
            d->mDeferDragViewCleanupPostAnimation = FALSE;
            mCurrentDragView->SetVisibility(VISIBLE);
        }
        mItemsInvalidated = TRUE;
        SetupContentDimensions(getItemCount());
        mSuppressOnAdd = TRUE;
    }
    return mInfo->Add(item);
}

ECode Folder::HideItem(
    /* [in] */ IShortcutInfo* info)
{
    AutoPtr<IView> v;
    GetViewForInfo(info, (IView**)&v);
    return v->SetVisibility(INVISIBLE);
}

ECode Folder::ShowItem(
    /* [in] */ IShortcutInfo* info)
{
    AutoPtr<IView> v;
    GetViewForInfo(info, (IView**)&v);
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
        getItemCount(&count);
        SetupContentForNumItems(count + 1);
        FindAndSetEmptyCells(item);
    }
    CreateAndAddShortcut(item);
    Int32 id;
    mInfo->GetId(&id);
    Int32 x;
    item->GetCellX(&x);
    Int32 y;
    item->GetCellY(&y);
    return LauncherModel::AddOrMoveItemInDatabase(mLauncher, item, id, 0, x, y);
}

ECode Folder::OnRemove(
    /* [in] */ IShortcutInfo* item)
{
    mItemsInvalidated = TRUE;
    // If this item is being dragged from this open folder, we have already handled
    // the work associated with removing the item, so we don't have to do anything here.
    if (TO_IINTERFACE(item) == TO_IINTERFACE(mCurrentDragInfo)) return NOERROR;
    AutoPtr<IView> v;
    GetViewForInfo(item, (IView**)&v);
    mContent->RemoveView(v);
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
            if (TO_IINTERFCE(obj) == TO_IINTERFCE(item)) {
                return v;
            }
        }
    }
    return NULL;
}

ECode Folder::OnItemsChanged()
{
    return UpdateTextViewFocus();
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
                    mItemsInReadingOrder->Add(TO_IINTERFCE(v));
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
    return dragLayer->GetLocationInDragLayer(this, loc);
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