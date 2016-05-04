/*
 * Copyright (C) 2010 The Android Open Source Project
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
 */

package com.android.internal.telephony;

/**
 * WapPushManager constant value definitions
 */
public class WapPushManagerParams {
    /**
     * Application type activity
     */
    public static const Int32 APP_TYPE_ACTIVITY = 0;

    /**
     * Application type service
     */
    public static const Int32 APP_TYPE_SERVICE = 1;

    /**
     * Process Message return value
     * Message is handled
     */
    public static const Int32 MESSAGE_HANDLED = 0x1;

    /**
     * Process Message return value
     * Application ID or content type was not found in the application ID table
     */
    public static const Int32 APP_QUERY_FAILED = 0x2;

    /**
     * Process Message return value
     * Receiver application signature check failed
     */
    public static const Int32 SIGNATURE_NO_MATCH = 0x4;

    /**
     * Process Message return value
     * Receiver application was not found
     */
    public static const Int32 INVALID_RECEIVER_NAME = 0x8;

    /**
     * Process Message return value
     * Unknown exception
     */
    public static const Int32 EXCEPTION_CAUGHT = 0x10;

    /**
     * Process Message return value
     * Need further processing after WapPushManager message processing
     */
    public static const Int32 FURTHER_PROCESSING = 0x8000;

}

