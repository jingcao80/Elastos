
#ifndef __Elastos_DevSamples_AsyncTaskDemo_LoadImageTask_H__
#define __Elastos_DevSamples_AsyncTaskDemo_LoadImageTask_H__

#include "_Elastos.DevSamples.AsyncTaskDemo.h"
#include <Elastos.Droid.App.h>
#include <Elastos.Droid.Content.h>
#include <elastos/droid/os/AsyncTask.h>

using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::App::IProgressDialog;

namespace Elastos {
namespace DevSamples {
namespace AsyncTaskDemo {

class CActivityOne;

class LoadImageTask
    : public AsyncTask
{
public:
    LoadImageTask(
        /* [in] */ CActivityOne* host,
        /* [in] */ IContext* context);

    ~LoadImageTask();

    CARAPI DoInBackground(
        /* [in] */ ArrayOf<IInterface*>* params,
        /* [out] */ IInterface** result);

    CARAPI OnPreExecute();

    CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    CARAPI OnProgressUpdate(
            /* [in] */ ArrayOf<IInterface*>* values);

public:

    AutoPtr<IProgressDialog> mProgressBar;
    CActivityOne* mHost;
};

} // namespace AsyncTaskDemo
} // namespace DevSamples
} // namespace Elastos

#endif // __Elastos_DevSamples_AsyncTaskDemo_LoadImageTask_H__
