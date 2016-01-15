
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Graphics.h"
#include "elastos/droid/app/CActivityOptions.h"
#include "elastos/droid/app/Activity.h"
#include "elastos/droid/app/CActivityOptionsAnimationStartedListener.h"
#include "elastos/droid/app/ExitTransitionCoordinator.h"
#include "elastos/droid/os/CBundle.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/utility/CPairHelper.h"
#include <elastos/utility/logging/Logger.h>
#include <elastos/core/CoreUtils.h>

using Elastos::Droid::Os::EIID_IRemoteCallback;
using Elastos::Droid::Os::EIID_IBinder;
using Elastos::Droid::Os::CBundle;
using Elastos::Droid::Utility::IPairHelper;
using Elastos::Droid::Utility::CPairHelper;
using Elastos::Core::CoreUtils;
using Elastos::Utility::Logging::Logger;
using Elastos::Utility::CArrayList;

namespace Elastos {
namespace Droid {
namespace App {

//=============================================================================
//              AnimationStartedListenerRunnable
//=============================================================================

AnimationStartedListenerRunnable::AnimationStartedListenerRunnable(
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener)
    : mListener(listener)
{
}

ECode AnimationStartedListenerRunnable::Run()
{
    if (mListener != NULL) {
        mListener->OnAnimationStarted();
    }
    return NOERROR;
}

//=============================================================================
//              CActivityOptions
//=============================================================================
const String CActivityOptions::TAG("CActivityOptions");

/**
 * For Activity transitions, the calling Activity's TransitionListener used to
 * notify the called Activity when the shared element and the exit transitions
 * complete.
 */
const String CActivityOptions::KEY_TRANSITION_COMPLETE_LISTENER("android:transitionCompleteListener");

const String CActivityOptions::KEY_TRANSITION_IS_RETURNING("android:transitionIsReturning");
const String CActivityOptions::KEY_TRANSITION_SHARED_ELEMENTS("android:sharedElementNames");
const String CActivityOptions::KEY_RESULT_DATA("android:resultData");
const String CActivityOptions::KEY_RESULT_CODE("android:resultCode");
const String CActivityOptions::KEY_EXIT_COORDINATOR_INDEX("android:exitCoordinatorIndex");

CAR_INTERFACE_IMPL(CActivityOptions, Object, IActivityOptions)

CAR_OBJECT_IMPL(CActivityOptions)

ECode CActivityOptions::MakeCustomAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 enterResId,
    /* [in] */ Int32 exitResId,
    /* [out] */ IActivityOptions** options)
{
    return MakeCustomAnimation(context, enterResId, exitResId, NULL, NULL, options);
}

ECode CActivityOptions::MakeCustomAnimation(
    /* [in] */ IContext* context,
    /* [in] */ Int32 enterResId,
    /* [in] */ Int32 exitResId,
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options);
    assert(context != NULL);

    String pkgName;
    context->GetPackageName(&pkgName);

    AutoPtr<IActivityOptions> opts;
    CActivityOptions::New((IActivityOptions**)&opts);

    opts->SetPackageName(pkgName);
    opts->SetAnimationType(IActivityOptions::ANIM_CUSTOM);
    opts->SetCustomEnterResId(enterResId);
    opts->SetCustomExitResId(exitResId);
    ((CActivityOptions*)opts.Get())->SetOnAnimationStartedListener(handler, listener);
    *options = opts;
    REFCOUNT_ADD(*options);
    return NOERROR;
}

ECode CActivityOptions::SetOnAnimationStartedListener(
    /* [in] */ IHandler* handler,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener)
{
    if (listener != NULL) {
        mAnimationStartedListener = NULL;
        CActivityOptionsAnimationStartedListener::New(handler, listener, (IRemoteCallback**)&mAnimationStartedListener);
    }
    return NOERROR;
}

ECode CActivityOptions::MakeScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 width,
    /* [in] */ Int32 height,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options);
    assert(source != NULL);

    AutoPtr<IContext> context;
    source->GetContext((IContext**)&context);
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    source->GetLocationOnScreen(temp);
    AutoPtr<IActivityOptions> opts;
    CActivityOptions::New((IActivityOptions**)&opts);
    opts->SetPackageName(pkgName);
    opts->SetAnimationType(IActivityOptions::ANIM_SCALE_UP);
    opts->SetStartX((*temp)[0] + startX);
    opts->SetStartY((*temp)[1] + startY);
    opts->SetWidth(width);
    opts->SetHeight(height);
    *options = opts;
    REFCOUNT_ADD(*options);
    return NOERROR;
}

