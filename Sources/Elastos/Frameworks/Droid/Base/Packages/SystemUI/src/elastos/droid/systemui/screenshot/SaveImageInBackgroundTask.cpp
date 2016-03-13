#include "elastos/droid/systemui/screenshot/SaveImageInBackgroundTask.h"
#include "Elastos.Droid.Graphics.h"
#include "Elastos.Droid.Net.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.Provider.h"
#include "Elastos.Droid.Text.h"
#include "Elastos.CoreLibrary.Core.h"
#include "Elastos.CoreLibrary.Text.h"
#include "Elastos.CoreLibrary.Utility.h"
#include "elastos/droid/systemui/screenshot/GlobalScreenshot.h"
#include "elastos/droid/os/Environment.h"
#include "elastos/droid/os/Process.h"
#include "elastos/droid/R.h"
#include "../R.h"
#include <elastos/core/StringUtils.h>

using Elastos::Droid::App::CNotificationBigPictureStyle;
using Elastos::Droid::App::CNotificationBuilder;
using Elastos::Droid::App::CPendingIntentHelper;
using Elastos::Droid::App::INotification;
using Elastos::Droid::App::INotificationBigPictureStyle;
using Elastos::Droid::App::INotificationStyle;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::App::IPendingIntentHelper;
using Elastos::Droid::Content::CContentValues;
using Elastos::Droid::Content::CIntent;
using Elastos::Droid::Content::CIntentHelper;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IIntentHelper;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::BitmapCompressFormat_PNG;
using Elastos::Droid::Graphics::BitmapConfig;
using Elastos::Droid::Graphics::CBitmapHelper;
using Elastos::Droid::Graphics::CCanvas;
using Elastos::Droid::Graphics::CColorMatrix;
using Elastos::Droid::Graphics::CColorMatrixColorFilter;
using Elastos::Droid::Graphics::CMatrix;
using Elastos::Droid::Graphics::CPaint;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapHelper;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IColorFilter;
using Elastos::Droid::Graphics::IColorMatrix;
using Elastos::Droid::Graphics::IMatrix;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Environment;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::IProcess;
using Elastos::Droid::Os::Process;
using Elastos::Droid::Provider::CMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreImagesImageColumns;
using Elastos::Droid::Provider::IMediaStoreImagesMedia;
using Elastos::Droid::Provider::IMediaStoreMediaColumns;
using Elastos::Droid::R;
using Elastos::Core::CString;
using Elastos::Core::CSystem;
using Elastos::Core::ICharSequence;
using Elastos::Core::ISystem;
using Elastos::Core::StringUtils;
using Elastos::IO::CFile;
using Elastos::IO::ICloseable;
using Elastos::IO::IFlushable;
using Elastos::IO::IOutputStream;
using Elastos::Text::CDateFormatHelper;
using Elastos::Text::CSimpleDateFormat;
using Elastos::Text::IDateFormat;
using Elastos::Text::IDateFormatHelper;
using Elastos::Text::ISimpleDateFormat;
using Elastos::Utility::CDate;
using Elastos::Utility::IDate;


namespace Elastos {
namespace Droid {
namespace SystemUI {
namespace Screenshot {

const String SaveImageInBackgroundTask::TAG("SaveImageInBackgroundTask");
const String SaveImageInBackgroundTask::SCREENSHOTS_DIR_NAME("Screenshots");
const String SaveImageInBackgroundTask::SCREENSHOT_FILE_NAME_TEMPLATE("Screenshot_%s.png");
const String SaveImageInBackgroundTask::SCREENSHOT_SHARE_SUBJECT_TEMPLATE("Screenshot (%s)");
Boolean SaveImageInBackgroundTask::sTickerAddSpace;

SaveImageInBackgroundTask::SaveImageInBackgroundTask(
    /* [in] */ IContext* context,
    /* [in] */ SaveImageInBackgroundData* data,
    /* [in] */ INotificationManager* nManager,
    /* [in] */ Int32 nId)
{
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);

    // Prepare all the output metadata
    AutoPtr<ISystem> sys;
    CSystem::AcquireSingleton((ISystem**)&sys);
    sys->GetCurrentTimeMillis(&mImageTime);

