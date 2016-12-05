
#include "LoadImageTask.h"
#include "CActivityOne.h"
#include "R.h"
#include <Elastos.Droid.Os.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.Droid.Widget.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Net.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/utility/logging/Logger.h>
#include <utils/CallStack.h>

using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::IBitmapFactory;
using Elastos::Droid::Graphics::CBitmapFactory;
using Elastos::Droid::App::IDialog;
using Elastos::Droid::App::IAlertDialog;
using Elastos::Droid::App::CProgressDialog;
using Elastos::Droid::Content::IDialogInterface;
using Elastos::Droid::Widget::IImageView;
using Elastos::Utility::Logging::Logger;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::IInteger32;
using Elastos::IO::ICloseable;
using Elastos::IO::IInputStream;
using Elastos::Net::IURL;
using Elastos::Net::CURL;

namespace Elastos {
namespace DevSamples {
namespace AsyncTaskDemo {

LoadImageTask::LoadImageTask(
    /* [in] */ CActivityOne* host,
    /* [in] */ IContext* context)
    : mHost(host)
{
    CProgressDialog::New(context, (IProgressDialog**)&mProgressBar);
    IDialog::Probe(mProgressBar)->SetCancelable(TRUE);
    IAlertDialog::Probe(mProgressBar)->SetMessage(CoreUtils::Convert("Image downloading ..."));
    mProgressBar->SetProgressStyle(IProgressDialog::STYLE_HORIZONTAL);
    mProgressBar->SetMax(100);
}

LoadImageTask::~LoadImageTask()
{
    Logger::I("Load thread", " >> ~LoadImageTask");
}

ECode LoadImageTask::DoInBackground(
    /* [in] */ ArrayOf<IInterface*>* params,
    /* [out] */ IInterface** result)
{
    VALIDATE_NOT_NULL(result)

    Logger::I("Load thread", " >> doInBackground()");

    assert(params && params->GetLength() == 1);
    String imageUrl = Object::ToString((*params)[0]);

    AutoPtr< ArrayOf<IInterface*> > info = ArrayOf<IInterface*>::Alloc(1);
    info->Set(0, CoreUtils::Convert(10));
    PublishProgress(info);
    Thread::Sleep(1000);

    AutoPtr<IBitmapFactory> fact;
    CBitmapFactory::AcquireSingleton((IBitmapFactory**)&fact);

    AutoPtr<IBitmap> bitmap;

    // AutoPtr<IURL> urlObj;
    // CURL::New(imageUrl, (IURL**)&urlObj);

    // AutoPtr<IInputStream> in;
    // urlObj->OpenStream((IInputStream**)&in);

    // info->Set(0, CoreUtils::Convert(60));
    // PublishProgress(info);
    // Thread::Sleep(1000);

    // fact->DecodeStream(in, (IBitmap**)&bitmap);
    // ICloseable::Probe(in)->Close();

    AutoPtr<IResources> res;
    mHost->GetResources((IResources**)&res);
    fact->DecodeResource(res, R::drawable::leaf, (IBitmap**)&bitmap);
    Thread::Sleep(5000);

    info->Set(0, CoreUtils::Convert(100));
    PublishProgress(info);

    *result = bitmap;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

ECode LoadImageTask::OnPreExecute()
{
    mProgressBar->SetProgress(0);
    IAlertDialog::Probe(mProgressBar)->SetMessage(CoreUtils::Convert("Start Image downloading ..."));
    IDialog::Probe(mProgressBar)->Show();

    Logger::I("UI thread", " >> onPreExecute()");
    return NOERROR;
}

ECode LoadImageTask::OnPostExecute(
        /* [in] */ IInterface* result)
{
    Logger::I("UI thread", " >> onPostExecute()");
    if (result != NULL) {
        IAlertDialog::Probe(mProgressBar)->SetMessage(CoreUtils::Convert("Image downloading success!"));
        mHost->mImageView->SetImageBitmap(IBitmap::Probe(result));
    }
    else {
        IAlertDialog::Probe(mProgressBar)->SetMessage(CoreUtils::Convert("Image downloading failure!"));
    }

    IDialogInterface::Probe(mProgressBar)->Dismiss();
    return NOERROR;
}

ECode LoadImageTask::OnProgressUpdate(
    /* [in] */ ArrayOf<IInterface*>* values)
{
    Logger::I("UI thread", " >> OnProgressUpdate()");

    assert(values && values->GetLength() == 1);
    AutoPtr<IInteger32> iobj = IInteger32::Probe((*values)[0]);
    Int32 ival;
    iobj->GetValue(&ival);

    StringBuilder sb("Image downloading ...");
    sb += ival;
    String info = sb.ToString();
    Logger::I("UI thread", info);
    IAlertDialog::Probe(mProgressBar)->SetMessage(CoreUtils::Convert(info));
    mProgressBar->SetProgress(ival);
    return NOERROR;
}


} // namespace AsyncTaskDemo
} // namespace DevSamples
} // namespace Elastos