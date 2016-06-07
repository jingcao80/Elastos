
#include "elastos/droid/server/telecom/ContactsAsyncHelper.h"
#include "R.h"
#include "elastos/droid/server/telecom/Log.h"
#include "elastos/droid/server/telecom/ThreadUtil.h"
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <elastos/droid/graphics/drawable/Drawable.h>
#include <elastos/droid/net/Uri.h>
#include <elastos/droid/os/Looper.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::Drawable::Drawable;;
using Elastos::Droid::Graphics::Drawable::IBitmapDrawable;
using Elastos::Droid::Net::Uri;
using Elastos::Droid::Os::CHandlerThread;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::Looper;
using Elastos::IO::IInputStream;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

//=============================================================================
// ContactsAsyncHelper::SubHandler
//=============================================================================
ContactsAsyncHelper::SubHandler::SubHandler(
    /* [in] */ ILooper* looper)
    : Handler(looper)
{}

ECode ContactsAsyncHelper::SubHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    AutoPtr<WorkerArgs> args = (WorkerArgs*) IObject::Probe(msgObj);
    if (msgArg1 == EVENT_LOAD_IMAGE) {
        if (args->mListener != NULL) {
            Log::D("ContactsAsyncHelper", "Notifying listener: %s image: %s completed", TO_CSTR(args->mListener),
                    TO_CSTR(args->mDisplayPhotoUri));
            args->mListener->OnImageLoadComplete(msgWhat, args->mPhoto, args->mPhotoIcon,
                    args->mCookie);
        }
    }
    return NOERROR;
}

//=============================================================================
// ContactsAsyncHelper::WorkerHandler
//=============================================================================
ECode ContactsAsyncHelper::WorkerHandler::constructor(
    /* [in] */ ILooper* looper)
{
    Handler::constructor(looper);
    return NOERROR;
}

ECode ContactsAsyncHelper::WorkerHandler::HandleMessage(
    /* [in] */ IMessage* msg)
{
    Int32 msgWhat;
    msg->GetWhat(&msgWhat);
    AutoPtr<IInterface> msgObj;
    msg->GetObj((IInterface**)&msgObj);
    Int32 msgArg1;
    msg->GetArg1(&msgArg1);
    AutoPtr<WorkerArgs> args = (WorkerArgs*) IObject::Probe(msgObj);
    if (msgArg1 == EVENT_LOAD_IMAGE) {
        AutoPtr<IInputStream> inputStream;
        // try {
        ECode ec;
        do {
            // try {
            AutoPtr<IContentResolver> contentResolver;
            args->mContext->GetContentResolver((IContentResolver**)&contentResolver);
            ec = contentResolver->OpenInputStream(args->mDisplayPhotoUri, (IInputStream**)&inputStream);
            // } catch (Exception e) {
            if (FAILED(ec)) {
                Log::E("ContactsAsyncHelper", "Error opening photo input stream %d", ec);
            }
            // }
            if (inputStream != NULL) {
                ec = Drawable::CreateFromStream(inputStream,
                        TO_STR(args->mDisplayPhotoUri), (IDrawable**)&args->mPhoto);
                if (FAILED(ec)) break;
                // This assumes Drawable coming from contact database is usually
                // BitmapDrawable and thus we can have (down)scaled version of it.
                ec = GetPhotoIconWhenAppropriate(args->mContext, args->mPhoto, (IBitmap**)&args->mPhotoIcon);
                Log::D("ContactsAsyncHelper", "Loading image: %d token: %d image URI: %s", msgArg1,
                        msgWhat, TO_CSTR(args->mDisplayPhotoUri));
            } else {
                args->mPhoto = NULL;
                args->mPhotoIcon = NULL;
                Log::D("ContactsAsyncHelper", "Problem with image: %d token: %d image URI: %s, using default image.",
                        msgArg1, msgWhat, TO_CSTR(args->mDisplayPhotoUri));
            }
        } while(FALSE);
        // } finally {
        if (inputStream != NULL) {
            // try {
            ec = inputStream->Close();
            // } catch (IOException e) {
            if (FAILED(ec)) {
                if ((ECode) E_IO_EXCEPTION == ec) {
                    Log::E("ContactsAsyncHelper", "Unable to close input stream.");
                }
                else {
                    return ec;
                }
            }
            // }
        }
        if (FAILED(ec)) return ec;
        // }
    }
    // send the reply to the enclosing class.
    AutoPtr<IMessage> reply;
    sResultHandler->ObtainMessage(msgWhat, (IMessage**)&reply);
    reply->SetArg1(msgArg1);
    reply->SetObj(msgObj);
    reply->SendToTarget();
    return NOERROR;
}