    AutoPtr<ISimpleDateFormat> sdf;
    CSimpleDateFormat::New(String("yyyy-MM-dd-HH-mm-ss"), (ISimpleDateFormat**)&sdf);
    AutoPtr<IDate> d;
    CDate::New(mImageTime, (IDate**)&d);
    String imageDate;
    IDateFormat::Probe(sdf)->Format(d, &imageDate);

    String ifn;
    ifn.AppendFormat(SCREENSHOT_FILE_NAME_TEMPLATE.string(), imageDate.string());
    mImageFileName = ifn;

    AutoPtr<IFile> file = Environment::GetExternalStoragePublicDirectory(IEnvironment::DIRECTORY_PICTURES);
    CFile::New(file, SCREENSHOTS_DIR_NAME, (IFile**)&mScreenshotDir);

    AutoPtr<IFile> file2;
    CFile::New(mScreenshotDir, mImageFileName, (IFile**)&file2);
    String path;
    file2->GetAbsolutePath(&path);
    mImageFilePath = path;

    // Create the large notification icon
    Int32 width, height;
    data->mImage->GetWidth(&width);
    mImageWidth = width;
    data->mImage->GetHeight(&height);
    mImageHeight = height;
    Int32 iconSize = data->mIconSize;
    Int32 previewWidth = data->mPreviewWidth;
    Int32 previewHeight = data->mPreviewheight;

    // const Int32 shortSide = mImageWidth < mImageHeight ? mImageWidth : mImageHeight;
    AutoPtr<IBitmapHelper> bh;
    CBitmapHelper::AcquireSingleton((IBitmapHelper**)&bh);
    BitmapConfig config;
    data->mImage->GetConfig(&config);
    AutoPtr<IBitmap> preview;
    bh->CreateBitmap(previewWidth, previewHeight, config, (IBitmap**)&preview);
    AutoPtr<ICanvas> c;
    CCanvas::New(preview, (ICanvas**)&c);
    AutoPtr<IPaint> paint;
    CPaint::New((IPaint**)&paint);

    AutoPtr<IColorMatrix> desat;
    CColorMatrix::New((IColorMatrix**)&desat);
    desat->SetSaturation(0.25f);
    AutoPtr<IColorFilter> cf;
    CColorMatrixColorFilter::New(desat, (IColorFilter**)&cf);
    paint->SetColorFilter(cf);
    AutoPtr<IMatrix> matrix;
    CMatrix::New((IMatrix**)&matrix);
    Boolean result;
    matrix->PostTranslate((previewWidth - mImageWidth) / 2,
        (previewHeight - mImageHeight) / 2, &result);
    c->DrawBitmap(data->mImage, matrix, paint);
    c->DrawColor(0x40FFFFFF);
    c->SetBitmap(NULL);

    AutoPtr<IBitmap> croppedIcon;
    bh->CreateScaledBitmap(preview, iconSize, iconSize, TRUE, (IBitmap**)&croppedIcon);

    // Show the intermediate notification
    sTickerAddSpace = !sTickerAddSpace;
    mNotificationId = nId;
    mNotificationManager = nManager;
    Int64 tm;
    sys->GetCurrentTimeMillis(&tm);
    const Int64 now = tm;

    CNotificationBuilder::New(context, (INotificationBuilder**)&mNotificationBuilder);
    String s1;
    r->GetString(R::string::screenshot_saving_ticker, &s1);
    s1 += sTickerAddSpace ? " " : "";
    AutoPtr<ICharSequence> cs;
    CString::New(s1, (ICharSequence**)&cs);
    mNotificationBuilder->SetTicker(cs);
    String s2;
    r->GetString(R::string::screenshot_saving_title, &s2);
    cs = NULL;
    CString::New(s2, (ICharSequence**)&cs);
    mNotificationBuilder->SetContentTitle(cs);
    String s3;
    r->GetString(R::string::screenshot_saving_text, &s3);
    cs = NULL;
    CString::New(s3, (ICharSequence**)&cs);
    mNotificationBuilder->SetContentText(cs);
    mNotificationBuilder->SetSmallIcon(R::drawable::stat_notify_image);
    mNotificationBuilder->SetWhen(now);
    Int32 cc;
    r->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &cc);
    mNotificationBuilder->SetColor(cc);

