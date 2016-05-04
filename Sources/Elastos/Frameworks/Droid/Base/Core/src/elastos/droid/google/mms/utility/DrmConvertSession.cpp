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
using Elastos::Droid::Drm::IDrmConvertedStatus;
using Elastos::Droid::Drm::IDrmManagerClient;
using Elastos::Droid::Utility::ILog;
using Elastos::Droid::Provider::IDownloads;

using Elastos::IO::IRandomAccessFile;


public class DrmConvertSession {
    private DrmManagerClient mDrmClient;
    private Int32 mConvertSessionId;
    private static const String TAG = "DrmConvertSession";

    private DrmConvertSession(DrmManagerClient drmClient, Int32 convertSessionId) {
        mDrmClient = drmClient;
        mConvertSessionId = convertSessionId;
    }

    /**
     * Start of converting a file.
     *
     * @param context The context of the application running the convert session.
     * @param mimeType Mimetype of content that shall be converted.
     * @return A convert session or NULL in case an error occurs.
     */
    public static DrmConvertSession Open(Context context, String mimeType) {
        DrmManagerClient drmClient = NULL;
        Int32 convertSessionId = -1;
        If (context != NULL && mimeType != NULL && !mimeType->Equals("")) {
            try {
                drmClient = new DrmManagerClient(context);
                try {
                    convertSessionId = drmClient->OpenConvertSession(mimeType);
                } Catch (IllegalArgumentException e) {
                    Logger::W(TAG, "Conversion of Mimetype: " + mimeType
                            + " is not supported.", e);
                } Catch (IllegalStateException e) {
                    Logger::W(TAG, "Could not access Open DrmFramework.", e);
                }
            } Catch (IllegalArgumentException e) {
                Logger::W(TAG,
                        "DrmManagerClient instance could not be created, context is Illegal.");
            } Catch (IllegalStateException e) {
                Logger::W(TAG, "DrmManagerClient didn't initialize properly.");
            }
        }

        If (drmClient == NULL || convertSessionId < 0) {
            return NULL;
        } else {
            return new DrmConvertSession(drmClient, convertSessionId);
        }
    }
    /**
     * Convert a buffer of data to protected format.
     *
     * @param buffer Buffer filled with data to convert.
     * @param size The number of bytes that shall be converted.
     * @return A Buffer filled with converted data, if execution is ok, in all
     *         other case NULL.
     */
    public Byte [] Convert(Byte[] inBuffer, Int32 size) {
        Byte[] result = NULL;
        If (inBuffer != NULL) {
            DrmConvertedStatus convertedStatus = NULL;
            try {
                If (size != inBuffer.length) {
                    Byte[] buf = new Byte[size];
                    System->Arraycopy(inBuffer, 0, buf, 0, size);
                    convertedStatus = mDrmClient->ConvertData(mConvertSessionId, buf);
                } else {
                    convertedStatus = mDrmClient->ConvertData(mConvertSessionId, inBuffer);
                }

                If (convertedStatus != NULL &&
                        convertedStatus.statusCode == DrmConvertedStatus.STATUS_OK &&
                        convertedStatus.convertedData != NULL) {
                    result = convertedStatus.convertedData;
                }
            } Catch (IllegalArgumentException e) {
                Logger::W(TAG, "Buffer with data to convert is illegal. Convertsession: "
                        + mConvertSessionId, e);
            } Catch (IllegalStateException e) {
                Logger::W(TAG, "Could not convert data. Convertsession: " +
                        mConvertSessionId, e);
            }
        } else {
            throw new IllegalArgumentException("Parameter inBuffer is NULL");
        }
        return result;
    }

    /**
     * Ends a conversion session of a file.
     *
     * @param fileName The filename of the converted file.
     * @return Downloads.Impl.STATUS_SUCCESS if execution is ok.
     *         Downloads.Impl.STATUS_FILE_ERROR in case converted file can not
     *         be accessed. Downloads.Impl.STATUS_NOT_ACCEPTABLE if a problem
     *         occurs when accessing drm framework.
     *         Downloads.Impl.STATUS_UNKNOWN_ERROR if a general error occurred.
     */
    public Int32 Close(String filename) {
        DrmConvertedStatus convertedStatus = NULL;
        Int32 result = Downloads.Impl.STATUS_UNKNOWN_ERROR;
        If (mDrmClient != NULL && mConvertSessionId >= 0) {
            try {
                convertedStatus = mDrmClient->CloseConvertSession(mConvertSessionId);
                If (convertedStatus == NULL ||
                        convertedStatus.statusCode != DrmConvertedStatus.STATUS_OK ||
                        convertedStatus.convertedData == NULL) {
                    result = Downloads.Impl.STATUS_NOT_ACCEPTABLE;
                } else {
                    RandomAccessFile rndAccessFile = NULL;
                    try {
                        rndAccessFile = new RandomAccessFile(filename, "rw");
                        rndAccessFile->Seek(convertedStatus.offset);
                        rndAccessFile->Write(convertedStatus.convertedData);
                        result = Downloads.Impl.STATUS_SUCCESS;
                    } Catch (FileNotFoundException e) {
                        result = Downloads.Impl.STATUS_FILE_ERROR;
                        Logger::W(TAG, "File: " + filename + " could not be found.", e);
                    } Catch (IOException e) {
                        result = Downloads.Impl.STATUS_FILE_ERROR;
                        Logger::W(TAG, "Could not access File: " + filename + " .", e);
                    } Catch (IllegalArgumentException e) {
                        result = Downloads.Impl.STATUS_FILE_ERROR;
                        Logger::W(TAG, "Could not open file in mode: rw", e);
                    } Catch (SecurityException e) {
                        Logger::W(TAG, "Access to File: " + filename +
                                " was denied denied by SecurityManager.", e);
                    } finally {
                        If (rndAccessFile != NULL) {
                            try {
                                rndAccessFile->Close();
                            } Catch (IOException e) {
                                result = Downloads.Impl.STATUS_FILE_ERROR;
                                Logger::W(TAG, "Failed to close File:" + filename
                                        + ".", e);
                            }
                        }
                    }
                }
            } Catch (IllegalStateException e) {
                Logger::W(TAG, "Could not close convertsession. Convertsession: " +
                        mConvertSessionId, e);
            }
        }
        return result;
    }
}
