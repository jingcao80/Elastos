/*
 * Copyright (C) 2006 The Android Open Source Project
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

package android.provider;

using Elastos::Droid::Annotation::ISdkConstant;
using Elastos::Droid::Annotation::SdkConstant::ISdkConstantType;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContentValues;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Database::Sqlite::ISqliteWrapper;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Droid::Telephony::ISubscriptionManager;
using Elastos::Droid::Text::ITextUtils;
using Elastos::Droid::Telephony::IRlog;
using Elastos::Droid::Utility::IPatterns;

using Elastos::Droid::Internal::Telephony::IPhoneConstants;
using Elastos::Droid::Internal::Telephony::ISmsApplication;

using Elastos::Utility::IArrayList;
using Elastos::Utility::IArrays;
using Elastos::Utility::IHashSet;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Regex::IMatcher;
using Elastos::Utility::Regex::IPattern;

/**
 * The Telephony provider contains data related to phone operation, specifically SMS and MMS
 * messages and access to the APN list, including the MMSC to use.
 *
 * <p class="note"><strong>Note:</strong> These APIs are not available on all Android-powered
 * devices. If your app depends on telephony features such as for managing SMS messages, include
 * a <a href="{@docRoot}guide/topics/manifest/uses-feature-element.html">{@code &lt;uses-feature>}
 * </a> element in your manifest that declares the {@code "android.hardware.telephony"} hardware
 * feature. Alternatively, you can check for telephony availability at runtime using either
 * {@link android.content.pm.PackageManager#hasSystemFeature
 * HasSystemFeature(PackageManager.FEATURE_TELEPHONY)} or {@link
 * android.telephony.TelephonyManager#getPhoneType}.</p>
 *
 * <h3>Creating an SMS app</h3>
 *
 * <p>Only the default SMS App (selected by the user in system settings) is able to write to the
 * SMS Provider (the tables defined within the {@code Telephony} class) and only the default SMS
 * app receives the {@link android.provider.Telephony.Sms.Intents#SMS_DELIVER_ACTION} broadcast
 * when the user receives an SMS or the {@link
 * android.provider.Telephony.Sms.Intents#WAP_PUSH_DELIVER_ACTION} broadcast when the user
 * receives an MMS.</p>
 *
 * <p>Any app that wants to behave as the user's default SMS app must handle the following intents:
 * <ul>
 * <li>In a broadcast receiver, include an intent filter for {@link Sms.Intents#SMS_DELIVER_ACTION}
 * (<code>"android.provider.Telephony.SMS_DELIVER"</code>). The broadcast receiver must also
 * require the {@link Manifest::permission::BROADCAST_SMS} permission.
 * <p>This allows your app to directly receive incoming SMS messages.</p></li>
 * <li>In a broadcast receiver, include an intent filter for {@link
 * Sms.Intents#WAP_PUSH_DELIVER_ACTION}} ({@code "android.provider.Telephony.WAP_PUSH_DELIVER"})
 * with the MIME type <code>"application/vnd.wap.mms-message"</code>.
 * The broadcast receiver must also require the {@link
 * Manifest::permission::BROADCAST_WAP_PUSH} permission.
 * <p>This allows your app to directly receive incoming MMS messages.</p></li>
 * <li>In your activity that delivers new messages, include an intent filter for
 * {@link android.content.IIntent::ACTION_SENDTO} (<code>"android.intent.action.SENDTO"
 * </code>) with schemas, <code>sms:</code>, <code>smsto:</code>, <code>mms:</code>, and
 * <code>mmsto:</code>.
 * <p>This allows your app to receive intents from other apps that want to deliver a
 * message.</p></li>
 * <li>In a service, include an intent filter for {@link
 * android.telephony.TelephonyManager#ACTION_RESPOND_VIA_MESSAGE}
 * (<code>"android.intent.action.RESPOND_VIA_MESSAGE"</code>) with schemas,
 * <code>sms:</code>, <code>smsto:</code>, <code>mms:</code>, and <code>mmsto:</code>.
 * This service must also require the {@link
 * Manifest::permission::SEND_RESPOND_VIA_MESSAGE} permission.
 * <p>This allows users to respond to incoming phone calls with an immediate text message
 * using your app.</p></li>
 * </ul>
 *
 * <p>Other apps that are not selected as the default SMS app can only <em>read</em> the SMS
 * Provider, but may also be notified when a new SMS arrives by listening for the {@link
 * Sms.Intents#SMS_RECEIVED_ACTION}
 * broadcast, which is a non-abortable broadcast that may be delivered to multiple apps. This
 * broadcast is intended for apps that&mdash;while not selected as the default SMS app&mdash;need to
 * read special incoming messages such as to perform phone number verification.</p>
 *
 * <p>For more information about building SMS apps, read the blog post, <a
 * href="http://android-developers.blogspot.com/2013/10/getting-your-sms-apps-ready-for-kitkat.html"
 * >Getting Your SMS Apps Ready for KitKat</a>.</p>
 *
 */
public class Telephony {
    private static const String TAG = "Telephony";

    /**
     * Not instantiable.
     * @hide
     */
    private Telephony() {
    }

    /**
     * Base columns for tables that contain text-based SMSs.
     */
    public interface TextBasedSmsColumns {

        /** Message type: all messages. */
        public static const Int32 MESSAGE_TYPE_ALL    = 0;

        /** Message type: inbox. */
        public static const Int32 MESSAGE_TYPE_INBOX  = 1;

        /** Message type: sent messages. */
        public static const Int32 MESSAGE_TYPE_SENT   = 2;

        /** Message type: drafts. */
        public static const Int32 MESSAGE_TYPE_DRAFT  = 3;

        /** Message type: outbox. */
        public static const Int32 MESSAGE_TYPE_OUTBOX = 4;

        /** Message type: failed outgoing message. */
        public static const Int32 MESSAGE_TYPE_FAILED = 5;

        /** Message type: queued to send later. */
        public static const Int32 MESSAGE_TYPE_QUEUED = 6;

        /**
         * The type of message.
         * <P>Type: INTEGER</P>
         */
        public static const String TYPE = "type";

        /**
         * The thread ID of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String THREAD_ID = "thread_id";

        /**
         * The address of the other party.
         * <P>Type: TEXT</P>
         */
        public static const String ADDRESS = "address";

        /**
         * The date the message was received.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DATE = "date";

        /**
         * The date the message was sent.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DATE_SENT = "date_sent";

        /**
         * Has the message been read?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String READ = "read";

        /**
         * Has the message been seen by the user? The "seen" flag determines
         * whether we need to show a notification.
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String SEEN = "seen";

        /**
         * {@code TP-Status} value for the message, or -1 if no status has been received.
         * <P>Type: INTEGER</P>
         */
        public static const String STATUS = "status";

        /** TP-Status: no status received. */
        public static const Int32 STATUS_NONE = -1;
        /** TP-Status: complete. */
        public static const Int32 STATUS_COMPLETE = 0;
        /** TP-Status: pending. */
        public static const Int32 STATUS_PENDING = 32;
        /** TP-Status: failed. */
        public static const Int32 STATUS_FAILED = 64;

        /**
         * The subject of the message, if present.
         * <P>Type: TEXT</P>
         */
        public static const String SUBJECT = "subject";

        /**
         * The body of the message.
         * <P>Type: TEXT</P>
         */
        public static const String BODY = "body";

        /**
         * The ID of the sender of the conversation, if present.
         * <P>Type: INTEGER (reference to item in {@code content://contacts/people})</P>
         */
        public static const String PERSON = "person";

        /**
         * The protocol identifier code.
         * <P>Type: INTEGER</P>
         */
        public static const String PROTOCOL = "protocol";

        /**
         * Is the {@code TP-Reply-Path} flag set?
         * <P>Type: BOOLEAN</P>
         */
        public static const String REPLY_PATH_PRESENT = "reply_path_present";

        /**
         * The service Center (SC) through which to send the message, if present.
         * <P>Type: TEXT</P>
         */
        public static const String SERVICE_CENTER = "service_center";

        /**
         * Is the message locked?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String LOCKED = "locked";

        /**
         * The phone id to which the message belongs to
         * <p>Type: INTEGER (Int64) </p>
         * @hide
         */
        public static const String PHONE_ID = "phone_id";

        /**
         * The MTU size of the mobile interface to which the APN connected
         * @hide
         */
        public static const String MTU = "mtu";

        /**
         * Error code associated with sending or receiving this message
         * <P>Type: INTEGER</P>
         */
        public static const String ERROR_CODE = "error_code";

        /**
         * The identity of the sender of a sent message. It is
         * usually the package name of the app which sends the message.
         * <p>Type: TEXT</p>
         */
        public static const String CREATOR = "creator";

       /**
         * The priority of the message.
         * <P>Type: INTEGER</P>
         * @hide
         */
        public static const String PRIORITY = "priority";
    }

    /**
     * Contains all text-based SMS messages.
     */
    public class Sms implements BaseColumns, TextBasedSmsColumns {

        /**
         * Not instantiable.
         * @hide
         */
        private Sms() {
        }

        /**
         * Used to determine the currently configured default SMS package.
         * @param context context of the requesting application
         * @return package name for the default SMS package or NULL
         */
        public static String GetDefaultSmsPackage(Context context) {
            ComponentName component = SmsApplication->GetDefaultSmsApplication(context, FALSE);
            If (component != NULL) {
                return component->GetPackageName();
            }
            return NULL;
        }

