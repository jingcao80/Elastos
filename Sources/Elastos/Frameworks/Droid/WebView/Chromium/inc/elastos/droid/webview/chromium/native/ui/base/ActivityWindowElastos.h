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

// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_ACTIVITYWINDOWANDROID_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_ACTIVITYWINDOWANDROID_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/WindowElastos.h"

// package org.chromium.ui.base;
// import android.app.Activity;
// import android.app.PendingIntent;
// import android.content.ActivityNotFoundException;
// import android.content.Intent;
// import android.content.IntentSender.SendIntentException;
// import java.lang.ref.WeakReference;

using Elastos::Droid::App::IActivity;
using Elastos::Droid::App::IPendingIntent;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::WindowElastos;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * The class provides the WindowElastos's implementation which requires
  * Activity Instance.
  * Only instantiate this class when you need the implemented features.
  */
class ActivityWindowElastos
    : public WindowElastos
{
public:
    ActivityWindowElastos(
        /* [in] */ IActivity* activity);

    // @Override
    CARAPI_(Int32) ShowCancelableIntent(
        /* [in] */ IPendingIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    // @Override
    CARAPI_(Int32) ShowCancelableIntent(
        /* [in] */ IIntent* intent,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

    // @Override
    CARAPI CancelIntent(
        /* [in] */ Int32 requestCode);

    // @Override
    CARAPI_(Boolean) OnActivityResult(
        /* [in] */ Int32 requestCode,
        /* [in] */ Int32 resultCode,
        /* [in] */ IIntent* data);

    // @Override
    CARAPI_(AutoPtr<IWeakReference>) GetActivity();

private:
    CARAPI_(Int32) GenerateNextRequestCode();

    CARAPI StoreCallbackData(
        /* [in] */ Int32 requestCode,
        /* [in] */ IntentCallback* callback,
        /* [in] */ Int32 errorId);

private:
    // Constants used for intent request code bounding.
    static const Int32 REQUEST_CODE_PREFIX = 1000;
    static const Int32 REQUEST_CODE_RANGE_SIZE = 100;
    static const String TAG;
    /*const*/ AutoPtr<IWeakReference> mActivityRef;
    Int32 mNextRequestCode;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_ACTIVITYWINDOWANDROID_H_