    CNotificationBigPictureStyle::New((INotificationBigPictureStyle**)&mNotificationStyle);
    mNotificationStyle->BigPicture(preview);
    mNotificationBuilder->SetStyle(INotificationStyle::Probe(mNotificationStyle));

    // For "public" situations we want to show all the same info but
    // omit the actual screenshot image.
    CNotificationBuilder::New(context, (INotificationBuilder**)&mPublicNotificationBuilder);
    String s4;
    r->GetString(R::string::screenshot_saving_title, &s4);
    cs = NULL;
    CString::New(s4, (ICharSequence**)&cs);
    mPublicNotificationBuilder->SetContentTitle(cs);
    String s5;
    r->GetString(R::string::screenshot_saving_text, &s5);
    cs = NULL;
    CString::New(s5, (ICharSequence**)&cs);
    mPublicNotificationBuilder->SetContentText(cs);
    mPublicNotificationBuilder->SetSmallIcon(R::drawable::stat_notify_image);
    mPublicNotificationBuilder->SetCategory(INotification::CATEGORY_PROGRESS);
    mPublicNotificationBuilder->SetWhen(now);
    mPublicNotificationBuilder->SetColor(cc);

    AutoPtr<INotification>  notification;
    mPublicNotificationBuilder->Build((INotification**)&notification);
    mNotificationBuilder->SetPublicVersion(notification);

    AutoPtr<INotification>  n;
    mNotificationBuilder->Build((INotification**)&n);
    Int32 flags;
    n->GetFlags(&flags);
    n->SetFlags(flags |= INotification::FLAG_NO_CLEAR);
    mNotificationManager->Notify(nId, n);

