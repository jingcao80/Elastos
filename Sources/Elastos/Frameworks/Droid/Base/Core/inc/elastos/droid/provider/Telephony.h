#ifndef __ELASTOS_DROID_PROVIDER_TELEPHONE_H__
#define __ELASTOS_DROID_PROVIDER_TELEPHONE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "elastos/core/Object.h"
#include "Elastos.Droid.Provider.h"
#include "elastos/core/StringBuilder.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Database::ICursor;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Telephony::ISmsMessage;
using Elastos::Core::StringBuilder;
using Elastos::Utility::IList;
using Elastos::Utility::ISet;
using Elastos::Utility::Regex::IPattern;

namespace Elastos {
namespace Droid {
namespace Provider {

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
 * Sms.Intents#WAP_PUSH_DELIVER_ACTION} ({@code "android.provider.Telephony.WAP_PUSH_DELIVER"})
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
class Telephony
    : public ITelephony
    , public Object
{
public:
    /**
     * Contains all text-based SMS messages.
     */
    class Sms
        : public ITelephonySms
        , public IBaseColumns
        , public ITelephonyTextBasedSmsColumns
        , public Object
    {
    public:
        /**
         * Contains all text-based SMS messages in the SMS app inbox.
         */
        class Inbox
            : public ITelephonySmsInbox
            , public IBaseColumns
            , public ITelephonyTextBasedSmsColumns
            , public Object
        {
        public:
            Inbox(
                /* [in] */ Sms* owner);

            CAR_INTERFACE_DECL();

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [in] */ Boolean read,
                /* [out] */ IUri** uri);

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ Int64 subId,
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [in] */ Boolean read,
                /* [out] */ IUri** uri);
        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms/inbox");

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Inbox() {
            }

            Sms* mOwner;

        };

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        class Sent
            : public ITelephonySmsSent
            , public IBaseColumns
            , public ITelephonyTextBasedSmsColumns
            , public Object
        {
        public:
            Sent(
                /* [in] */ Sms* owner);

            CAR_INTERFACE_DECL();

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [out] */ IUri** uri);

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ Int64 subId,
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [out] */ IUri** uri);
        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms/sent");

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Sent() {
            }

            Sms* mOwner;
        };

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        class Draft
            : public ITelephonySmsDraft
            , public IBaseColumns
            , public ITelephonyTextBasedSmsColumns
            , public Object
        {
        public:
            Draft(
                /* [in] */ Sms* owner);

            CAR_INTERFACE_DECL();

           /**
            * @hide
            */
            /*static*/ CARAPI AddMessage(
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [out] */ IUri** uri);

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ Int64 subId,
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [out] */ IUri** uri);

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms/draft");

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Draft() {
            }

