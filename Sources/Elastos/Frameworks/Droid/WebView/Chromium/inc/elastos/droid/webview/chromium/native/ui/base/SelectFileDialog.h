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

// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_SELECTFILEDIALOG_H_
#define _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_SELECTFILEDIALOG_H_

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/AsyncTask.h"
#include "elastos/droid/webkit/webview/chromium/native/ui/base/WindowElastos.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Webkit::Webview::Chromium::Ui::Base::WindowElastos;
using Elastos::IO::IFile;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Ui {
namespace Base {

/**
  * A dialog that is triggered from a file input field that allows a user to select a file based on
  * a set of accepted file types. The path of the selected file is passed to the native dialog.
  */
// @JNINamespace("ui")
class SelectFileDialog
    : public WindowElastos::IntentCallback
{
private:
    class GetDisplayNameTask
        : public AsyncTask
    {
    public:
        GetDisplayNameTask(
            /* [in] */ SelectFileDialog* owner,
            /* [in] */ IContentResolver* contentResolver,
            /* [in] */ Boolean isMultiple);

    protected:
        // @Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [out] */ IInterface** result);

        // @Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    public:
        SelectFileDialog* mOwner;
        AutoPtr< ArrayOf<String> > mFilePaths;
        AutoPtr<IContentResolver> mContentResolver;
        Boolean mIsMultiple;
    };

public:
    /**
      * Callback method to handle the intent results and pass on the path to the native
      * SelectFileDialog.
      * @param window The window that has access to the application activity.
      * @param resultCode The result code whether the intent returned successfully.
      * @param contentResolver The content resolver used to extract the path of the selected file.
      * @param results The results of the requested intent.
      */
    // @Override
    CARAPI OnIntentCompleted(
        /* [in] */ WindowElastos* window,
        /* [in] */ Int32 resultCode,
        /* [in] */ IContentResolver* contentResolver,
        /* [in] */ IIntent* results);

    static CARAPI_(void*) ElaSelectFileDialogCallback_Init();

private:
    SelectFileDialog(
        /* [in] */ HANDLE nativeSelectFileDialog);

    /**
      * Creates and starts an intent based on the passed fileTypes and capture value.
      * @param fileTypes MIME types requested (i.e. "image")
      * @param capture The capture value as described in http://www.w3.org/TR/html-media-capture/
      * @param window The WindowElastos that can show intents
      */
    // @CalledByNative
    CARAPI SelectFile(
        /* [in] */ ArrayOf<String>* fileTypes,
        /* [in] */ Boolean capture,
        /* [in] */ WindowElastos* window);

    /**
      * Get a file for the image capture in the CAPTURE_IMAGE_DIRECTORY directory.
      */
    CARAPI_(AutoPtr<IFile>) GetFileForImageCapture();

    CARAPI OnFileNotSelected();

    CARAPI_(Boolean) NoSpecificType();

    CARAPI_(Boolean) ShouldShowTypes(
        /* [in] */ const String& allTypes,
        /* [in] */ const String& specificType);

    CARAPI_(Boolean) ShouldShowImageTypes();

    CARAPI_(Boolean) ShouldShowVideoTypes();

    CARAPI_(Boolean) ShouldShowAudioTypes();

    CARAPI_(Boolean) AcceptsSpecificType(
        /* [in] */ const String& type);

    CARAPI_(Boolean) CaptureCamera();

    CARAPI_(Boolean) CaptureCamcorder();

    CARAPI_(Boolean) CaptureMicrophone();

    CARAPI_(Boolean) AcceptSpecificType(
        /* [in] */ const String& accept);

    // @CalledByNative return SelectFileDialog
    static CARAPI_(AutoPtr<IInterface>) Create(
        /* [in] */ HANDLE nativeSelectFileDialog);

    CARAPI NativeOnFileSelected(
        /* [in] */ HANDLE nativeSelectFileDialogImpl,
        /* [in] */ const String& filePath,
        /* [in] */ const String& displayName);

    CARAPI NativeOnFileNotSelected(
        /* [in] */ HANDLE nativeSelectFileDialogImpl);

    static CARAPI_(void) SelectFile(
        /* [in] */ IInterface* obj,
        /* [in] */ ArrayOf<String>* fileTypes,
        /* [in] */ Boolean capture,
        /* [in] */ IInterface* window);

private:
    static const String IMAGE_TYPE;
    static const String VIDEO_TYPE;
    static const String AUDIO_TYPE;
    static const String ALL_IMAGE_TYPES;
    static const String ALL_VIDEO_TYPES;
    static const String ALL_AUDIO_TYPES;
    static const String ANY_TYPES;
    static const String CAPTURE_IMAGE_DIRECTORY;
    HANDLE mNativeSelectFileDialog;
    AutoPtr<IList> mFileTypes;
    Boolean mCapture;
    AutoPtr<IUri> mCameraOutputUri;
};

} // namespace Base
} // namespace Ui
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif // _ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_UI_BASE_SELECTFILEDIALOG_H_

