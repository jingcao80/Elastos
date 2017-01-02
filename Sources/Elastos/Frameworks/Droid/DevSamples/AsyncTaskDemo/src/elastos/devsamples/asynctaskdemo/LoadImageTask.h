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