            Sms* mOwner;
        };

        /**
         * Contains all pending outgoing text-based SMS messages.
         */
        class Outbox
            : public ITelephonySmsOutbox
            , public IBaseColumns
            , public ITelephonyTextBasedSmsColumns
            , public Object
        {
        public:
            Outbox(
                /* [in] */ Sms* owner);

            CAR_INTERFACE_DECL();

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [in] */ Boolean deliveryReport,
                /* [in] */ Int64 threadId,
                /* [out] */ IUri** uri);

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
            /*static*/ CARAPI AddMessage(
                /* [in] */ Int64 subId,
                /* [in] */ IContentResolver* resolver,
                /* [in] */ const String& address,
                /* [in] */ const String& body,
                /* [in] */ const String& subject,
                /* [in] */ Int64 date,
                /* [in] */ Boolean deliveryReport,
                /* [in] */ Int64 threadId,
                /* [out] */ IUri** uri);
        private:
            /**
             * Not instantiable.
             * @hide
             */
            Outbox() {
            }

            Sms* mOwner;
        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms/outbox");

        };

        /**
         * Contains all sent text-based SMS messages in the SMS app.
         */
        class Conversations
            : public ITelephonySmsConversations
            , public IBaseColumns
            , public ITelephonyTextBasedSmsColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Conversations() {
            }

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms/conversations");
        };

        /**
         * Contains constants for SMS related Intents that are broadcast.
         */
        class Intents
        {
        public:
            /**
             * Read the PDUs out of an {@link #SMS_RECEIVED_ACTION} or a
             * {@link #DATA_SMS_RECEIVED_ACTION} intent.
             *
             * @param intent the intent to read from
             * @return an array of SmsMessages for the PDUs
             */
            static CARAPI_(AutoPtr<ArrayOf<ISmsMessage*> >) GetMessagesFromIntent(
                /* [in] */ IIntent* intent);

            /**
             * Read the normalized addresses out of PDUs
             * @param pdus bytes for PDUs
             * @param format the format of the message
             * @return a list of Addresses for the PDUs
             * @hide
             */
            static CARAPI_(AutoPtr<IList>) GetNormalizedAddressesFromPdus(
                    /* [in] */ ArrayOf<ArrayOf<Byte>*> * pdus,
                    /* [in] */ const String& format);
        private:
            /**
             * Not instantiable.
             * @hide
             */
            Intents() {
            }


            static String NormalizeDigitsOnly(
                /* [in] */ const String& number);

            static AutoPtr<StringBuilder> NormalizeDigits(
                /* [in] */ const String& number,
                /* [in] */ Boolean keepNonDigits);
        };
    public:

        CAR_INTERFACE_DECL();

        /**
         * Used to determine the currently configured default SMS package.
         * @param context context of the requesting application
         * @return package name for the default SMS package or NULL
         */
        /*static*/ CARAPI GetDefaultSmsPackage(
            /* [in] */ IContext* context,
            /* [out] */ String* smsPackage);

        /**
         * Return cursor for table query.
         * @hide
         */
        /*static*/ CARAPI Query(
            /* [in] */ IContentResolver* cr,
            /* [in] */ ArrayOf<String>* projection,
            /* [out] */ ICursor** cursor);

        /**
         * Return cursor for table query.
         * @hide
         */
        /*static*/ CARAPI Query(
            /* [in] */ IContentResolver* cr,
            /* [in] */ ArrayOf<String>* projection,
            /* [in] */ const String& where,
            /* [in] */ const String& orderBy,
            /* [out] */ ICursor** cursor);

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
        /*static*/ CARAPI AddMessageToUri(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& address,
            /* [in] */ const String& body,
            /* [in] */ const String& subject,
            /* [in] */ Int64 date,
            /* [in] */ Boolean read,
            /* [in] */ Boolean deliveryReport,
            /* [out] */ IUri** _uri);

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
        /*static*/ CARAPI AddMessageToUri(
            /* [in] */ Int64 subId,
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& address,
            /* [in] */ const String& body,
            /* [in] */ const String& subject,
            /* [in] */ Int64 date,
            /* [in] */ Boolean read,
            /* [in] */ Boolean deliveryReport,
            /* [out] */ IUri** _uri);

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
        /*static*/ CARAPI AddMessageToUri(
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& address,
            /* [in] */ const String& body,
            /* [in] */ const String& subject,
            /* [in] */ Int64 date,
            /* [in] */ Boolean read,
            /* [in] */ Boolean deliveryReport,
            /* [in] */ Int64 threadId,
            /* [out] */ IUri** _uri);

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
        /*static*/ CARAPI AddMessageToUri(
            /* [in] */ Int64 subId,
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& address,
            /* [in] */ const String& body,
            /* [in] */ const String& subject,
            /* [in] */ Int64 date,
            /* [in] */ Boolean read,
            /* [in] */ Boolean deliveryReport,
            /* [in] */ Int64 threadId,
            /* [out] */ IUri** _uri);

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
        /*static*/ CARAPI AddMessageToUri(
            /* [in] */ Int64 subId,
            /* [in] */ IContentResolver* resolver,
            /* [in] */ IUri* uri,
            /* [in] */ const String& address,
            /* [in] */ const String& body,
            /* [in] */ const String& subject,
            /* [in] */ Int64 date,
            /* [in] */ Boolean read,
            /* [in] */ Boolean deliveryReport,
            /* [in] */ Int64 threadId,
            /* [in] */ Int32 priority,
            /* [out] */ IUri** _uri);

        /**
         * Move a message to the given folder.
         *
         * @param context the context to use
         * @param uri the message to move
         * @param folder the folder to move to
         * @return TRUE if the operation succeeded
         * @hide
         */
        /*static*/ CARAPI MoveMessageToFolder(
            /* [in] */ IContext* context,
            /* [in] */ IUri* uri,
            /* [in] */ Int32 folder,
            /* [in] */ Int32 error,
            /* [out] */ Boolean* result);

        /**
         * Returns TRUE iff the Folder (message type) identifies an
         * outgoing message.
         * @hide
         */
        /*static*/ CARAPI IsOutgoingFolder(
            /* [in] */ Int32 messageType,
            /* [out] */ Boolean* result);

    public:
        /**
         * The {@code content://} style URL for this table.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://sms");

    private:
        /**
         * Not instantiable.
         * @hide
         */
        Sms() {
        }

    };

    /**
     * Helper functions for the "threads" table used by MMS and SMS.
     */
    class Threads
        : public ITelephonyThreads
        , public ITelephonyThreadsColumns
        , public Object
    {
    public:
        CAR_INTERFACE_DECL()

        /**
         * This is a single-recipient version of {@code getOrCreateThreadId}.
         * It's convenient for use with SMS messages.
         * @param context the context object to use.
         * @param recipient the recipient to send to.
         * @hide
         */
        /*static*/ CARAPI GetOrCreateThreadId(
            /* [in] */ IContext* context,
            /* [in] */ const String& recipient,
            /* [out] */ Int64* threadId);

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
        /*static*/ CARAPI GetOrCreateThreadId(
            /* [in] */ IContext* context,
            /* [in] */ ISet* recipients,//String
            /* [out] */ Int64* threadId);
    private:
        /**
         * Not instantiable.
         * @hide
         */
        Threads() {
        }

    public:
        /**
         * The {@code content://} style URL for this table, by conversation.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->WithAppendedPath(MmsSms.CONTENT_URI, "conversations");

        /**
         * The {@code content://} style URL for this table, for obsolete threads.
         */
        static AutoPtr<IUri> OBSOLETE_THREADS_URI;// = Uri->WithAppendedPath(CONTENT_URI, "obsolete");

    private:
        static AutoPtr<ArrayOf<String> > ID_PROJECTION;// = { BaseColumns._ID };

        /**
         * Private {@code content://} style URL for this table. Used by
         * {@link #GetOrCreateThreadId(android.content.Context, java.util.Set)}.
         */
        static AutoPtr<IUri> THREAD_ID_CONTENT_URI;// = Uri->Parse("content://mms-sms/threadID");

    };

    /**
     * Contains all MMS messages.
     */
    class Mms
        : public ITelephonyMms
        , public ITelephonyBaseMmsColumns
        , public Object
    {
    public:
        /**
         * Contains all MMS messages in the MMS app inbox.
         */
        class Inbox
            : public ITelephonyMmsInbox
            , public ITelephonyBaseMmsColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Inbox() {
            }
        public:

            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms/inbox");

        };

        /**
         * Contains all MMS messages in the MMS app sent folder.
         */
        class Sent
            : public ITelephonyMmsSent
            , public ITelephonyBaseMmsColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:

            /**
             * Not instantiable.
             * @hide
             */
            Sent() {
            }

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms/sent");

        };

        /**
         * Contains all MMS messages in the MMS app drafts folder.
         */
        class Draft
            : public ITelephonyMmsDraft
            , public ITelephonyBaseMmsColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Draft() {
            }

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms/drafts");
        };

        /**
         * Contains all MMS messages in the MMS app outbox.
         */
        class Outbox
            : public ITelephonyMmsOutbox
            , public ITelephonyBaseMmsColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Outbox() {
            }

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms/outbox");
        };

        /**
         * Contains address information for an MMS message.
         */
        class Addr
            : public ITelephonyMmsAddr
            , public IBaseColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Addr() {
            }
        };

        /**
         * Contains message parts.
         */
        class Part
            : public ITelephonyMmsPart
            , public IBaseColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Part() {
            }

        };

        /**
         * Message send rate table.
         */
        class Rate
            : public ITelephonyMmsRate
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Rate() {
            }

        public:
            /**
             * The {@code content://} style URL for this table.
             */
            static AutoPtr<IUri> CONTENT_URI;// = Uri->WithAppendedPath(Mms.CONTENT_URI, "rate");
        };

        /**
         * Intents class.
         */
        class Intents
            : public ITelephonyMmsIntents
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            Intents() {
            }

        };

    public:
        CAR_INTERFACE_DECL();

        /**
         * Helper method to query this table.
         * @hide
         */
        /*static*/ CARAPI Query(
            /* [in] */ IContentResolver* cr,
            /* [in] */ ArrayOf<String>* projection,
            /* [out] */ ICursor** cursor);

        /**
         * Helper method to query this table.
         * @hide
         */
        /*static*/ CARAPI Query(
            /* [in] */ IContentResolver* cr,
            /* [in] */ ArrayOf<String>* projection,
            /* [in] */ const String& where,
            /* [in] */ const String& orderBy,
            /* [out] */ ICursor** cursor);

        /**
         * Helper method to extract email address from address string.
         * @hide
         */
        /*static*/ CARAPI ExtractAddrSpec(
            /* [in] */ const String& address,
            /* [out] */ String* addrSpec);

        /**
         * Is the specified address an email address?
         *
         * @param address the input address to test
         * @return TRUE if address is an email address; FALSE otherwise.
         * @hide
         */
        /*static*/ CARAPI IsEmailAddress(
            /* [in] */ const String& address,
            /* [out] */ Boolean* result);

        /**
         * Is the specified number a phone number?
         *
         * @param number the input number to test
         * @return TRUE if number is a phone number; FALSE otherwise.
         * @hide
         */
        /*static*/ CARAPI IsPhoneNumber(
            /* [in] */ const String& number,
            /* [out] */ Boolean* result);

    private:
        /**
         * Not instantiable.
         * @hide
         */
        Mms() {
        }

    public:
        /**
         * The {@code content://} URI for this table.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms");

        /**
         * Content URI for getting MMS report requests.
         */
        static AutoPtr<IUri> REPORT_REQUEST_URI;// = Uri->WithAppendedPath(CONTENT_URI, "report-request");

        /**
         * Content URI for getting MMS report status.
         */
        static AutoPtr<IUri> REPORT_STATUS_URI;// = Uri->WithAppendedPath(CONTENT_URI, "report-status");

        /**
         * Regex pattern for names and email addresses.
         * <ul>
         *     <li><em>mailbox</em> = {@code name-addr}</li>
         *     <li><em>name-addr</em> = {@code [display-name] angle-addr}</li>
         *     <li><em>angle-addr</em> = {@code [CFWS] "<" addr-spec ">" [CFWS]}</li>
         * </ul>
         * @hide
         */
        static AutoPtr<IPattern> NAME_ADDR_EMAIL_PATTERN;// = Pattern->Compile("\\s*(\"[^\"]*\"|[^<>\"]+)\\s*<([^<>]+)>\\s*");

    };

    /**
     * Contains all MMS and SMS messages.
     */
    class MmsSms
        : public ITelephonyMmsSms
        , public IBaseColumns
        , public Object
    {
    public:
        /**
         * Contains pending messages info.
         */
        class PendingMessages
            : public ITelephonyMmsSmsPendingMessages
            , public IBaseColumns
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            PendingMessages() {
            }
        public:

            static AutoPtr<IUri> CONTENT_URI;// = Uri->WithAppendedPath(MmsSms.CONTENT_URI, "pending");

        };

        /**
         * Words table used by provider for full-text searches.
         * @hide
         */
        class WordsTable
            : public ITelephonyMmsSmsWordsTable
            , public Object
        {
        public:
            CAR_INTERFACE_DECL();

        private:
            /**
             * Not instantiable.
             * @hide
             */
            WordsTable() {}

        };

    public:
        CAR_INTERFACE_DECL();

    private:
        /**
         * Not instantiable.
         * @hide
         */
        MmsSms() {
        }

    public:
        /**
         * The {@code content://} style URL for this table.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://mms-sms/");

        /**
         * The {@code content://} style URL for this table, by conversation.
         */
        static AutoPtr<IUri> CONTENT_CONVERSATIONS_URI;// = Uri->Parse("content://mms-sms/conversations");

        /**
         * The {@code content://} style URL for this table, by phone number.
         */
        static AutoPtr<IUri> CONTENT_FILTER_BYPHONE_URI;// = Uri->Parse("content://mms-sms/messages/byphone");

        /**
         * The {@code content://} style URL for undelivered messages in this table.
         */
        static AutoPtr<IUri> CONTENT_UNDELIVERED_URI;// = Uri->Parse("content://mms-sms/undelivered");

        /**
         * The {@code content://} style URL for draft messages in this table.
         */
        static AutoPtr<IUri> CONTENT_DRAFT_URI;// = Uri->Parse("content://mms-sms/draft");

        /**
         * The {@code content://} style URL for locked messages in this table.
         */
        static AutoPtr<IUri> CONTENT_LOCKED_URI;// = Uri->Parse("content://mms-sms/locked");

        /**
         * Pass in a query parameter called "pattern" which is the text to search for.
         * The sort order is fixed to be: {@code thread_id ASC, date DESC}.
         */
        static AutoPtr<IUri> SEARCH_URI;// = Uri->Parse("content://mms-sms/search");

    };

    /**
     * Carriers class contains information about APNs, including MMSC information.
     */
    class Carriers
        : public ITelephonyCarriers
        , public IBaseColumns
        , public Object
    {
    public:
        CAR_INTERFACE_DECL();

    private:
        /**
         * Not instantiable.
         * @hide
         */
        Carriers() {}

    public:
        /**
         * The {@code content://} style URL for this table.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://telephony/carriers");

    };

    /**
     * Contains received SMS cell broadcast messages.
     * @hide
     */
    class CellBroadcasts
        : public ITelephonyCellBroadcasts
        , public IBaseColumns
        , public Object
    {
    public:
        CAR_INTERFACE_DECL();

    private:
        /**
         * Not instantiable.
         * @hide
         */
        CellBroadcasts() {}

    public:
        /**
         * The {@code content://} URI for this table.
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://cellbroadcasts");

        /**
         * Query columns for instantiating {@link android.telephony.CellBroadcastMessage} objects.
         */
        static AutoPtr<ArrayOf<String> > QUERY_COLUMNS;
                // = {
                //_ID,
                //GEOGRAPHICAL_SCOPE,
                //PLMN,
                //LAC,
                //CID,
                //SERIAL_NUMBER,
                //SERVICE_CATEGORY,
                //LANGUAGE_CODE,
                //MESSAGE_BODY,
                //DELIVERY_TIME,
                //MESSAGE_READ,
                //MESSAGE_FORMAT,
                //MESSAGE_PRIORITY,
                //ETWS_WARNING_TYPE,
                //CMAS_MESSAGE_CLASS,
                //CMAS_CATEGORY,
                //CMAS_RESPONSE_TYPE,
                //CMAS_SEVERITY,
                //CMAS_URGENCY,
                //CMAS_CERTAINTY
                //};
    };

    /**
     * Contains phone numbers that are blacklisted
     * for phone and/or message purposes.
     * @hide
     */
    class Blacklist
        : public ITelephonyBlacklist
        , public IBaseColumns
        , public Object
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * The content:// style URL for this table
         */
        static AutoPtr<IUri> CONTENT_URI;// = Uri->Parse("content://blacklist");

        /**
         * The content:// style URL for filtering this table by number.
         * When using this, make sure the number is correctly encoded
         * when appended to the Uri.
         */
        static AutoPtr<IUri> CONTENT_FILTER_BYNUMBER_URI;// = Uri->Parse("content://blacklist/bynumber");

        /**
         * The content:// style URL for filtering this table on phone numbers
         */
        static AutoPtr<IUri> CONTENT_PHONE_URI;// = Uri->Parse("content://blacklist/phone");

        /**
         * The content:// style URL for filtering this table on message numbers
         */
        static AutoPtr<IUri> CONTENT_MESSAGE_URI;// = Uri->Parse("content://blacklist/message");

    };
private:
    static const String TAG;// = "Telephony";

    /**
     * Not instantiable.
     * @hide
     */
    Telephony() {
    }

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_TELEPHONE_H__