ECode CActivityOptions::MakeThumbnailScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [out] */ IActivityOptions** options)
{
    return MakeThumbnailScaleUpAnimation(source, thumbnail, startX, startY, NULL, options);
}

ECode CActivityOptions::MakeThumbnailScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return MakeThumbnailAnimation(source, thumbnail, startX, startY, listener, TRUE, options);
}

ECode CActivityOptions::MakeThumbnailScaleDownAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return MakeThumbnailAnimation(source, thumbnail, startX, startY, listener, FALSE, options);
}

ECode CActivityOptions::MakeThumbnailAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [in] */ Boolean scaleUp,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options);
    assert(source != NULL);

    AutoPtr<IContext> context;
    source->GetContext((IContext**)&context);
    String pkgName;
    context->GetPackageName(&pkgName);
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    source->GetLocationOnScreen((ArrayOf<Int32>*)temp);
    AutoPtr<IHandler> handler;
    source->GetHandler((IHandler**)&handler);
    AutoPtr<IActivityOptions> opts;
    CActivityOptions::New((IActivityOptions**)&opts);
    opts->SetPackageName(pkgName);
    opts->SetAnimationType(scaleUp ? IActivityOptions::ANIM_SCALE_UP :IActivityOptions::ANIM_THUMBNAIL_SCALE_DOWN);
    opts->SetThumbnail(thumbnail);
    opts->SetStartX((*temp)[0] + startX);
    opts->SetStartY((*temp)[1] + startY);
    ((CActivityOptions*)opts.Get())->SetOnAnimationStartedListener(handler, listener);
    *options = opts;
    REFCOUNT_ADD(*options);
    return NOERROR;
}

ECode CActivityOptions::MakeThumbnailAspectScaleUpAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return MakeAspectScaledThumbnailAnimation(source, thumbnail, startX, startY,
        targetWidth, targetHeight, listener, TRUE, options);
}

ECode CActivityOptions::MakeThumbnailAspectScaleDownAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [out] */ IActivityOptions** options)
{
    return MakeAspectScaledThumbnailAnimation(source, thumbnail, startX, startY,
            targetWidth, targetHeight, listener, FALSE, options);
}

ECode CActivityOptions::MakeAspectScaledThumbnailAnimation(
    /* [in] */ IView* source,
    /* [in] */ IBitmap* thumbnail,
    /* [in] */ Int32 startX,
    /* [in] */ Int32 startY,
    /* [in] */ Int32 targetWidth,
    /* [in] */ Int32 targetHeight,
    /* [in] */ IActivityOptionsOnAnimationStartedListener* listener,
    /* [in] */ Boolean scaleUp,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    AutoPtr<CActivityOptions> opts;
    CActivityOptions::NewByFriend((CActivityOptions**)&opts);

    AutoPtr<IContext> ctx;
    source->GetContext((IContext**)&ctx);
    ctx->GetPackageName(&opts->mPackageName);

    opts->mAnimationType =
        scaleUp ?
            IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP :
            IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_DOWN;
    opts->mThumbnail = thumbnail;
    AutoPtr< ArrayOf<Int32> > temp = ArrayOf<Int32>::Alloc(2);
    source->GetLocationOnScreen((ArrayOf<Int32>*)temp);
    opts->mStartX = (*temp)[0] + startX;
    opts->mStartY = (*temp)[1] + startY;
    opts->mWidth = targetWidth;
    opts->mHeight = targetHeight;
    AutoPtr<IHandler> h;
    source->GetHandler((IHandler**)&h);
    opts->SetOnAnimationStartedListener(h, listener);
    *options = opts;
    REFCOUNT_ADD(*options)
    return NOERROR;
}