ECode ContactsAsyncHelper::WorkerHandler::GetPhotoIconWhenAppropriate(
    /* [in] */ IContext* context,
    /* [in] */ IDrawable* photo,
    /* [out] */ IBitmap** result)
{
    VALIDATE_NOT_NULL(result)

    if (!(IBitmapDrawable::Probe(photo) != NULL)) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<IResources> res;
    context->GetResources((IResources**)&res);
    Int32 iconSize;
    res->GetDimensionPixelSize(R::dimen::notification_icon_size, &iconSize);
    AutoPtr<IBitmap> orgBitmap;
    IBitmapDrawable::Probe(photo)->GetBitmap((IBitmap**)&orgBitmap);
    Int32 orgWidth;
    orgBitmap->GetWidth(&orgWidth);
    Int32 orgHeight;
    orgBitmap->GetHeight(&orgHeight);
    Int32 longerEdge = orgWidth > orgHeight ? orgWidth : orgHeight;
    // We want downscaled one only when the original icon is too big.
    if (longerEdge > iconSize) {
        Float ratio = ((Float) longerEdge) / iconSize;
        Int32 newWidth = (Int32) (orgWidth / ratio);
        Int32 newHeight = (Int32) (orgHeight / ratio);
        // If the longer edge is much longer than the shorter edge, the latter may
        // become 0 which will cause a crash.
        if (newWidth <= 0 || newHeight <= 0) {
            Log::W("ContactsAsyncHelper", "Photo icon's width or height become 0.");
            *result = NULL;
            return NOERROR;
        }
        // It is sure ratio >= 1.0f in any case and thus the newly created Bitmap
        // should be smaller than the original.
        AutoPtr<IBitmapHelper> helper;
        CBitmapHelper::AcquireSingleton((IBitmapHelper**)&helper);
        return helper->CreateScaledBitmap(orgBitmap, newWidth, newHeight, TRUE, result);
    } else {
        *result = orgBitmap;
        REFCOUNT_ADD(*result)
        return NOERROR;
    }
    return NOERROR;
}

//=============================================================================
// ContactsAsyncHelper
//=============================================================================
const String ContactsAsyncHelper::LOG__TAG("ContactsAsyncHelper");
const Int32 ContactsAsyncHelper::EVENT_LOAD_IMAGE = 1;
AutoPtr<IHandler> ContactsAsyncHelper::sResultHandler = new SubHandler(Looper::GetMainLooper());
const AutoPtr<IHandler> ContactsAsyncHelper::sThreadHandler = InitThreadHandler();

ContactsAsyncHelper::ContactsAsyncHelper()
{}

ECode ContactsAsyncHelper::StartObtainPhotoAsync(
    /* [in] */ Int32 token,
    /* [in] */ IContext* context,
    /* [in] */ IUri* displayPhotoUri,
    /* [in] */ IContactsAsyncHelperOnImageLoadCompleteListener* listener,
    /* [in] */ IObject* cookie)
{
    ThreadUtil::CheckOnMainThread();
    // in case the source caller info is null, the URI will be null as well.
    // just update using the placeholder image in this case.
    if (displayPhotoUri == NULL) {
        Log::Wtf(LOG__TAG, "Uri is missing");
        return NOERROR;
    }
    // Added additional Cookie field in the callee to handle arguments
    // sent to the callback function.
    // setup arguments
    AutoPtr<WorkerArgs> args = new WorkerArgs;
    args->mCookie = cookie;
    args->mContext = context;
    args->mDisplayPhotoUri = displayPhotoUri;
    args->mListener = listener;
    // setup message arguments
    AutoPtr<IMessage> msg;
    sThreadHandler->ObtainMessage(token, (IMessage**)&msg);
    msg->SetArg1(EVENT_LOAD_IMAGE);
    msg->SetObj(TO_IINTERFACE(args));
    Log::D(LOG__TAG, "Begin loading image: %s, displaying default image for now.", TO_CSTR(args->mDisplayPhotoUri));
    // notify the thread to begin working
    Boolean b;
    sThreadHandler->SendMessage(msg, &b);
    return NOERROR;
}

AutoPtr<IHandler> ContactsAsyncHelper::InitThreadHandler()
{
    AutoPtr<IHandlerThread> thread;
    CHandlerThread::New(String("ContactsAsyncWorker"), (IHandlerThread**)&thread);
    IThread::Probe(thread)->Start();
    AutoPtr<ILooper> looper;
    thread->GetLooper((ILooper**)&looper);
    AutoPtr<IHandler> rev = new WorkerHandler();
    ((WorkerHandler*) rev.Get())->constructor(looper);
    return rev;
}

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos
