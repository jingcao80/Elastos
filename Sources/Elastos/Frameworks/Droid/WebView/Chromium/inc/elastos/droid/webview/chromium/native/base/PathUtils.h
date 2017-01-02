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

#ifndef __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHUTILS_H__
#define __ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHUTILS_H__

#include "elastos/droid/ext/frameworkext.h"
#include <elastos/core/Object.h>

using Elastos::IO::IFile;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IApplicationInfo;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace Webview {
namespace Chromium {
namespace Base {

/**
 * This class provides the path related methods for the native library.
 */
class PathUtils
    : public Object
{
public:
    /**
     * Sets the suffix that should be used for the directory where private data is to be stored
     * by the application.
     * @param suffix The private data directory suffix.
     * @see Context#getDir(String, int)
     */
    static CARAPI_(void) SetPrivateDataDirectorySuffix(
        /* [in] */ const String& suffix);

    /**
     * @return the private directory that is used to store application data.
     */
    //@CalledByNative
    static CARAPI_(String) GetDataDirectory(
        /* [in] */ IContext* appContext);

    /**
     * @return the private directory that is used to store application database.
     */
    //@CalledByNative
    static CARAPI_(String) GetDatabaseDirectory(
        /* [in] */ IContext* appContext);

    /**
     * @return the cache directory.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    static CARAPI_(String) GetCacheDirectory(
        /* [in] */ IContext* appContext);

    /**
     * @return the external storage directory.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    static CARAPI_(String) GetExternalStorageDirectory();

    static CARAPI_(void*) ElaPathUtilsCallback_Init();

private:
    static CARAPI_(String) GetDataDirectory(
        /* [in] */ IInterface* appContext);

    static CARAPI_(String) GetDatabaseDirectory(
        /* [in] */ IInterface* appContext);

    static CARAPI_(String) GetCacheDirectory(
        /* [in] */ IInterface* appContext);

    static CARAPI_(String) GetDownloadsDirectory(
        /* [in] */ IInterface* appContext);

    static CARAPI_(String) GetNativeLibraryDirectory(
        /* [in] */ IInterface* appContext);

    // Prevent instantiation.
    PathUtils();

    /**
     * @return the public downloads directory.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    static CARAPI_(String) GetDownloadsDirectory(
        /* [in] */ IContext* appContext);

    /**
     * @return the path to native libraries.
     */
    //@SuppressWarnings("unused")
    //@CalledByNative
    static CARAPI_(String) GetNativeLibraryDirectory(
        /* [in] */ IContext* appContext);

    static String sDataDirectorySuffix;
};

} // namespace Base
} // namespace Chromium
} // namespace Webview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_WEBVIEW_CHROMIUM_BASE_PATHUTILS_H__
