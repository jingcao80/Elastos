#ifndef __ELASTOS_DROID_PROVIDER_CTELEPHONYSMS_H__
#define __ELASTOS_DROID_PROVIDER_CTELEPHONYSMS_H__

#include "_Elastos_Droid_Provider_CTelephonySms.h"
#include "elastos/core/Singleton.h"

using Elastos::Droid::Net::IUri;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ICursor;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CTelephonySms)
    , public Singleton
    , public ITelephonySms
    , public IBaseColumns
    , public ITelephonyTextBasedSmsColumns
{
public:
    CAR_INTERFACE_DECL();
    CAR_SINGLETON_DECL();

    /**
     * Used to determine the currently configured default SMS package.
     * @param context context of the requesting application
     * @return package name for the default SMS package or NULL
     */
    CARAPI GetDefaultSmsPackage(
        /* [in] */ IContext* context,
        /* [out] */ String* smsPackage);

    /**
     * Return cursor for table query.
     * @hide
     */
    CARAPI Query(
        /* [in] */ IContentResolver* cr,
        /* [in] */ ArrayOf<String>* projection,
        /* [out] */ ICursor** cursor);

    /**
     * Return cursor for table query.
     * @hide
     */
    CARAPI Query(
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
    CARAPI AddMessageToUri(
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
    CARAPI AddMessageToUri(
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
    CARAPI AddMessageToUri(
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
    CARAPI AddMessageToUri(
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
    CARAPI AddMessageToUri(
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
    CARAPI MoveMessageToFolder(
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
    CARAPI IsOutgoingFolder(
        /* [in] */ Int32 messageType,
        /* [out] */ Boolean* result);

    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

};

} // namespace Provider
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_PROVIDER_CTELEPHONYSMS_H__