ECode CActivityOptions::MakeSceneTransitionAnimation(
    /* [in] */ IActivity* activity,
    /* [in] */ IView* sharedElement,
    /* [in] */ const String& sharedElementName,
    /* [out] */ IActivityOptions** options)
{
    AutoPtr<ICharSequence> csq = CoreUtils::Convert(sharedElementName);
    AutoPtr<IPairHelper> helper;
    CPairHelper::AcquireSingleton((IPairHelper**)&helper);
    AutoPtr<IPair> pair;
    helper->Create(TO_IINTERFACE(sharedElement), TO_IINTERFACE(csq), (IPair**)&pair);
    AutoPtr< ArrayOf<IPair*> > arrays = ArrayOf<IPair*>::Alloc(1);
    arrays->Set(0, pair);
    return MakeSceneTransitionAnimation(activity, arrays, options);
}

ECode CActivityOptions::MakeSceneTransitionAnimation(
    /* [in] */ IActivity* activity,
    /* [in] */ ArrayOf<IPair*>* sharedElements,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    *options = NULL;

    AutoPtr<CActivityOptions> opts;
    CActivityOptions::NewByFriend((CActivityOptions**)&opts);
    AutoPtr<IWindow> window;
    activity->GetWindow((IWindow**)&window);
    Boolean bval;
    window->HasFeature(IWindow::FEATURE_ACTIVITY_TRANSITIONS, &bval);
    if (!bval) {
        opts->mAnimationType = IActivityOptions::ANIM_DEFAULT;
        *options = (IActivityOptions*)opts;
        return NOERROR;
    }

    opts->mAnimationType = IActivityOptions::ANIM_SCENE_TRANSITION;

    AutoPtr<IArrayList> names, views;
    CArrayList::New((IArrayList**)&names);
    CArrayList::New((IArrayList**)&views);

    if (sharedElements != NULL) {
        for (Int32 i = 0; i < sharedElements->GetLength(); i++) {
            IPair* sharedElement = IPair::Probe((*sharedElements)[i]); //Pair<View, String>
            AutoPtr<IInterface> first, second;
            sharedElement->GetFirst((IInterface**)&first);
            sharedElement->GetSecond((IInterface**)&second);

            String sharedElementName = Object::ToString(second);
            if (sharedElementName.IsNull()) {
                // throw new IllegalArgumentException("Shared element name must not be null");
                Logger::E(TAG, "Shared element name must not be null");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }

            names->Add(second);
            IView* view = IView::Probe(first);
            if (view == NULL) {
                Logger::E(TAG, "Shared element must not be null");
                return E_ILLEGAL_ARGUMENT_EXCEPTION;
            }
            views->Add(first);
        }
    }

    Int32 ival;
    AutoPtr<ExitTransitionCoordinator> exitCoordinator = new ExitTransitionCoordinator();
    exitCoordinator->constructor(activity, names, names, views, FALSE);
    opts->mTransitionReceiver = IResultReceiver::Probe(exitCoordinator);
    opts->mSharedElementNames = names;
    opts->mIsReturning = FALSE;
    opts->mExitCoordinatorIndex =
        ((Activity*)activity)->mActivityTransitionState->AddExitTransitionCoordinator(
            IExitTransitionCoordinator::Probe(exitCoordinator), &ival);
    *options = (IActivityOptions*)opts.Get();
    REFCOUNT_ADD(*options)
    return NOERROR;
}

ECode CActivityOptions::MakeSceneTransitionAnimation(
    /* [in] */ IActivity* activity,
    /* [in] */ IExitTransitionCoordinator* exitCoordinator,
    /* [in] */ IArrayList* sharedElementNames,
    /* [in] */ Int32 resultCode,
    /* [in] */ IIntent* resultData,
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    Int32 ival;
    AutoPtr<CActivityOptions> opts;
    CActivityOptions::NewByFriend((CActivityOptions**)&opts);
    opts->mAnimationType = ANIM_SCENE_TRANSITION;
    opts->mSharedElementNames = sharedElementNames;
    opts->mTransitionReceiver = IResultReceiver::Probe(exitCoordinator);;
    opts->mIsReturning = true;
    opts->mResultCode = resultCode;
    opts->mResultData = resultData;
    opts->mExitCoordinatorIndex =
        ((Activity*)activity)->mActivityTransitionState->AddExitTransitionCoordinator(
            exitCoordinator, &ival);
    *options = (IActivityOptions*)opts.Get();
    REFCOUNT_ADD(*options)
    return NOERROR;
}

