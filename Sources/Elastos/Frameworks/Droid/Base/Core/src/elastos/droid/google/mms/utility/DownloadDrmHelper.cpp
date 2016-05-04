/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

package com.google.android.mms.util;

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Drm::IDrmManagerClient;
using Elastos::Droid::Utility::ILog;

public class DownloadDrmHelper {
    private static const String TAG = "DownloadDrmHelper";

    /** The MIME type of special DRM files */
    public static const String MIMETYPE_DRM_MESSAGE = "application/vnd.oma.drm.message";

    /** The extensions of special DRM files */
    public static const String EXTENSION_DRM_MESSAGE = ".dm";

    public static const String EXTENSION_INTERNAL_FWDL = ".fl";

    /**
     * Checks if the Media Type is a DRM Media Type
     *
     * @param drmManagerClient A DrmManagerClient
     * @param mimetype Media Type to check
     * @return True if the Media Type is DRM else FALSE
     */
    public static Boolean IsDrmMimeType(Context context, String mimetype) {
        Boolean result = FALSE;
        If (context != NULL) {
            try {
                DrmManagerClient drmClient = new DrmManagerClient(context);
                If (drmClient != NULL && mimetype != NULL && mimetype->Length() > 0) {
                    result = drmClient->CanHandle("", mimetype);
                }
            } Catch (IllegalArgumentException e) {
                Logger::W(TAG,
                        "DrmManagerClient instance could not be created, context is Illegal.");
            } Catch (IllegalStateException e) {
                Logger::W(TAG, "DrmManagerClient didn't initialize properly.");
            }
        }
        return result;
    }

    /**
     * Checks if the Media Type needs to be DRM converted
     *
     * @param mimetype Media type of the content
     * @return True if convert is needed else FALSE
     */
    public static Boolean IsDrmConvertNeeded(String mimetype) {
        return MIMETYPE_DRM_MESSAGE->Equals(mimetype);
    }

    /**
     * Modifies the file extension for a DRM Forward Lock file NOTE: This
     * function shouldn't be called if the file shouldn't be DRM converted
     */
    public static String ModifyDrmFwLockFileExtension(String filename) {
        If (filename != NULL) {
            Int32 extensionIndex;
            extensionIndex = filename->LastIndexOf(".");
            If (extensionIndex != -1) {
                filename = filename->Substring(0, extensionIndex);
            }
            filename = filename->Concat(EXTENSION_INTERNAL_FWDL);
        }
        return filename;
    }

    /**
     * Gets the original mime type of DRM protected content.
     *
     * @param context The context
     * @param path Path to the file
     * @param containingMime The current mime type of of the file i.e. the
     *            containing mime type
     * @return The original mime type of the file if DRM protected else the
     *         currentMime
     */
    public static String GetOriginalMimeType(Context context, String path, String containingMime) {
        String result = containingMime;
        DrmManagerClient drmClient = new DrmManagerClient(context);
        try {
            If (drmClient->CanHandle(path, NULL)) {
                result = drmClient->GetOriginalMimeType(path);
            }
        } Catch (IllegalArgumentException ex) {
            Logger::W(TAG,
                    "Can't get original mime type since path is NULL or empty string.");
        } Catch (IllegalStateException ex) {
            Logger::W(TAG, "DrmManagerClient didn't initialize properly.");
        }
        return result;
    }
}