    // On the tablet, the large icon makes the notification appear as if it is clickable (and
    // on small devices, the large icon is not shown) so defer showing the large icon until
    // we compose the final post-save notification below.
    mNotificationBuilder->SetLargeIcon(croppedIcon);
    // But we still don't set it for the expanded view, allowing the smallIcon to show here.
    mNotificationStyle->BigLargeIcon(NULL);
}

ECode SaveImageInBackgroundTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)//SaveImageInBackgroundData
{
    Int32 length = params->GetLength();
    if (length != 1) {
        *result = NULL;
        return NOERROR;
    }
    AutoPtr<ArrayOf<SaveImageInBackgroundData*> > _params = ArrayOf<SaveImageInBackgroundData*>::Alloc(length);
    for (Int32 i = 0; i < length; i++) {
        (*_params)[i] = (SaveImageInBackgroundData*)(IObject::Probe((*params)[i]));
    }
    if (IsCancelled()) {
        (*_params)[0]->ClearImage();
        (*_params)[0]->ClearContext();
        *result = NULL;
        return NOERROR;
    }

    // By default, AsyncTask sets the worker thread to have background thread priority, so bump
    // it back up so that we save a little quicker.
    Process::SetThreadPriority(IProcess::THREAD_PRIORITY_FOREGROUND);

    AutoPtr<IContext> context = (*_params)[0]->mContext;
    AutoPtr<IBitmap> image = (*_params)[0]->mImage;
    AutoPtr<IResources> r;
    context->GetResources((IResources**)&r);

    // try {
    // Create screenshot directory if it doesn't exist
    Boolean succeeded;
    mScreenshotDir->Mkdirs(&succeeded);

    // media provider uses seconds for DATE_MODIFIED and DATE_ADDED, but milliseconds
    // for DATE_TAKEN
    Int64 dateSeconds = mImageTime / 1000;

    // Save the screenshot to the MediaStore
    AutoPtr<IContentValues> values;
    CContentValues::New((IContentValues**)&values);
    AutoPtr<IContentResolver> resolver;
    context->GetContentResolver((IContentResolver**)&resolver);
    values->Put(IMediaStoreMediaColumns::DATA, mImageFilePath);
    values->Put(IMediaStoreMediaColumns::TITLE, mImageFileName);
    values->Put(IMediaStoreMediaColumns::DISPLAY_NAME, mImageFileName);
    values->Put(IMediaStoreImagesImageColumns::DATE_TAKEN, StringUtils::ToString(mImageTime));
    values->Put(IMediaStoreMediaColumns::DATE_ADDED, StringUtils::ToString(dateSeconds));
    values->Put(IMediaStoreMediaColumns::DATE_MODIFIED, StringUtils::ToString(dateSeconds));
    values->Put(IMediaStoreMediaColumns::MIME_TYPE, String("image/png"));
    values->Put(IMediaStoreMediaColumns::WIDTH, StringUtils::ToString(mImageWidth));
    values->Put(IMediaStoreMediaColumns::HEIGHT, StringUtils::ToString(mImageHeight));
    AutoPtr<IMediaStoreImagesMedia> msim;
    CMediaStoreImagesMedia::AcquireSingleton((IMediaStoreImagesMedia**)&msim);
    AutoPtr<IUri> uu;
    msim->GetEXTERNAL_CONTENT_URI((IUri**)&uu);
    AutoPtr<IUri> uri;
    resolver->Insert(uu, values, (IUri**)&uri);

    AutoPtr<IDate> date;
    CDate::New(mImageTime, (IDate**)&date);
    AutoPtr<IDateFormatHelper> dfh;
    CDateFormatHelper::AcquireSingleton((IDateFormatHelper**)&dfh);
    AutoPtr<IDateFormat> df;
    dfh->GetDateTimeInstance((IDateFormat**)&df);
    String subjectDate;
    df->Format(date, &subjectDate);
    String subject;
    subject.AppendFormat(SCREENSHOT_SHARE_SUBJECT_TEMPLATE.string(), subjectDate.string());
    AutoPtr<IIntent> sharingIntent;
    CIntent::New(IIntent::ACTION_SEND, (IIntent**)&sharingIntent);
    sharingIntent->SetType(String("image/png"));
    sharingIntent->PutExtra(IIntent::EXTRA_STREAM, IParcelable::Probe(uri));
    sharingIntent->PutExtra(IIntent::EXTRA_SUBJECT, subject);

    AutoPtr<IIntentHelper> ih;
    CIntentHelper::AcquireSingleton((IIntentHelper**)&ih);
    AutoPtr<IIntent> chooserIntent;
    ih->CreateChooser(sharingIntent, NULL, (IIntent**)&chooserIntent);

    chooserIntent->AddFlags(IIntent::FLAG_ACTIVITY_CLEAR_TASK
        | IIntent::FLAG_ACTIVITY_NEW_TASK);

    String str;
    r->GetString(Elastos::Droid::R::string::share, &str);
    AutoPtr<ICharSequence> cs;
    CString::New(str, (ICharSequence**)&cs);
    AutoPtr<IPendingIntentHelper> pih;
    CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
    AutoPtr<IPendingIntent> pi;
    pih->GetActivity(context, 0, chooserIntent,
        IPendingIntent::FLAG_CANCEL_CURRENT, (IPendingIntent**)&pi);
    mNotificationBuilder->AddAction(R::drawable::ic_menu_share, cs, pi);

    AutoPtr<IOutputStream> out;
    resolver->OpenOutputStream(uri, (IOutputStream**)&out);
    Boolean compress;
    image->Compress(BitmapCompressFormat_PNG, 100, out, &compress);
    IFlushable::Probe(out)->Flush();
    ICloseable::Probe(out)->Close();

    // update file size in the database
    values->Clear();
    AutoPtr<IFile> pf;
    CFile::New(mImageFilePath, (IFile**)&pf);
    Int64 len;
    pf->GetLength(&len);
    values->Put(IMediaStoreMediaColumns::SIZE, len);
    Int32 rowsaffected;
    resolver->Update(uri, values, String(NULL), NULL, &rowsaffected);

    (*_params)[0]->mImageUri = uri;
    (*_params)[0]->mImage = NULL;
    (*_params)[0]->mResult = 0;
    // } catch (Exception e) {
        // IOException/UnsupportedOperationException may be thrown if external storage is not
        // mounted
        // params[0].clearImage();
        // params[0].result = 1;
    // }

    // Recycle the bitmap data
    if (image != NULL) {
        image->Recycle();
    }

    *result = (*_params)[0]->Probe(EIID_IInterface);
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode SaveImageInBackgroundTask::OnPostExecute(
    /* [in] */ IInterface* params)
{
    AutoPtr<SaveImageInBackgroundData> _params = (SaveImageInBackgroundData*)(IObject::Probe(params));
    if (IsCancelled()) {
        _params->mFinisher->Run();
        _params->ClearImage();
        _params->ClearContext();
        return E_NULL_POINTER_EXCEPTION;
    }

    if (_params->mResult > 0) {
        // Show a message that we've failed to save the image to disk
        GlobalScreenshot::NotifyScreenshotError(_params->mContext, mNotificationManager);
    }
    else {
        // Show the final notification to indicate screenshot saved
        AutoPtr<IResources> r;
        _params->mContext->GetResources((IResources**)&r);

        // Create the intent to show the screenshot in gallery
        AutoPtr<IIntent> launchIntent;
        CIntent::New(IIntent::ACTION_VIEW, (IIntent**)&launchIntent);
        launchIntent->SetDataAndType(_params->mImageUri, String("image/png"));
        launchIntent->SetFlags(IIntent::FLAG_ACTIVITY_NEW_TASK);

        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 nw;
        sys->GetCurrentTimeMillis(&nw);
        const Int64 now = nw;

        String s1;
        r->GetString(R::string::screenshot_saved_title, &s1);
        AutoPtr<ICharSequence> cs;
        CString::New(s1, (ICharSequence**)&cs);
        mNotificationBuilder->SetContentTitle(cs);
        String s2;
        r->GetString(R::string::screenshot_saved_text, &s2);
        cs = NULL;
        CString::New(s2, (ICharSequence**)&cs);
        mNotificationBuilder->SetContentText(cs);

        AutoPtr<IPendingIntentHelper> pih;
        CPendingIntentHelper::AcquireSingleton((IPendingIntentHelper**)&pih);
        AutoPtr<IPendingIntent> pi;
        pih->GetActivity(_params->mContext, 0, launchIntent, 0, (IPendingIntent**)&pi);
        mNotificationBuilder->SetContentIntent(pi);
        mNotificationBuilder->SetWhen(now);
        mNotificationBuilder->SetAutoCancel(TRUE);
        Int32 color;
        r->GetColor(Elastos::Droid::R::color::system_notification_accent_color, &color);
        mNotificationBuilder->SetColor(color);

        // Update the text in the public version as well
        String s3;
        r->GetString(R::string::screenshot_saved_title, &s3);
        cs = NULL;
        CString::New(s3, (ICharSequence**)&cs);
        mPublicNotificationBuilder->SetContentTitle(cs);

        String s4;
        r->GetString(R::string::screenshot_saved_text, &s4);
        cs = NULL;
        CString::New(s4, (ICharSequence**)&cs);
        mPublicNotificationBuilder->SetContentText(cs);
        mPublicNotificationBuilder->SetContentIntent(pi);
        mPublicNotificationBuilder->SetWhen(now);
        mPublicNotificationBuilder->SetAutoCancel(TRUE);
        mPublicNotificationBuilder->SetColor(color);

        AutoPtr<INotification> notification;
        mPublicNotificationBuilder->Build((INotification**)&notification);
        mNotificationBuilder->SetPublicVersion(notification);

        AutoPtr<INotification> n;
        mNotificationBuilder->Build((INotification**)&n);

        Int32 flags;
        n->GetFlags(&flags);
        n->SetFlags(flags &= ~INotification::FLAG_NO_CLEAR);

        mNotificationManager->Notify(mNotificationId, n);
    }
    _params->mFinisher->Run();
    _params->ClearContext();
    return NOERROR;
}

} // namespace Screenshot
} // namespace SystemUI
} // namespace Droid
} // namespace Elastos