ECode CActivityOptions::MakeTaskLaunchBehind(
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    AutoPtr<CActivityOptions> opts;
    CActivityOptions::NewByFriend((CActivityOptions**)&opts);
    opts->mAnimationType = ANIM_LAUNCH_TASK_BEHIND;
    *options = (IActivityOptions*)opts.Get();
    REFCOUNT_ADD(*options)
    return NOERROR;
}

ECode CActivityOptions::GetLaunchTaskBehind(
    /* [out] */ Boolean* bebind)
{
    VALIDATE_NOT_NULL(bebind)
    *bebind = mAnimationType == IActivityOptions::ANIM_LAUNCH_TASK_BEHIND;
    return NOERROR;
}

CActivityOptions::CActivityOptions()
    : mAnimationType(0)
    , mCustomEnterResId(0)
    , mCustomExitResId(0)
    , mStartX(0)
    , mStartY(0)
    , mWidth(0)
    , mHeight(0)
    , mIsReturning(FALSE)
    , mResultCode(0)
    , mExitCoordinatorIndex(0)
{}

CActivityOptions::~CActivityOptions()
{}

ECode CActivityOptions::constructor()
{
    return NOERROR;
}

ECode CActivityOptions::constructor(
    /* [in] */ IBundle* opts)
{
    VALIDATE_NOT_NULL(opts)

    opts->GetString(IActivityOptions::KEY_PACKAGE_NAME, &mPackageName);
    opts->GetInt32(IActivityOptions::KEY_ANIM_TYPE, &mAnimationType);

    if (mAnimationType == IActivityOptions::ANIM_CUSTOM) {
        opts->GetInt32(IActivityOptions::KEY_ANIM_ENTER_RES_ID, 0, &mCustomEnterResId);
        opts->GetInt32(IActivityOptions::KEY_ANIM_EXIT_RES_ID, 0, &mCustomExitResId);

        AutoPtr<IBinder> binder;
        opts->GetIBinder(IActivityOptions::KEY_ANIM_START_LISTENER, (IBinder**)&binder);
        mAnimationStartedListener = IRemoteCallback::Probe(binder);
    }
    else if (mAnimationType == IActivityOptions::ANIM_SCALE_UP) {
        opts->GetInt32(IActivityOptions::KEY_ANIM_START_X, 0, &mStartX);
        opts->GetInt32(IActivityOptions::KEY_ANIM_START_Y, 0, &mStartY);
        opts->GetInt32(IActivityOptions::KEY_ANIM_WIDTH, 0, &mWidth);
        opts->GetInt32(IActivityOptions::KEY_ANIM_HEIGHT, 0, &mHeight);
    }
    else if (mAnimationType == IActivityOptions::ANIM_THUMBNAIL_SCALE_UP
        || mAnimationType == IActivityOptions::ANIM_THUMBNAIL_SCALE_DOWN
        || mAnimationType == IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP
        || mAnimationType == IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_DOWN
    ) {
        AutoPtr<IParcelable> parcelable;
        opts->GetParcelable(IActivityOptions::KEY_ANIM_THUMBNAIL, (IParcelable**)&parcelable);
        mThumbnail = IBitmap::Probe(parcelable);
        opts->GetInt32(IActivityOptions::KEY_ANIM_START_X, 0, &mStartX);
        opts->GetInt32(IActivityOptions::KEY_ANIM_START_Y, 0, &mStartY);
        opts->GetInt32(IActivityOptions::KEY_ANIM_WIDTH, 0, &mWidth);
        opts->GetInt32(IActivityOptions::KEY_ANIM_START_Y, 0, &mHeight);
        AutoPtr<IBinder> binder;
        opts->GetBinder(IActivityOptions::KEY_ANIM_START_LISTENER, (IBinder**)&binder);
        mAnimationStartedListener = IRemoteCallback::Probe(binder);
    }
    else if (mAnimationType == IActivityOptions::ANIM_SCENE_TRANSITION) {
        AutoPtr<IParcelable> parcelable;
        opts->GetParcelable(KEY_TRANSITION_COMPLETE_LISTENER, (IParcelable**)&parcelable);
        mTransitionReceiver = IResultReceiver::Probe(parcelable);

        opts->GetBoolean(KEY_TRANSITION_IS_RETURNING, FALSE, &mIsReturning);
        opts->GetStringArrayList(KEY_TRANSITION_SHARED_ELEMENTS, (IArrayList**)&mSharedElementNames);
        parcelable = NULL;
        opts->GetParcelable(KEY_RESULT_DATA, (IParcelable**)&parcelable);
        mResultData = IIntent::Probe(parcelable);
        opts->GetInt32(KEY_RESULT_CODE, &mResultCode);
        opts->GetInt32(KEY_EXIT_COORDINATOR_INDEX, &mExitCoordinatorIndex);
    }
    return NOERROR;
}