        /**
         * Return cursor for table query.
         * @hide
         */
        public static Cursor Query(ContentResolver cr, String[] projection) {
            return cr->Query(CONTENT_URI, projection, NULL, NULL, DEFAULT_SORT_ORDER);
        }

        /**
         * Return cursor for table query.
         * @hide
         */
        public static Cursor Query(ContentResolver cr, String[] projection,
                String where, String orderBy) {
            return cr->Query(CONTENT_URI, projection, where,
                    NULL, orderBy == NULL ? DEFAULT_SORT_ORDER : orderBy);
        }

        /**
         * The {@code content://} style URL for this table.
         */
        public static const Uri CONTENT_URI = Uri->Parse("content://sms");

        /**
         * The default sort order for this table.
         */
        public static const String DEFAULT_SORT_ORDER = "date DESC";

        /**
         * Add an SMS to the given URI.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the pseudo-subject of the message
         * @param date the timestamp for the message
         * @param read TRUE if the message has been read, FALSE if not
         * @param deliveryReport TRUE if a delivery report was requested, FALSE if not
         * @return the URI for the new message
         * @hide
         */
        public static Uri AddMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, Boolean read, Boolean deliveryReport) {
            return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                    resolver, uri, address, body, subject, date, read, deliveryReport, -1L);
        }

        /**
         * Add an SMS to the given URI.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read TRUE if the message has been read, FALSE if not
         * @param deliveryReport TRUE if a delivery report was requested, FALSE if not
         * @param subId the sub_id which the message belongs to
         * @return the URI for the new message
         * @hide
         */
        public static Uri AddMessageToUri(Int64 subId, ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, Boolean read, Boolean deliveryReport) {
            return AddMessageToUri(subId, resolver, uri, address, body, subject,
                    date, read, deliveryReport, -1L);
        }

        /**
         * Add an SMS to the given URI with the specified thread ID.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the pseudo-subject of the message
         * @param date the timestamp for the message
         * @param read TRUE if the message has been read, FALSE if not
         * @param deliveryReport TRUE if a delivery report was requested, FALSE if not
         * @param threadId the thread_id of the message
         * @return the URI for the new message
         * @hide
         */
        public static Uri AddMessageToUri(ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, Boolean read, Boolean deliveryReport, Int64 threadId) {
            return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                    resolver, uri, address, body, subject,
                    date, read, deliveryReport, threadId);
        }

        /**
         * Add an SMS to the given URI with thread_id specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read TRUE if the message has been read, FALSE if not
         * @param deliveryReport TRUE if a delivery report was requested, FALSE if not
         * @param threadId the thread_id of the message
         * @param subId the sub_id which the message belongs to
         * @return the URI for the new message
         * @hide
         */
        public static Uri AddMessageToUri(Int64 subId, ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, Boolean read, Boolean deliveryReport, Int64 threadId) {
            return AddMessageToUri(subId, resolver, uri, address, body, subject,
                    date, read, deliveryReport, threadId, -1);
        }

        /**
         * Add an SMS to the given URI with priority specified.
         *
         * @param resolver the content resolver to use
         * @param uri the URI to add the message to
         * @param address the address of the sender
         * @param body the body of the message
         * @param subject the psuedo-subject of the message
         * @param date the timestamp for the message
         * @param read TRUE if the message has been read, FALSE if not
         * @param deliveryReport TRUE if a delivery report was requested, FALSE if not
         * @param threadId the thread_id of the message
         * @param subId the sub_id which the message belongs to
         * @param priority the priority of the message
         * @return the URI for the new message
         * @hide
         */
        public static Uri AddMessageToUri(Int64 subId, ContentResolver resolver,
                Uri uri, String address, String body, String subject,
                Long date, Boolean read, Boolean deliveryReport, Int64 threadId,
                Int32 priority) {
            ContentValues values = new ContentValues(8);
            Rlog->V(TAG,"Telephony addMessageToUri sub id: " + subId);

            Int32 phoneId = SubscriptionManager->GetPhoneId(subId);
            values->Put(PHONE_ID, phoneId);
            values->Put(ADDRESS, address);
            If (date != NULL) {
                values->Put(DATE, date);
            }
            values->Put(READ, read ? Integer->ValueOf(1) : Integer->ValueOf(0));
            values->Put(SUBJECT, subject);
            values->Put(BODY, body);
            values->Put(PRIORITY, priority);
            If (deliveryReport) {
                values->Put(STATUS, STATUS_PENDING);
            }
            If (threadId != -1L) {
                values->Put(THREAD_ID, threadId);
            }
            return resolver->Insert(uri, values);
        }

        /**
         * Move a message to the given folder.
         *
         * @param context the context to use
         * @param uri the message to move
         * @param folder the folder to move to
         * @return TRUE if the operation succeeded
         * @hide
         */
        public static Boolean MoveMessageToFolder(Context context,
                Uri uri, Int32 folder, Int32 error) {
            If (uri == NULL) {
                return FALSE;
            }

            Boolean markAsUnread = FALSE;
            Boolean markAsRead = FALSE;
            Switch(folder) {
            case MESSAGE_TYPE_INBOX:
            case MESSAGE_TYPE_DRAFT:
                break;
            case MESSAGE_TYPE_OUTBOX:
            case MESSAGE_TYPE_SENT:
                markAsRead = TRUE;
                break;
            case MESSAGE_TYPE_FAILED:
            case MESSAGE_TYPE_QUEUED:
                markAsUnread = TRUE;
                break;
            default:
                return FALSE;
            }

            ContentValues values = new ContentValues(3);

            values->Put(TYPE, folder);
            If (markAsUnread) {
                values->Put(READ, 0);
            } else If (markAsRead) {
                values->Put(READ, 1);
            }
            values->Put(ERROR_CODE, error);

            return 1 == SqliteWrapper->Update(context, context->GetContentResolver(),
                            uri, values, NULL, NULL);
        }

        /**
         * Returns TRUE iff the Folder (message type) identifies an
         * outgoing message.
         * @hide
         */
        public static Boolean IsOutgoingFolder(Int32 messageType) {
            Return  (messageType == MESSAGE_TYPE_FAILED)
                    || (messageType == MESSAGE_TYPE_OUTBOX)
                    || (messageType == MESSAGE_TYPE_SENT)
                    || (messageType == MESSAGE_TYPE_QUEUED);
        }

        /**
         * Contains all text-based SMS messages in the SMS app inbox.
         */
        public class Inbox implements BaseColumns, TextBasedSmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Inbox() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->Parse("content://sms/inbox");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the pseudo-subject of the message
             * @param date the timestamp for the message
             * @param read TRUE if the message has been read, FALSE if not
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    Boolean read) {
                return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                        resolver, CONTENT_URI, address, body, subject, date, read, FALSE);
            }

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param read TRUE if the message has been read, FALSE if not
             * @param subId the sub_id which the message belongs to
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(Int64 subId, ContentResolver resolver,
                    String address, String body, String subject, Long date, Boolean read) {
                return AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
                        subject, date, read, FALSE);
            }
        }

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        public class Sent implements BaseColumns, TextBasedSmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Sent() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->Parse("content://sms/sent");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the pseudo-subject of the message
             * @param date the timestamp for the message
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                        resolver, CONTENT_URI, address, body, subject, date, TRUE, FALSE);
            }

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param subId the sub_id which the message belongs to
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(Int64 subId, ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
                        subject, date, TRUE, FALSE);
            }
        }

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        public class Draft implements BaseColumns, TextBasedSmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Draft() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->Parse("content://sms/draft");

           /**
            * @hide
            */
            public static Uri AddMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                        resolver, CONTENT_URI, address, body, subject, date, TRUE, FALSE);
            }

            /**
             * Add an SMS to the Draft box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param subId the sub_id which the message belongs to
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(Int64 subId, ContentResolver resolver,
                    String address, String body, String subject, Long date) {
                return AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
                        subject, date, TRUE, FALSE);
            }

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all pending outgoing text-based SMS messages.
         */
        public class Outbox implements BaseColumns, TextBasedSmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Outbox() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->Parse("content://sms/outbox");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * Add an SMS to the outbox.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the pseudo-subject of the message
             * @param date the timestamp for the message
             * @param deliveryReport whether a delivery report was requested for the message
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    Boolean deliveryReport, Int64 threadId) {
                return AddMessageToUri(SubscriptionManager->GetDefaultSmsSubId(),
                        resolver, CONTENT_URI, address, body, subject, date,
                        TRUE, deliveryReport, threadId);
            }

            /**
             * Add an SMS to the Out box.
             *
             * @param resolver the content resolver to use
             * @param address the address of the sender
             * @param body the body of the message
             * @param subject the psuedo-subject of the message
             * @param date the timestamp for the message
             * @param deliveryReport whether a delivery report was requested for the message
             * @param subId the sub_id which the message belongs to
             * @return the URI for the new message
             * @hide
             */
            public static Uri AddMessage(Int64 subId, ContentResolver resolver,
                    String address, String body, String subject, Long date,
                    Boolean deliveryReport, Int64 threadId) {
                return AddMessageToUri(subId, resolver, CONTENT_URI, address, body,
                        subject, date, TRUE, deliveryReport, threadId);
            }
        }

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        public class Conversations
                implements BaseColumns, TextBasedSmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Conversations() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->Parse("content://sms/conversations");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";

            /**
             * The first 45 characters of the body of the message.
             * <P>Type: TEXT</P>
             */
            public static const String SNIPPET = "snippet";

            /**
             * The number of messages in the conversation.
             * <P>Type: INTEGER</P>
             */
            public static const String MESSAGE_COUNT = "msg_count";
        }

        /**
         * Contains constants for SMS related Intents that are broadcast.
         */
        public class Intents {

            /**
             * Not instantiable.
             * @hide
             */
            private Intents() {
            }

            /**
             * Set by BroadcastReceiver to indicate that the message was handled
             * successfully.
             */
            public static const Int32 RESULT_SMS_HANDLED = 1;

            /**
             * Set by BroadcastReceiver to indicate a generic error while
             * processing the message.
             */
            public static const Int32 RESULT_SMS_GENERIC_ERROR = 2;

            /**
             * Set by BroadcastReceiver to indicate insufficient memory to store
             * the message.
             */
            public static const Int32 RESULT_SMS_OUT_OF_MEMORY = 3;

            /**
             * Set by BroadcastReceiver to indicate that the message, while
             * possibly valid, is of a format or encoding that is not
             * supported.
             */
            public static const Int32 RESULT_SMS_UNSUPPORTED = 4;

            /**
             * Set by BroadcastReceiver to indicate a duplicate incoming message.
             */
            public static const Int32 RESULT_SMS_DUPLICATED = 5;

            /**
             * Used internally: The sender of the SMS was blacklisted
             * for not being listed in the contact list
             * @hide
             */
            public static const Int32 RESULT_SMS_BLACKLISTED_UNKNOWN = 6;

            /**
             * Used internally: The sender of the SMS was blacklisted
             * for being listed in the blacklist
             * @hide
             */
            public static const Int32 RESULT_SMS_BLACKLISTED_LIST = 7;

            /**
             * Used internally: The sender of the SMS was blacklisted
             * for matching a blacklist regex entry
             * @hide
             */
            public static const Int32 RESULT_SMS_BLACKLISTED_REGEX = 8;

            /**
             * Used internally:
             * Broadcast Action: A new protected text-based SMS message has been received
             * by the device. This intent will be delivered to all registered
             * receivers who possess {@link Manifest::permission::RECEIVE_PROTECTED_SMS}.
             * These apps SHOULD NOT write the message or notify the user.
             * The intent will have the following extra values:
             * </p>
             *
             * <ul>
             *   <li><em>"pdus"</em> - An Object[] of Byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             * @hide
             */
            public static const String PROTECTED_SMS_RECEIVED_ACTION =
                    "android.provider.Telephony.ACTION_PROTECTED_SMS_RECEIVED";

            /**
             * Activity action: Ask the user to change the default
             * SMS application. This will show a dialog that asks the
             * user whether they want to replace the current default
             * SMS application with the one specified in
             * {@link #EXTRA_PACKAGE_NAME}.
             */
            @SdkConstant(SdkConstantType.ACTIVITY_INTENT_ACTION)
            public static const String ACTION_CHANGE_DEFAULT =
                    "android.provider.Telephony.ACTION_CHANGE_DEFAULT";

            /**
             * The PackageName string passed in as an
             * extra for {@link #ACTION_CHANGE_DEFAULT}
             *
             * @see #ACTION_CHANGE_DEFAULT
             */
            public static const String EXTRA_PACKAGE_NAME = "package";

            /**
             * Broadcast Action: A new text-based SMS message has been received
             * by the device. This intent will only be delivered to the default
             * sms app. That app is responsible for writing the message and notifying
             * the user. The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>"pdus"</em> - An Object[] of Byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must declare
             * {@link Manifest::permission::BROADCAST_SMS} as a required permission in
             * the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">{@code
             * &lt;receiver>}</a> tag.
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_DELIVER_ACTION =
                    "android.provider.Telephony.SMS_DELIVER";

            /**
             * Broadcast Action: A new text-based SMS message has been received
             * by the device. This intent will only be delivered to a
             * carrier app which is responsible for filtering the message.
             * If the carrier app wants to drop a message, it should set the result
             * code to {@link android.app.Activity#RESULT_CANCELED}. The carrier app can
             * also modify the SMS PDU by setting the "pdus" value in result extras.</p>
             *
             * The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>"pdus"</em> - An Object[] of Byte[]s containing the PDUs
             *   that make up the message.</li>
             *   <li><em>"format"</em> - A String describing the format of the PDUs. It can
             *   be either "3gpp" or "3gpp2".</li>
             *   <li><em>"destport"</em> - An Int32 describing the destination port of a data
             *   SMS. It will be -1 for text SMS.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must be a carrier privileged app.
             * It must also declare {@link Manifest::permission::BROADCAST_SMS} as a required
             * permission in the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">
             * {@code &lt;receiver>}</a> tag.
             * {@hide}
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_FILTER_ACTION =
                    "android.provider.Telephony.SMS_FILTER";

            /**
             * Broadcast Action: A new text-based SMS message has been received
             * by the device. This intent will be delivered to all registered
             * receivers as a notification. These apps are not expected to write the
             * message or notify the user. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>"pdus"</em> - An Object[] of Byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_RECEIVED";

            /**
             * Broadcast Action: A new data based SMS message has been received
             * by the device. This intent will be delivered to all registered
             * receivers as a notification. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>"pdus"</em> - An Object[] of Byte[]s containing the PDUs
             *   that make up the message.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String DATA_SMS_RECEIVED_ACTION =
                    "android.intent.action.DATA_SMS_RECEIVED";

            /**
             * Broadcast Action: A new WAP PUSH message has been received by the
             * device. This intent will only be delivered to the default
             * sms app. That app is responsible for writing the message and notifying
             * the user. The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>"transactionId"</em> - (Integer) The WAP transaction ID</li>
             *   <li><em>"pduType"</em> - (Integer) The WAP PDU type</li>
             *   <li><em>"header"</em> - (Byte[]) The header of the message</li>
             *   <li><em>"data"</em> - (Byte[]) The data payload of the message</li>
             *   <li><em>"contentTypeParameters" </em>
             *   -(HashMap&lt;String,String&gt;) Any parameters associated with the content type
             *   (decoded from the WSP Content-Type header)</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             *
             * <p>The contentTypeParameters extra value is map of content parameters keyed by
             * their names.</p>
             *
             * <p>If any unassigned well-known parameters are encountered, the key of the map will
             * be 'unassigned/0x...', where '...' is the hex value of the unassigned parameter.  If
             * a parameter has No-Value the value in the map will be NULL.</p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must declare
             * {@link Manifest::permission::BROADCAST_WAP_PUSH} as a required permission in
             * the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">{@code
             * &lt;receiver>}</a> tag.
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String WAP_PUSH_DELIVER_ACTION =
                    "android.provider.Telephony.WAP_PUSH_DELIVER";

            /**
             * Broadcast Action: A new WAP PUSH message has been received by the
             * device. This intent will be delivered to all registered
             * receivers as a notification. These apps are not expected to write the
             * message or notify the user. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>"transactionId"</em> - (Integer) The WAP transaction ID</li>
             *   <li><em>"pduType"</em> - (Integer) The WAP PDU type</li>
             *   <li><em>"header"</em> - (Byte[]) The header of the message</li>
             *   <li><em>"data"</em> - (Byte[]) The data payload of the message</li>
             *   <li><em>"contentTypeParameters"</em>
             *   - (HashMap&lt;String,String&gt;) Any parameters associated with the content type
             *   (decoded from the WSP Content-Type header)</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             *
             * <p>The contentTypeParameters extra value is map of content parameters keyed by
             * their names.</p>
             *
             * <p>If any unassigned well-known parameters are encountered, the key of the map will
             * be 'unassigned/0x...', where '...' is the hex value of the unassigned parameter.  If
             * a parameter has No-Value the value in the map will be NULL.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String WAP_PUSH_RECEIVED_ACTION =
                    "android.provider.Telephony.WAP_PUSH_RECEIVED";

            /**
             * Broadcast Action: A new Cell Broadcast message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>"message"</em> - An SmsCbMessage object containing the broadcast message
             *   data. This is not an emergency alert, so ETWS and CMAS data will be NULL.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_CB_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_CB_RECEIVED";

            /**
             * Broadcast Action: A new Emergency Broadcast message has been received
             * by the device. The intent will have the following extra
             * values:</p>
             *
             * <ul>
             *   <li><em>"message"</em> - An SmsCbMessage object containing the broadcast message
             *   data, including ETWS or CMAS warning notification info if present.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_EMERGENCY_CB_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_EMERGENCY_CB_RECEIVED";

            /**
             * Broadcast Action: A new CDMA SMS has been received containing Service Category
             * Program Data (updates the list of enabled broadcast channels). The intent will
             * have the following extra values:</p>
             *
             * <ul>
             *   <li><em>"operations"</em> - An array of CdmaSmsCbProgramData objects containing
             *   the service category Operations (add/delete/clear) to perform.</li>
             * </ul>
             *
             * <p>The extra values can be extracted using
             * {@link #GetMessagesFromIntent(Intent)}.</p>
             *
             * <p>If a BroadcastReceiver encounters an error while processing
             * this intent it should set the result code appropriately.</p>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED_ACTION =
                    "android.provider.Telephony.SMS_SERVICE_CATEGORY_PROGRAM_DATA_RECEIVED";

            /**
             * Broadcast Action: The SIM storage for SMS messages is full.  If
             * space is not freed, messages targeted for the SIM (class 2) may
             * not be saved.
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SIM_FULL_ACTION =
                    "android.provider.Telephony.SIM_FULL";

            /**
             * Broadcast Action: An incoming SMS has been rejected by the
             * telephony framework.  This intent is sent in lieu of any
             * of the RECEIVED_ACTION intents.  The intent will have the
             * following extra value:</p>
             *
             * <ul>
             *   <li><em>"result"</em> - An Int32 result code, e.g. {@link #RESULT_SMS_OUT_OF_MEMORY}
             *   indicating the error returned to the network.</li>
             * </ul>
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_REJECTED_ACTION =
                "android.provider.Telephony.SMS_REJECTED";

            /**
             * Broadcast Action: A new SMS PDU needs to be sent from
             * the device. This intent will only be delivered to a
             * carrier app. That app is responsible for sending the PDU.
             * The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>"pdu"</em> - (Byte[]) The PDU to send.</li>
             *   <li><em>"smsc"</em> - (Byte[]) The service center Address (for GSM PDU only).</li>
             *   <li><em>"format"</em> - (String) The format of the PDU. Either 3gpp or 3gpp2. </li>
             *   <li><em>"concat.refNumber"</em> - (Int32) If the SMS is part of a multi-part SMS, the
             *   ref number used in the SMS header.</li>
             *   <li><em>"concat.seqNumber"</em> - (Int32) If the SMS is part of a multi-part SMS, the
             *   sequence number of this SMS.</li>
             *   <li><em>"concat.msgCount"</em> - (Int32) If the SMS is part of a multi-part SMS, the
             *   total number of SMSes in the multi-part SMS.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver is trying to send the message,
             *  it should set the result code to {@link android.app.Activity#RESULT_OK} and set
             *  the following in the result extra values:</p>
             *
             * <ul>
             *   <li><em>"messageref"</em> - (Int32) The new message reference number which will be
             *   later used in the updateSmsSendStatus call.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver cannot send the message, it should not set the result
             *  code and the platform will send it via the normal pathway.
             * </p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must be a carrier privileged app.
             * It must also declare {@link Manifest::permission::BROADCAST_SMS} as a required
             * permission in the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">
             * {@code &lt;receiver>}</a> tag.
             * {@hide}
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String SMS_SEND_ACTION =
                "android.provider.Telephony.SMS_SEND";

            /**
             * Read the PDUs out of an {@link #SMS_RECEIVED_ACTION} or a
             * {@link #DATA_SMS_RECEIVED_ACTION} intent.
             *
             * @param intent the intent to read from
             * @return an array of SmsMessages for the PDUs
             */
            public static SmsMessage[] GetMessagesFromIntent(Intent intent) {
                Object[] messages = (Object[]) intent->GetSerializableExtra("pdus");
                String format = intent->GetStringExtra("format");
                Int64 subId = intent->GetLongExtra(PhoneConstants.SUBSCRIPTION_KEY,
                        SubscriptionManager->GetDefaultSmsSubId());

                Rlog->V(TAG, " getMessagesFromIntent sub_id : " + subId);

                Int32 pduCount = messages.length;
                SmsMessage[] msgs = new SmsMessage[pduCount];

                For (Int32 i = 0; i < pduCount; i++) {
                    Byte[] pdu = (Byte[]) messages[i];
                    msgs[i] = SmsMessage->CreateFromPdu(pdu, format);
                    msgs[i].SetSubId(subId);
                }
                return msgs;
            }

            /**
             * Read the normalized addresses out of PDUs
             * @param pdus bytes for PDUs
             * @param format the format of the message
             * @return a list of Addresses for the PDUs
             * @hide
             */
            public static List<String> GetNormalizedAddressesFromPdus(Byte[][] pdus,
                    String format) {
                Int32 pduCount = pdus.length;
                SmsMessage[] msgs = new SmsMessage[pduCount];
                List<String> addresses = new ArrayList<String>();

                For (Int32 i = 0; i < pduCount; i++) {
                    Byte[] pdu = (Byte[]) pdus[i];
                    msgs[i] = SmsMessage->CreateFromPdu(pdu, format);
                    // If the originating address is NULL on our message
                    // then the format for SmsMessage createFromPdu is likely
                    // incorrect. SmsMessage CreateFromPdu(the new method)
                    // takes in a format parameter that it gets from the Tracker
                    // however, on some of our legacy devices using a legacy ril,
                    // since that format is derived by getting voice tech,
                    // we can get a bad format and no valid members.
                    // Thus we introduce a hack that utilizes the deprecated
                    // SmsMessage.createFromPdu if we get a NULL originating address.
                    If (msgs[i].GetOriginatingAddress() == NULL) {
                        msgs[i] = SmsMessage->CreateFromPdu(pdu);
                    }
                    String originatingAddress = msgs[i].GetOriginatingAddress();
                    If (!TextUtils->IsEmpty(originatingAddress)) {
                        String normalized = NormalizeDigitsOnly(originatingAddress);
                        addresses->Add(normalized);
                    }
                }
                return addresses;
            }

            private static String NormalizeDigitsOnly(String number) {
                return NormalizeDigits(number, FALSE /* strip non-digits */).ToString();
            }

            private static StringBuilder NormalizeDigits(String number, Boolean keepNonDigits) {
                StringBuilder normalizedDigits = new StringBuilder(number->Length());
                For (Char32 c : number->ToCharArray()) {
                    Int32 digit = Character->Digit(c, 10);
                    If (digit != -1) {
                        normalizedDigits->Append(digit);
                    } else If (keepNonDigits) {
                        normalizedDigits->Append(c);
                    }
                }
                return normalizedDigits;
            }

        }
    }

    /**
     * Base columns for tables that contain MMSs.
     */
    public interface BaseMmsColumns extends BaseColumns {

        /** Message box: all messages. */
        public static const Int32 MESSAGE_BOX_ALL    = 0;
        /** Message box: inbox. */
        public static const Int32 MESSAGE_BOX_INBOX  = 1;
        /** Message box: sent messages. */
        public static const Int32 MESSAGE_BOX_SENT   = 2;
        /** Message box: drafts. */
        public static const Int32 MESSAGE_BOX_DRAFTS = 3;
        /** Message box: outbox. */
        public static const Int32 MESSAGE_BOX_OUTBOX = 4;
        /** Message box: failed. */
        public static const Int32 MESSAGE_BOX_FAILED = 5;

        /**
         * The thread ID of the message.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String THREAD_ID = "thread_id";

        /**
         * The date the message was received.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DATE = "date";

        /**
         * The date the message was sent.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DATE_SENT = "date_sent";

        /**
         * The box which the message belongs to, e.g. {@link #MESSAGE_BOX_INBOX}.
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_BOX = "msg_box";

        /**
         * Has the message been read?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String READ = "read";

        /**
         * Has the message been seen by the user? The "seen" flag determines
         * whether we need to show a new message notification.
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String SEEN = "seen";

        /**
         * Does the message have only a text Part (can also have a subject) with
         * no picture, slideshow, sound, etc. parts?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String TEXT_ONLY = "text_only";

        /**
         * The {@code Message-ID} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String MESSAGE_ID = "m_id";

        /**
         * The subject of the message, if present.
         * <P>Type: TEXT</P>
         */
        public static const String SUBJECT = "sub";

        /**
         * The character set of the subject, if present.
         * <P>Type: INTEGER</P>
         */
        public static const String SUBJECT_CHARSET = "sub_cs";

        /**
         * The {@code Content-Type} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String CONTENT_TYPE = "ct_t";

        /**
         * The {@code Content-Location} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String CONTENT_LOCATION = "ct_l";

        /**
         * The expiry time of the message.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String EXPIRY = "exp";

        /**
         * The class of the message.
         * <P>Type: TEXT</P>
         */
        public static const String MESSAGE_CLASS = "m_cls";

        /**
         * The type of the message defined by MMS spec.
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_TYPE = "m_type";

        /**
         * The version of the specification that this message conforms to.
         * <P>Type: INTEGER</P>
         */
        public static const String MMS_VERSION = "v";

        /**
         * The size of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_SIZE = "m_size";

        /**
         * The priority of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String PRIORITY = "pri";

        /**
         * The {@code read-report} of the message.
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String READ_REPORT = "rr";

        /**
         * Is read report allowed?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String REPORT_ALLOWED = "rpt_a";

        /**
         * The {@code response-status} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String RESPONSE_STATUS = "resp_st";

        /**
         * The {@code status} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String STATUS = "st";

        /**
         * The {@code transaction-id} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String TRANSACTION_ID = "tr_id";

        /**
         * The {@code retrieve-status} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String RETRIEVE_STATUS = "retr_st";

        /**
         * The {@code retrieve-text} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String RETRIEVE_TEXT = "retr_txt";

        /**
         * The character set of the retrieve-text.
         * <P>Type: INTEGER</P>
         */
        public static const String RETRIEVE_TEXT_CHARSET = "retr_txt_cs";

        /**
         * The {@code read-status} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String READ_STATUS = "read_status";

        /**
         * The {@code content-class} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String CONTENT_CLASS = "ct_cls";

        /**
         * The {@code delivery-report} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String DELIVERY_REPORT = "d_rpt";

        /**
         * The {@code delivery-time-token} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String DELIVERY_TIME_TOKEN = "d_tm_tok";

        /**
         * The {@code delivery-time} of the message.
         * <P>Type: INTEGER</P>
         */
        public static const String DELIVERY_TIME = "d_tm";

        /**
         * The {@code response-text} of the message.
         * <P>Type: TEXT</P>
         */
        public static const String RESPONSE_TEXT = "resp_txt";

        /**
         * The {@code sender-visibility} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String SENDER_VISIBILITY = "s_vis";

        /**
         * The {@code reply-charging} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_CHARGING = "r_chg";

        /**
         * The {@code reply-charging-deadline-token} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_CHARGING_DEADLINE_TOKEN = "r_chg_dl_tok";

        /**
         * The {@code reply-charging-deadline} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_CHARGING_DEADLINE = "r_chg_dl";

        /**
         * The {@code reply-charging-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_CHARGING_ID = "r_chg_id";

        /**
         * The {@code reply-charging-size} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_CHARGING_SIZE = "r_chg_sz";

        /**
         * The {@code previously-sent-by} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String PREVIOUSLY_SENT_BY = "p_s_by";

        /**
         * The {@code previously-sent-date} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String PREVIOUSLY_SENT_DATE = "p_s_d";

        /**
         * The {@code store} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String STORE = "store";

        /**
         * The {@code mm-state} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MM_STATE = "mm_st";

        /**
         * The {@code mm-flags-token} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MM_FLAGS_TOKEN = "mm_flg_tok";

        /**
         * The {@code mm-flags} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MM_FLAGS = "mm_flg";

        /**
         * The {@code store-status} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String STORE_STATUS = "store_st";

        /**
         * The {@code store-status-text} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String STORE_STATUS_TEXT = "store_st_txt";

        /**
         * The {@code stored} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String STORED = "stored";

        /**
         * The {@code totals} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String TOTALS = "totals";

        /**
         * The {@code mbox-totals} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MBOX_TOTALS = "mb_t";

        /**
         * The {@code mbox-totals-token} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MBOX_TOTALS_TOKEN = "mb_t_tok";

        /**
         * The {@code quotas} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String QUOTAS = "qt";

        /**
         * The {@code mbox-quotas} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MBOX_QUOTAS = "mb_qt";

        /**
         * The {@code mbox-quotas-token} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MBOX_QUOTAS_TOKEN = "mb_qt_tok";

        /**
         * The {@code message-count} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String MESSAGE_COUNT = "m_cnt";

        /**
         * The {@code start} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String START = "start";

        /**
         * The {@code distribution-indicator} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String DISTRIBUTION_INDICATOR = "d_ind";

        /**
         * The {@code element-descriptor} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String ELEMENT_DESCRIPTOR = "e_des";

        /**
         * The {@code limit} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String LIMIT = "limit";

        /**
         * The {@code recommended-retrieval-mode} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String RECOMMENDED_RETRIEVAL_MODE = "r_r_mod";

        /**
         * The {@code recommended-retrieval-mode-text} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String RECOMMENDED_RETRIEVAL_MODE_TEXT = "r_r_mod_txt";

        /**
         * The {@code status-text} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String STATUS_TEXT = "st_txt";

        /**
         * The {@code applic-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String APPLIC_ID = "apl_id";

        /**
         * The {@code reply-applic-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLY_APPLIC_ID = "r_apl_id";

        /**
         * The {@code aux-applic-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String AUX_APPLIC_ID = "aux_apl_id";

        /**
         * The {@code drm-content} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String DRM_CONTENT = "drm_c";

        /**
         * The {@code adaptation-allowed} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String ADAPTATION_ALLOWED = "adp_a";

        /**
         * The {@code replace-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String REPLACE_ID = "repl_id";

        /**
         * The {@code cancel-id} of the message.
         * <P>Type: TEXT</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String CANCEL_ID = "cl_id";

        /**
         * The {@code cancel-status} of the message.
         * <P>Type: INTEGER</P>
         * @deprecated this column is no longer supported.
         * @hide
         */
        //@Deprecated
        public static const String CANCEL_STATUS = "cl_st";

        /**
         * Is the message locked?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String LOCKED = "locked";

        /**
         * The phone id to which message belongs to
         * <p>Type: INTEGER</p>
         * @hide
         */
        public static const String PHONE_ID = "phone_id";

        /**
         * The identity of the sender of a sent message. It is
         * usually the package name of the app which sends the message.
         * <p>Type: TEXT</p>
         */
        public static const String CREATOR = "creator";
    }

    /**
     * Columns for the "canonical_addresses" table used by MMS and SMS.
     */
    public interface CanonicalAddressesColumns extends BaseColumns {
        /**
         * An address used in MMS or SMS.  Email addresses are
         * converted to lower case and are compared by string
         * equality.  Other addresses are compared using
         * PHONE_NUMBERS_EQUAL.
         * <P>Type: TEXT</P>
         */
        public static const String ADDRESS = "address";
    }

    /**
     * Columns for the "threads" table used by MMS and SMS.
     */
    public interface ThreadsColumns extends BaseColumns {

        /**
         * The date at which the thread was created.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DATE = "date";

        /**
         * A string encoding of the recipient IDs of the recipients of
         * the message, in numerical order and separated by spaces.
         * <P>Type: TEXT</P>
         */
        public static const String RECIPIENT_IDS = "recipient_ids";

        /**
         * The message count of the thread.
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_COUNT = "message_count";

        /**
         * Indicates whether all messages of the thread have been read.
         * <P>Type: INTEGER</P>
         */
        public static const String READ = "read";

        /**
         * The snippet of the latest message in the thread.
         * <P>Type: TEXT</P>
         */
        public static const String SNIPPET = "snippet";

        /**
         * The charset of the snippet.
         * <P>Type: INTEGER</P>
         */
        public static const String SNIPPET_CHARSET = "snippet_cs";

        /**
         * Type of the thread, either {@link Threads#COMMON_THREAD} or
         * {@link Threads#BROADCAST_THREAD}.
         * <P>Type: INTEGER</P>
         */
        public static const String TYPE = "type";

        /**
         * Indicates whether there is a transmission error in the thread.
         * <P>Type: INTEGER</P>
         */
        public static const String ERROR = "error";

        /**
         * Indicates whether this thread contains any attachments.
         * <P>Type: INTEGER</P>
         */
        public static const String HAS_ATTACHMENT = "has_attachment";

        /**
         * If the thread is archived
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String ARCHIVED = "archived";
    }

    /**
     * Helper functions for the "threads" table used by MMS and SMS.
     */
    public class Threads implements ThreadsColumns {

        private static const String[] ID_PROJECTION = { BaseColumns._ID };

        /**
         * Private {@code content://} style URL for this table. Used by
         * {@link #GetOrCreateThreadId(android.content.Context, java.util.Set)}.
         */
        private static const Uri THREAD_ID_CONTENT_URI = Uri->Parse(
                "content://mms-sms/threadID");

        /**
         * The {@code content://} style URL for this table, by conversation.
         */
        public static const Uri CONTENT_URI = Uri->WithAppendedPath(
                MmsSms.CONTENT_URI, "conversations");

        /**
         * The {@code content://} style URL for this table, for obsolete threads.
         */
        public static const Uri OBSOLETE_THREADS_URI = Uri->WithAppendedPath(
                CONTENT_URI, "obsolete");

        /** Thread type: common thread. */
        public static const Int32 COMMON_THREAD    = 0;

        /** Thread type: broadcast thread. */
        public static const Int32 BROADCAST_THREAD = 1;

        /**
         * Not instantiable.
         * @hide
         */
        private Threads() {
        }

        /**
         * This is a single-recipient version of {@code getOrCreateThreadId}.
         * It's convenient for use with SMS messages.
         * @param context the context object to use.
         * @param recipient the recipient to send to.
         * @hide
         */
        public static Int64 GetOrCreateThreadId(Context context, String recipient) {
            Set<String> recipients = new HashSet<String>();

            recipients->Add(recipient);
            return GetOrCreateThreadId(context, recipients);
        }

        /**
         * Given the recipients list and subject of an unsaved message,
         * return its thread ID.  If the message starts a new thread,
         * allocate a new thread ID.  Otherwise, use the appropriate
         * existing thread ID.
         *
         * <p>Find the thread ID of the same set of Recipients (in any order,
         * without any additions). If one is found, return it. Otherwise,
         * return a unique thread ID.</p>
         * @hide
         */
        public static Int64 GetOrCreateThreadId(
                Context context, Set<String> recipients) {
            Uri.Builder uriBuilder = THREAD_ID_CONTENT_URI->BuildUpon();

            For (String recipient : recipients) {
                If (Mms->IsEmailAddress(recipient)) {
                    recipient = Mms->ExtractAddrSpec(recipient);
                }

                uriBuilder->AppendQueryParameter("recipient", recipient);
            }

            Uri uri = uriBuilder->Build();
            //If (DEBUG) Rlog->V(TAG, "getOrCreateThreadId uri: " + uri);

            Cursor cursor = SqliteWrapper->Query(context, context->GetContentResolver(),
                    uri, ID_PROJECTION, NULL, NULL, NULL);
            If (cursor != NULL) {
                try {
                    If (cursor->MoveToFirst()) {
                        return cursor->GetLong(0);
                    } else {
                        Rlog->E(TAG, "getOrCreateThreadId returned no rows!");
                    }
                } finally {
                    cursor->Close();
                }
            }

            Rlog->E(TAG, "getOrCreateThreadId failed with uri " + uri->ToString());
            throw new IllegalArgumentException("Unable to find or allocate a thread ID.");
        }
    }

    /**
     * Contains all MMS messages.
     */
    public class Mms implements BaseMmsColumns {

        /**
         * Not instantiable.
         * @hide
         */
        private Mms() {
        }

        /**
         * The {@code content://} URI for this table.
         */
        public static const Uri CONTENT_URI = Uri->Parse("content://mms");

        /**
         * Content URI for getting MMS report requests.
         */
        public static const Uri REPORT_REQUEST_URI = Uri->WithAppendedPath(
                                            CONTENT_URI, "report-request");

        /**
         * Content URI for getting MMS report status.
         */
        public static const Uri REPORT_STATUS_URI = Uri->WithAppendedPath(
                                            CONTENT_URI, "report-status");

        /**
         * The default sort order for this table.
         */
        public static const String DEFAULT_SORT_ORDER = "date DESC";

        /**
         * Regex pattern for names and email addresses.
         * <ul>
         *     <li><em>mailbox</em> = {@code name-addr}</li>
         *     <li><em>name-addr</em> = {@code [display-name] angle-addr}</li>
         *     <li><em>angle-addr</em> = {@code [CFWS] "<" addr-spec ">" [CFWS]}</li>
         * </ul>
         * @hide
         */
        public static const Pattern NAME_ADDR_EMAIL_PATTERN =
                Pattern->Compile("\\s*(\"[^\"]*\"|[^<>\"]+)\\s*<([^<>]+)>\\s*");

        /**
         * Helper method to query this table.
         * @hide
         */
        public static Cursor Query(
                ContentResolver cr, String[] projection) {
            return cr->Query(CONTENT_URI, projection, NULL, NULL, DEFAULT_SORT_ORDER);
        }

        /**
         * Helper method to query this table.
         * @hide
         */
        public static Cursor Query(
                ContentResolver cr, String[] projection,
                String where, String orderBy) {
            return cr->Query(CONTENT_URI, projection,
                    where, NULL, orderBy == NULL ? DEFAULT_SORT_ORDER : orderBy);
        }

        /**
         * Helper method to extract email address from address string.
         * @hide
         */
        public static String ExtractAddrSpec(String address) {
            Matcher match = NAME_ADDR_EMAIL_PATTERN->Matcher(address);

            If (match->Matches()) {
                return match->Group(2);
            }
            return address;
        }

        /**
         * Is the specified address an email address?
         *
         * @param address the input address to test
         * @return TRUE if address is an email address; FALSE otherwise.
         * @hide
         */
        public static Boolean IsEmailAddress(String address) {
            If (TextUtils->IsEmpty(address)) {
                return FALSE;
            }

            String s = ExtractAddrSpec(address);
            Matcher match = Patterns.EMAIL_ADDRESS->Matcher(s);
            return match->Matches();
        }

        /**
         * Is the specified number a phone number?
         *
         * @param number the input number to test
         * @return TRUE if number is a phone number; FALSE otherwise.
         * @hide
         */
        public static Boolean IsPhoneNumber(String number) {
            If (TextUtils->IsEmpty(number)) {
                return FALSE;
            }

            Matcher match = Patterns.PHONE->Matcher(number);
            return match->Matches();
        }

        /**
         * Contains all MMS messages in the MMS app inbox.
         */
        public class Inbox implements BaseMmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Inbox() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri
                    CONTENT_URI = Uri->Parse("content://mms/inbox");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app sent folder.
         */
        public class Sent implements BaseMmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Sent() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri
                    CONTENT_URI = Uri->Parse("content://mms/sent");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app drafts folder.
         */
        public class Draft implements BaseMmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Draft() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri
                    CONTENT_URI = Uri->Parse("content://mms/drafts");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains all MMS messages in the MMS app outbox.
         */
        public class Outbox implements BaseMmsColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Outbox() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri
                    CONTENT_URI = Uri->Parse("content://mms/outbox");

            /**
             * The default sort order for this table.
             */
            public static const String DEFAULT_SORT_ORDER = "date DESC";
        }

        /**
         * Contains address information for an MMS message.
         */
        public class Addr implements BaseColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Addr() {
            }

            /**
             * The ID of MM which this address entry belongs to.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String MSG_ID = "msg_id";

            /**
             * The ID of contact entry in Phone Book.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String CONTACT_ID = "contact_id";

            /**
             * The address text.
             * <P>Type: TEXT</P>
             */
            public static const String ADDRESS = "address";

            /**
             * Type of address: must be one of {@code PduHeaders.BCC},
             * {@code PduHeaders.CC}, {@code PduHeaders.FROM}, {@code PduHeaders.TO}.
             * <P>Type: INTEGER</P>
             */
            public static const String TYPE = "type";

            /**
             * Character set of this Entry (MMS charset value).
             * <P>Type: INTEGER</P>
             */
            public static const String CHARSET = "charset";
        }

        /**
         * Contains message parts.
         */
        public class Part implements BaseColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private Part() {
            }

            /**
             * The identifier of the message which this part belongs to.
             * <P>Type: INTEGER</P>
             */
            public static const String MSG_ID = "mid";

            /**
             * The order of the part.
             * <P>Type: INTEGER</P>
             */
            public static const String SEQ = "seq";

            /**
             * The content type of the part.
             * <P>Type: TEXT</P>
             */
            public static const String CONTENT_TYPE = "ct";

            /**
             * The name of the part.
             * <P>Type: TEXT</P>
             */
            public static const String NAME = "name";

            /**
             * The charset of the part.
             * <P>Type: TEXT</P>
             */
            public static const String CHARSET = "chset";

            /**
             * The file name of the part.
             * <P>Type: TEXT</P>
             */
            public static const String FILENAME = "fn";

            /**
             * The content disposition of the part.
             * <P>Type: TEXT</P>
             */
            public static const String CONTENT_DISPOSITION = "cd";

            /**
             * The content ID of the part.
             * <P>Type: INTEGER</P>
             */
            public static const String CONTENT_ID = "cid";

            /**
             * The content location of the part.
             * <P>Type: INTEGER</P>
             */
            public static const String CONTENT_LOCATION = "cl";

            /**
             * The start of content-type of the message.
             * <P>Type: INTEGER</P>
             */
            public static const String CT_START = "ctt_s";

            /**
             * The type of content-type of the message.
             * <P>Type: TEXT</P>
             */
            public static const String CT_TYPE = "ctt_t";

            /**
             * The Location (on filesystem) of the binary data of the part.
             * <P>Type: INTEGER</P>
             */
            public static const String _DATA = "_data";

            /**
             * The message text.
             * <P>Type: TEXT</P>
             */
            public static const String TEXT = "text";
        }

        /**
         * Message send rate table.
         */
        public class Rate {

            /**
             * Not instantiable.
             * @hide
             */
            private Rate() {
            }

            /**
             * The {@code content://} style URL for this table.
             */
            public static const Uri CONTENT_URI = Uri->WithAppendedPath(
                    Mms.CONTENT_URI, "rate");

            /**
             * When a message was successfully sent.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String SENT_TIME = "sent_time";
        }

        /**
         * Intents class.
         */
        public class Intents {

            /**
             * Not instantiable.
             * @hide
             */
            private Intents() {
            }

            /**
             * Indicates that the contents of specified URIs were changed.
             * The application which is showing or caching these contents
             * should be updated.
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String CONTENT_CHANGED_ACTION
                    = "android.intent.action.CONTENT_CHANGED";

            /**
             * Broadcast Action: A new MMS PDU needs to be sent from
             * the device. This intent will only be delivered to a
             * carrier app. That app is responsible for sending the PDU.
             * The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>{@link #EXTRA_MMS_CONTENT_URI}</em> - (Uri) The content provider of the
             *     PDU to send.</li>
             *   <li><em>{@link #EXTRA_MMS_LOCATION_URL}</em> - (String) The optional url to send
             *     this MMS PDU. If this is not specified, PDU should be sent to the default MMSC
             *     url.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver is trying to send the message,
             *  it should set the result code to {@link android.app.Activity#RESULT_OK} and set
             *  the following in the result extra values:</p>
             *
             * <ul>
             *   <li><em>"messageref"</em> - (Int32) The new message reference number which will be
             *   later used in the updateMmsSendStatus call.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver cannot send the message, it should not set the result
             *  code and the platform will send it via the normal pathway.
             * </p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must be a carrier privileged app.
             * It must also declare {@link Manifest::permission::BROADCAST_WAP_PUSH} as a required
             * permission in the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">
             * {@code &lt;receiver>}</a> tag.
             * {@hide}
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String MMS_SEND_ACTION =
                    "android.provider.Telephony.MMS_SEND";

            /**
             * Broadcast Action: A new MMS needs to be downloaded.
             * This intent will only be delivered to a
             * carrier app. That app is responsible for downloading the message at the URL.
             * The intent will have the following extra values:</p>
             *
             * <ul>
             *   <li><em>{@link #EXTRA_MMS_CONTENT_URI}</em> - (Uri) The content provider of the
             *     PDU to be downloaded.</li>
             *   <li><em>{@link #EXTRA_MMS_LOCATION_URL}</em> - (String) The message URL to be
             *     downloaded.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver is trying to download the message,
             *  it should set the result code to {@link android.app.Activity#RESULT_OK} and set
             *  the following in the result extra values:</p>
             *
             * <ul>
             *   <li><em>"messageref"</em> - (Int32) The new message reference number which will be
             *   later used in the updateMmsDownloadStatus call.</li>
             * </ul>
             *
             * <p>If a BroadcastReceiver cannot download the message, it should not set the result
             *  code and the platform will download it via the normal pathway.
             * </p>
             *
             * <p class="note"><strong>Note:</strong>
             * The broadcast receiver that filters for this intent must be a carrier privileged app.
             * It must also declare {@link Manifest::permission::BROADCAST_WAP_PUSH} as a required
             * permission in the <a href="{@docRoot}guide/topics/manifest/receiver-element.html">
             * {@code &lt;receiver>}</a> tag.
             * {@hide}
             */
            @SdkConstant(SdkConstantType.BROADCAST_INTENT_ACTION)
            public static const String MMS_DOWNLOAD_ACTION =
                    "android.provider.Telephony.MMS_DOWNLOAD";

            /**
             * An extra field which stores the URI of deleted contents.
             */
            public static const String DELETED_CONTENTS = "deleted_contents";

            /**
             * The content provider of the PDU to be sent/downloaded passed as an extra for
             * {@link #MMS_DOWNLOAD_ACTION} and {@link #MMS_SEND_ACTION}.
             * {@hide}
             */
            public static const String EXTRA_MMS_CONTENT_URI =
                    "android.provider.Telephony.extra.MMS_CONTENT_URI";

            /**
             * The message URL to be downloaded passed as an extra for {@link #MMS_DOWNLOAD_ACTION}.
             * It is also the URL to send an MMS to passed as an extra for
             * {@link #MMS_SEND_ACTION}.
             * {@hide}
             */
            public static const String EXTRA_MMS_LOCATION_URL =
                    "android.provider.Telephony.extra.MMS_LOCATION_URL";
        }
    }

    /**
     * Contains all MMS and SMS messages.
     */
    public class MmsSms implements BaseColumns {

        /**
         * Not instantiable.
         * @hide
         */
        private MmsSms() {
        }

        /**
         * The column that specifies number of messages of type.
         * @hide
         */
        public static const String TRANSPORT_TYPE_COUNT_COLUMN =
                "transport_type_count";

        /**
         * The column to distinguish SMS and MMS messages in query results.
         */
        public static const String TYPE_DISCRIMINATOR_COLUMN =
                "transport_type";

        /**
         * The {@code content://} style URL for this table.
         */
        public static const Uri CONTENT_URI = Uri->Parse("content://mms-sms/");

        /**
         * The {@code content://} style URL for this table, by conversation.
         */
        public static const Uri CONTENT_CONVERSATIONS_URI = Uri->Parse(
                "content://mms-sms/conversations");

        /**
         * The {@code content://} style URL for this table, by phone number.
         */
        public static const Uri CONTENT_FILTER_BYPHONE_URI = Uri->Parse(
                "content://mms-sms/messages/byphone");

        /**
         * The {@code content://} style URL for undelivered messages in this table.
         */
        public static const Uri CONTENT_UNDELIVERED_URI = Uri->Parse(
                "content://mms-sms/undelivered");

        /**
         * The {@code content://} style URL for draft messages in this table.
         */
        public static const Uri CONTENT_DRAFT_URI = Uri->Parse(
                "content://mms-sms/draft");

        /**
         * The {@code content://} style URL for locked messages in this table.
         */
        public static const Uri CONTENT_LOCKED_URI = Uri->Parse(
                "content://mms-sms/locked");

        /**
         * Pass in a query parameter called "pattern" which is the text to search for.
         * The sort order is fixed to be: {@code thread_id ASC, date DESC}.
         */
        public static const Uri SEARCH_URI = Uri->Parse(
                "content://mms-sms/search");

        // Constants for message protocol types.

        /** SMS protocol type. */
        public static const Int32 SMS_PROTO = 0;

        /** MMS protocol type. */
        public static const Int32 MMS_PROTO = 1;

        // Constants for error types of pending messages.

        /** Error type: no error. */
        public static const Int32 NO_ERROR                      = 0;

        /** Error type: generic transient error. */
        public static const Int32 ERR_TYPE_GENERIC              = 1;

        /** Error type: SMS protocol transient error. */
        public static const Int32 ERR_TYPE_SMS_PROTO_TRANSIENT  = 2;

        /** Error type: MMS protocol transient error. */
        public static const Int32 ERR_TYPE_MMS_PROTO_TRANSIENT  = 3;

        /** Error type: transport failure. */
        public static const Int32 ERR_TYPE_TRANSPORT_FAILURE    = 4;

        /** Error type: permanent Error (along with all higher error values). */
        public static const Int32 ERR_TYPE_GENERIC_PERMANENT    = 10;

        /** Error type: SMS protocol permanent error. */
        public static const Int32 ERR_TYPE_SMS_PROTO_PERMANENT  = 11;

        /** Error type: MMS protocol permanent error. */
        public static const Int32 ERR_TYPE_MMS_PROTO_PERMANENT  = 12;

        /**
         * Contains pending messages info.
         */
        public class PendingMessages implements BaseColumns {

            /**
             * Not instantiable.
             * @hide
             */
            private PendingMessages() {
            }

            public static const Uri CONTENT_URI = Uri->WithAppendedPath(
                    MmsSms.CONTENT_URI, "pending");

            /**
             * The type of transport Protocol (MMS or SMS).
             * <P>Type: INTEGER</P>
             */
            public static const String PROTO_TYPE = "proto_type";

            /**
             * The ID of the message to be sent or downloaded.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String MSG_ID = "msg_id";

            /**
             * The type of the message to be sent or downloaded.
             * This field is only valid for MM. For SM, its value is always set to 0.
             * <P>Type: INTEGER</P>
             */
            public static const String MSG_TYPE = "msg_type";

            /**
             * The type of the error code.
             * <P>Type: INTEGER</P>
             */
            public static const String ERROR_TYPE = "err_type";

            /**
             * The error code of sending/retrieving process.
             * <P>Type: INTEGER</P>
             */
            public static const String ERROR_CODE = "err_code";

            /**
             * How many times we tried to send or download the message.
             * <P>Type: INTEGER</P>
             */
            public static const String RETRY_INDEX = "retry_index";

            /**
             * The time to do next retry.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String DUE_TIME = "due_time";

            /**
             * The time we last tried to send or download the message.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String LAST_TRY = "last_try";

            /**
             * The phone id to which the pending message belongs to
             * <p>Type: INTEGER (Int64) </p>
             * @hide
             */
            public static const String PHONE_ID = "pending_phone_id";
        }

        /**
         * Words table used by provider for full-text searches.
         * @hide
         */
        public class WordsTable {

            /**
             * Not instantiable.
             * @hide
             */
            private WordsTable() {}

            /**
             * Primary key.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String ID = "_id";

            /**
             * Source row ID.
             * <P>Type: INTEGER (Int64)</P>
             */
            public static const String SOURCE_ROW_ID = "source_id";

            /**
             * Table ID (either 1 or 2).
             * <P>Type: INTEGER</P>
             */
            public static const String TABLE_ID = "table_to_use";

            /**
             * The words to index.
             * <P>Type: TEXT</P>
             */
            public static const String INDEXED_TEXT = "index_text";
        }
    }

    /**
     * Carriers class contains information about APNs, including MMSC information.
     */
    public class Carriers implements BaseColumns {

        /**
         * Not instantiable.
         * @hide
         */
        private Carriers() {}

        /**
         * The {@code content://} style URL for this table.
         */
        public static const Uri CONTENT_URI = Uri->Parse("content://telephony/carriers");

        /**
         * The default sort order for this table.
         */
        public static const String DEFAULT_SORT_ORDER = "name ASC";

        /**
         * Entry name.
         * <P>Type: TEXT</P>
         */
        public static const String NAME = "name";

        /**
         * APN name.
         * <P>Type: TEXT</P>
         */
        public static const String APN = "apn";

        /**
         * Proxy address.
         * <P>Type: TEXT</P>
         */
        public static const String PROXY = "proxy";

        /**
         * Proxy port.
         * <P>Type: TEXT</P>
         */
        public static const String PORT = "port";

        /**
         * MMS proxy address.
         * <P>Type: TEXT</P>
         */
        public static const String MMSPROXY = "mmsproxy";

        /**
         * MMS proxy port.
         * <P>Type: TEXT</P>
         */
        public static const String MMSPORT = "mmsport";

        /**
         * Server address.
         * <P>Type: TEXT</P>
         */
        public static const String SERVER = "server";

        /**
         * APN username.
         * <P>Type: TEXT</P>
         */
        public static const String USER = "user";

        /**
         * APN password.
         * <P>Type: TEXT</P>
         */
        public static const String PASSWORD = "password";

        /**
         * MMSC URL.
         * <P>Type: TEXT</P>
         */
        public static const String MMSC = "mmsc";

        /**
         * Mobile Country Code (MCC).
         * <P>Type: TEXT</P>
         */
        public static const String MCC = "mcc";

        /**
         * Mobile Network Code (MNC).
         * <P>Type: TEXT</P>
         */
        public static const String MNC = "mnc";

        /**
         * Numeric operator ID (as String). Usually {@code MCC + MNC}.
         * <P>Type: TEXT</P>
         */
        public static const String NUMERIC = "numeric";

        /**
         * Authentication type.
         * <P>Type:  INTEGER</P>
         */
        public static const String AUTH_TYPE = "authtype";

        /**
         * Comma-delimited list of APN types.
         * <P>Type: TEXT</P>
         */
        public static const String TYPE = "type";

        /**
         * The protocol to use to connect to this APN.
         *
         * One of the {@code PDP_type} values in TS 27.007 section 10.1.1.
         * For example: {@code IP}, {@code IPV6}, {@code IPV4V6}, or {@code PPP}.
         * <P>Type: TEXT</P>
         */
        public static const String PROTOCOL = "protocol";

        /**
         * The protocol to use to connect to this APN when roaming.
         * The syntax is the same as protocol.
         * <P>Type: TEXT</P>
         */
        public static const String ROAMING_PROTOCOL = "roaming_protocol";

        /**
         * Is this the current APN?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String CURRENT = "current";

        /**
         * Is this APN enabled?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String CARRIER_ENABLED = "carrier_enabled";

        /**
         * Radio Access Technology info.
         * To check what values are allowed, refer to {@link android.telephony.ServiceState}.
         * This should be spread to other technologies,
         * but is currently only used for LTE (14) and EHRPD (13).
         * <P>Type: INTEGER</P>
         */
        public static const String BEARER = "bearer";

        /**
         * MVNO type:
         * {@code SPN (Service Provider Name), IMSI, GID (Group Identifier Level 1)}.
         * <P>Type: TEXT</P>
         */
        public static const String MVNO_TYPE = "mvno_type";

        /**
         * MVNO data.
         * Use the following examples.
         * <ul>
         *     <li>SPN: A MOBILE, BEN NL, ...</li>
         *     <li>IMSI: 302720x94, 2060188, ...</li>
         *     <li>GID: 4E, 33, ...</li>
         * </ul>
         * <P>Type: TEXT</P>
         */
        public static const String MVNO_MATCH_DATA = "mvno_match_data";

        /**
         * The sub_id to which the APN belongs to
         * <p>Type: INTEGER (Int64) </p>
         * @hide
         */
        public static const String SUB_ID = "sub_id";

        /**
         * The profile_id to which the APN saved in modem
         * <p>Type: INTEGER</p>
         *@hide
         */
        public static const String PROFILE_ID = "profile_id";

        /**
         * Is the apn setting to be set in modem
         * <P>Type: INTEGER (Boolean)</P>
         *@hide
         */
        public static const String MODEM_COGNITIVE = "modem_cognitive";

        /**
         * The max connections of this apn
         * <p>Type: INTEGER</p>
         *@hide
         */
        public static const String MAX_CONNS = "max_conns";

        /**
         * The wait time for retry of the apn
         * <p>Type: INTEGER</p>
         *@hide
         */
        public static const String WAIT_TIME = "wait_time";

        /**
         * The time to limit max connection for the apn
         * <p>Type: INTEGER</p>
         *@hide
         */
        public static const String MAX_CONNS_TIME = "max_conns_time";

        /**
         * The MTU size of the mobile interface to  which the APN connected
         * <p>Type: INTEGER </p>
         * @hide
         */
        public static const String MTU = "mtu";
    }

    /**
     * Contains received SMS cell broadcast messages.
     * @hide
     */
    public class CellBroadcasts implements BaseColumns {

        /**
         * Not instantiable.
         * @hide
         */
        private CellBroadcasts() {}

        /**
         * The {@code content://} URI for this table.
         */
        public static const Uri CONTENT_URI = Uri->Parse("content://cellbroadcasts");

        /**
         * Message geographical scope.
         * <P>Type: INTEGER</P>
         */
        public static const String GEOGRAPHICAL_SCOPE = "geo_scope";

        /**
         * Message serial number.
         * <P>Type: INTEGER</P>
         */
        public static const String SERIAL_NUMBER = "serial_number";

        /**
         * PLMN of broadcast sender. {@code SERIAL_NUMBER + PLMN + LAC + CID} uniquely identifies
         * a broadcast for duplicate detection purposes.
         * <P>Type: TEXT</P>
         */
        public static const String PLMN = "plmn";

        /**
         * Location Area (GSM) or Service Area (UMTS) of broadcast sender. Unused for CDMA.
         * Only included if Geographical Scope of message is not PLMN Wide (01).
         * <P>Type: INTEGER</P>
         */
        public static const String LAC = "lac";

        /**
         * Cell ID of message Sender (GSM/UMTS). Unused for CDMA. Only included when the
         * Geographical Scope of message is cell Wide (00 or 11).
         * <P>Type: INTEGER</P>
         */
        public static const String CID = "cid";

        /**
         * Message code. <em>OBSOLETE: merged into SERIAL_NUMBER.</em>
         * <P>Type: INTEGER</P>
         */
        public static const String V1_MESSAGE_CODE = "message_code";

        /**
         * Message identifier. <em>OBSOLETE: renamed to SERVICE_CATEGORY.</em>
         * <P>Type: INTEGER</P>
         */
        public static const String V1_MESSAGE_IDENTIFIER = "message_id";

        /**
         * Service Category (GSM/UMTS: message identifier; CDMA: service category).
         * <P>Type: INTEGER</P>
         */
        public static const String SERVICE_CATEGORY = "service_category";

        /**
         * Message language code.
         * <P>Type: TEXT</P>
         */
        public static const String LANGUAGE_CODE = "language";

        /**
         * Message body.
         * <P>Type: TEXT</P>
         */
        public static const String MESSAGE_BODY = "body";

        /**
         * Message delivery time.
         * <P>Type: INTEGER (Int64)</P>
         */
        public static const String DELIVERY_TIME = "date";

        /**
         * Has the message been viewed?
         * <P>Type: INTEGER (Boolean)</P>
         */
        public static const String MESSAGE_READ = "read";

        /**
         * Message Format (3GPP or 3GPP2).
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_FORMAT = "format";

        /**
         * Message Priority (including emergency).
         * <P>Type: INTEGER</P>
         */
        public static const String MESSAGE_PRIORITY = "priority";

        /**
         * ETWS warning Type (ETWS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String ETWS_WARNING_TYPE = "etws_warning_type";

        /**
         * CMAS message Class (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_MESSAGE_CLASS = "cmas_message_class";

        /**
         * CMAS Category (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_CATEGORY = "cmas_category";

        /**
         * CMAS response Type (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_RESPONSE_TYPE = "cmas_response_type";

        /**
         * CMAS Severity (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_SEVERITY = "cmas_severity";

        /**
         * CMAS Urgency (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_URGENCY = "cmas_urgency";

        /**
         * CMAS Certainty (CMAS alerts only).
         * <P>Type: INTEGER</P>
         */
        public static const String CMAS_CERTAINTY = "cmas_certainty";

        /** The default sort order for this table. */
        public static const String DEFAULT_SORT_ORDER = DELIVERY_TIME + " DESC";

        /**
         * Query columns for instantiating {@link android.telephony.CellBroadcastMessage} objects.
         */
        public static const String[] QUERY_COLUMNS = {
                _ID,
                GEOGRAPHICAL_SCOPE,
                PLMN,
                LAC,
                CID,
                SERIAL_NUMBER,
                SERVICE_CATEGORY,
                LANGUAGE_CODE,
                MESSAGE_BODY,
                DELIVERY_TIME,
                MESSAGE_READ,
                MESSAGE_FORMAT,
                MESSAGE_PRIORITY,
                ETWS_WARNING_TYPE,
                CMAS_MESSAGE_CLASS,
                CMAS_CATEGORY,
                CMAS_RESPONSE_TYPE,
                CMAS_SEVERITY,
                CMAS_URGENCY,
                CMAS_CERTAINTY
        };
    }

    /**
     * Contains phone numbers that are blacklisted
     * for phone and/or message purposes.
     * @hide
     */
    public class Blacklist implements BaseColumns {
        /**
         * The content:// style URL for this table
         */
        public static const Uri CONTENT_URI =

                Uri->Parse("content://blacklist");

        /**
         * The content:// style URL for filtering this table by number.
         * When using this, make sure the number is correctly encoded
         * when appended to the Uri.
         */
        public static const Uri CONTENT_FILTER_BYNUMBER_URI =
                Uri->Parse("content://blacklist/bynumber");

        /**
         * The content:// style URL for filtering this table on phone numbers
         */
        public static const Uri CONTENT_PHONE_URI =
                Uri->Parse("content://blacklist/phone");

        /**
         * The content:// style URL for filtering this table on message numbers
         */
        public static const Uri CONTENT_MESSAGE_URI =
                Uri->Parse("content://blacklist/message");


        /**
         * Query parameter used to match numbers by regular-expression like
         * matching. Supported are the '*' and the '.' operators.
         * <p>
         * TYPE: Boolean
         */
        public static const String REGEX_KEY = "regex";

        /**
         * The default sort order for this table
         */
        public static const String DEFAULT_SORT_ORDER = "number ASC";

        /**
         * The phone number as the user entered it.
         * <P>Type: TEXT</P>
         */
        public static const String NUMBER = "number";

        /**
         * Whether the number contains a regular expression pattern
         * <P>Type: BOOLEAN (read only)</P>
         */
        public static const String IS_REGEX = "is_regex";

        /**
         * Blacklisting mode for phone calls
         * <P>Type: INTEGER (Int32)</P>
         */
        public static const String PHONE_MODE = "phone";

        /**
         * Blacklisting mode for messages
         * <P>Type: INTEGER (Int32)</P>
         */
        public static const String MESSAGE_MODE = "message";
    }
}