ECode CActivityOptions::GetPackageName(
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName);
    *pkgName = mPackageName;
    return NOERROR;
}

ECode CActivityOptions::SetPackageName(
    /* [in] */ const String& pkgName)
{
    mPackageName = pkgName;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetAnimationType(
    /* [out] */ Int32* type)
{
    VALIDATE_NOT_NULL(type);
    *type = mAnimationType;
    return NOERROR;
}

ECode CActivityOptions::SetAnimationType(
    /* [in] */ Int32 type)
{
    mAnimationType = type;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetCustomEnterResId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mCustomEnterResId;
    return NOERROR;
}

ECode CActivityOptions::SetCustomEnterResId(
    /* [in] */ Int32 id)
{
    mCustomEnterResId = id;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetCustomExitResId(
    /* [out] */ Int32* id)
{
    VALIDATE_NOT_NULL(id);
    *id = mCustomExitResId;
    return NOERROR;
}

ECode CActivityOptions::SetCustomExitResId(
    /* [in] */ Int32 id)
{
    mCustomExitResId = id;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetThumbnail(
    /* [out] */ IBitmap** thumbnail)
{
    VALIDATE_NOT_NULL(thumbnail);
    *thumbnail = mThumbnail;
    REFCOUNT_ADD(*thumbnail);
    return NOERROR;
}

ECode CActivityOptions::SetThumbnail(
    /* [in] */ IBitmap* thumbnail)
{
    mThumbnail = thumbnail;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetStartX(
    /* [out] */ Int32* x)
{
    VALIDATE_NOT_NULL(x);
    *x = mStartX;
    return NOERROR;
}

ECode CActivityOptions::SetStartX(
    /* [in] */ Int32 x)
{
    mStartX = x;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetStartY(
    /* [out] */ Int32* y)
{
    VALIDATE_NOT_NULL(y);
    *y = mStartY;
    return NOERROR;
}

ECode CActivityOptions::SetStartY(
    /* [in] */ Int32 y)
{
    mStartY = y;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetWidth(
    /* [out] */ Int32* width)
{
    VALIDATE_NOT_NULL(width);
    *width = mWidth;
    return NOERROR;
}

ECode CActivityOptions::SetWidth(
    /* [in] */ Int32 width)
{
    mWidth = width;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetHeight(
    /* [out] */ Int32* height)
{
    VALIDATE_NOT_NULL(height);
    *height = mHeight;
    return NOERROR;
}

ECode CActivityOptions::SetHeight(
    /* [in] */ Int32 height)
{
    mHeight = height;
    return NOERROR;
}

/** @hide */
ECode CActivityOptions::GetOnAnimationStartListener(
    /* [out] */ IRemoteCallback** cb)
{
    VALIDATE_NOT_NULL(cb);
    *cb = mAnimationStartedListener;
    REFCOUNT_ADD(*cb);
    return NOERROR;
}

ECode CActivityOptions::SetOnAnimationStartListener(
    /* [in] */ IRemoteCallback* cb)
{
    mAnimationStartedListener = cb;
    return NOERROR;
}

ECode CActivityOptions::GetExitCoordinatorKey(
    /* [out] */ Int32* key)
{
    VALIDATE_NOT_NULL(key)
    *key = mExitCoordinatorIndex;
    return NOERROR;
}

ECode CActivityOptions::Abort()
{
   if (mAnimationStartedListener != NULL) {
        // try {
            mAnimationStartedListener->SendResult(NULL);
        // } catch (RemoteException e) {
        // }
    }
    return NOERROR;
}

ECode CActivityOptions::IsReturning(
    /* [out] */ Boolean* bval)
{
    return mIsReturning;
}

ECode CActivityOptions::GetSharedElementNames(
    /* [out] */ IArrayList** list)
{
    VALIDATE_NOT_NULL(list)
    *list = mSharedElementNames;
    REFCOUNT_ADD(*list)
    return NOERROR;
}

ECode CActivityOptions::GetResultReceiver(
    /* [out] */ IResultReceiver** receiver)
{
    VALIDATE_NOT_NULL(receiver)
    *receiver = mTransitionReceiver;
    REFCOUNT_ADD(*receiver)
    return NOERROR;
}

ECode CActivityOptions::GetResultCode(
    /* [out] */ Int32* code)
{
    VALIDATE_NOT_NULL(code)
    *code = mResultCode;
    return NOERROR;
}

ECode CActivityOptions::GetResultData(
    /* [out] */ IIntent** intent)
{
    VALIDATE_NOT_NULL(intent)
    *intent = mResultData;
    REFCOUNT_ADD(*intent)
    return NOERROR;
}

ECode CActivityOptions::Abort(
    /* [in] */ IBundle* options)
{
    if (options != NULL) {
        AutoPtr<IActivityOptions> opts;
        CActivityOptions::New(options, (IActivityOptions**)&opts);
        opts->Abort();
    }
    return NOERROR;
}

ECode CActivityOptions::Update(
    /* [in] */ IActivityOptions* otherOptions)
{
    VALIDATE_NOT_NULL(otherOptions)
    String pkgName;
    otherOptions->GetPackageName(&pkgName);
    if (!pkgName.IsNull()) {
        mPackageName = pkgName;
    }

    CActivityOptions* other = (CActivityOptions*)otherOptions;

    mTransitionReceiver = NULL;
    mSharedElementNames = NULL;
    mIsReturning = FALSE;
    mResultData = NULL;
    mResultCode = 0;
    mExitCoordinatorIndex = 0;
    mAnimationType = other->mAnimationType;

    switch (other->mAnimationType) {
        case IActivityOptions::ANIM_CUSTOM:
            otherOptions->GetCustomEnterResId(&mCustomEnterResId);
            otherOptions->GetCustomExitResId(&mCustomExitResId);
            mThumbnail = NULL;
            if (mAnimationStartedListener != NULL) {
                // try {
                    mAnimationStartedListener->SendResult(NULL);
                // } catch (RemoteException e) {
                // }
            }
            mAnimationStartedListener = other->mAnimationStartedListener;
            break;

        case IActivityOptions::ANIM_SCALE_UP:
            otherOptions->GetStartX(&mStartX);
            otherOptions->GetStartY(&mStartY);
            otherOptions->GetWidth(&mWidth);
            otherOptions->GetHeight(&mHeight);
            if (mAnimationStartedListener != NULL) {
                // try {
                    mAnimationStartedListener->SendResult(NULL);
                // } catch (RemoteException e) {
                // }
            }
            break;

        case IActivityOptions::ANIM_THUMBNAIL_SCALE_UP:
        case IActivityOptions::ANIM_THUMBNAIL_SCALE_DOWN:
        case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP:
        case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_DOWN:
            otherOptions->GetThumbnail((IBitmap**)&mThumbnail);
            otherOptions->GetStartX(&mStartX);
            otherOptions->GetStartX(&mStartY);
            if (mAnimationStartedListener != NULL) {
                // try {
                    mAnimationStartedListener->SendResult(NULL);
                // } catch (RemoteException e) {
                // }
            }
            mAnimationStartedListener = other->mAnimationStartedListener;
            break;

        case IActivityOptions::ANIM_SCENE_TRANSITION:
            mTransitionReceiver = other->mTransitionReceiver;
            mSharedElementNames = other->mSharedElementNames;
            mIsReturning = other->mIsReturning;
            mThumbnail = NULL;
            mAnimationStartedListener = NULL;
            mResultData = other->mResultData;
            mResultCode = other->mResultCode;
            mExitCoordinatorIndex = other->mExitCoordinatorIndex;
            break;
    }
    return NOERROR;
}

ECode CActivityOptions::ToBundle(
    /* [out] */ IBundle** bundle)
{
    VALIDATE_NOT_NULL(bundle);
    *bundle = NULL;

    if (mAnimationType == IActivityOptions::ANIM_DEFAULT) {
        return NOERROR;
    }

    AutoPtr<IBundle> b;
    CBundle::New((IBundle**)&b);

    if (!mPackageName.IsNull()) {
        b->PutString(IActivityOptions::KEY_PACKAGE_NAME, mPackageName);
    }

    b->PutInt32(IActivityOptions::KEY_ANIM_TYPE, mAnimationType);
    switch (mAnimationType) {
        case IActivityOptions::ANIM_CUSTOM:
            b->PutInt32(IActivityOptions::KEY_ANIM_ENTER_RES_ID, mCustomEnterResId);
            b->PutInt32(IActivityOptions::KEY_ANIM_EXIT_RES_ID, mCustomExitResId);
            b->PutBinder(IActivityOptions::KEY_ANIM_START_LISTENER, IBinder::Probe(mAnimationStartedListener));
            break;

        case IActivityOptions::ANIM_SCALE_UP:
            b->PutInt32(IActivityOptions::KEY_ANIM_START_X, mStartX);
            b->PutInt32(IActivityOptions::KEY_ANIM_START_Y, mStartY);
            b->PutInt32(IActivityOptions::KEY_ANIM_WIDTH, mWidth);
            b->PutInt32(IActivityOptions::KEY_ANIM_HEIGHT, mHeight);
            break;

        case IActivityOptions::ANIM_THUMBNAIL_SCALE_UP:
        case IActivityOptions::ANIM_THUMBNAIL_SCALE_DOWN:
        case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_UP:
        case IActivityOptions::ANIM_THUMBNAIL_ASPECT_SCALE_DOWN:
            b->PutParcelable(IActivityOptions::KEY_ANIM_THUMBNAIL, IParcelable::Probe(mThumbnail));
            b->PutInt32(IActivityOptions::KEY_ANIM_START_X, mStartX);
            b->PutInt32(IActivityOptions::KEY_ANIM_START_Y, mStartY);
            b->PutBinder(IActivityOptions::KEY_ANIM_START_LISTENER,IBinder::Probe(mAnimationStartedListener));
            break;

        case IActivityOptions::ANIM_SCENE_TRANSITION:
            if (mTransitionReceiver != NULL) {
                b->PutParcelable(KEY_TRANSITION_COMPLETE_LISTENER, IParcelable::Probe(mTransitionReceiver));
            }
            b->PutBoolean(KEY_TRANSITION_IS_RETURNING, mIsReturning);
            b->PutStringArrayList(KEY_TRANSITION_SHARED_ELEMENTS, mSharedElementNames);
            b->PutParcelable(KEY_RESULT_DATA, IParcelable::Probe(mResultData));
            b->PutInt32(KEY_RESULT_CODE, mResultCode);
            b->PutInt32(KEY_EXIT_COORDINATOR_INDEX, mExitCoordinatorIndex);
            break;
    }

    *bundle = b;
    REFCOUNT_ADD(*bundle);
    return NOERROR;
}

ECode CActivityOptions::ForTargetActivity(
    /* [out] */ IActivityOptions** options)
{
    VALIDATE_NOT_NULL(options)
    *options = NULL;

    if (mAnimationType == IActivityOptions::ANIM_SCENE_TRANSITION) {
        AutoPtr<IActivityOptions> result;
        CActivityOptions::New((IActivityOptions**)&result);
        result->Update(THIS_PROBE(IActivityOptions));
        *options = result;
        REFCOUNT_ADD(*options)
    }

    return NOERROR;
}

} // namespace App
} // namespace Droid
} // namespace Elastos
