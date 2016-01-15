
#ifndef __ELASTOS_DROID_CONTENT_CSYNCSTATS_H__
#define __ELASTOS_DROID_CONTENT_CSYNCSTATS_H__

#include "_Elastos_Droid_Content_CSyncStats.h"
#include <elastos/core/Object.h>

namespace Elastos {
namespace Droid {
namespace Content {

CarClass(CSyncStats)
    , public Object
    , public ISyncStats
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CSyncStats();

    virtual ~CSyncStats();

    /**
     * The SyncAdapter was unable to authenticate the {@link android.accounts.Account}
     * that was specified in the request. The user needs to take some action to resolve
     * before a future request can expect to succeed. This is considered a hard error.
     */
    CARAPI GetNumAuthExceptions(
        /* [out] */ Int64* numAuthExceptions);

    CARAPI SetNumAuthExceptions(
        /* [in] */ Int64 numAuthExceptions);

    /**
     * The SyncAdapter had a problem, most likely with the network connectivity or a timeout
     * while waiting for a network response. The request may succeed if it is tried again
     * later. This is considered a soft error.
     */
    CARAPI GetNumIoExceptions(
        /* [out] */ Int64* numIoExceptions);

    CARAPI SetNumIoExceptions(
        /* [in] */ Int64 numIoExceptions);

    /**
     * The SyncAdapter had a problem with the data it received from the server or the storage
     * later. This problem will likely repeat if the request is tried again. The problem
     * will need to be cleared up by either the server or the storage layer (likely with help
     * from the user). If the SyncAdapter cleans up the data itself then it typically won't
     * increment this value although it may still do so in order to record that it had to
     * perform some cleanup. E.g., if the SyncAdapter received a bad entry from the server
     * when processing a feed of entries, it may choose to drop the entry and thus make
     * progress and still increment this value just so the SyncAdapter can record that an
     * error occurred. This is considered a hard error.
     */
    CARAPI GetNumParseExceptions(
        /* [out] */ Int64* numParseExceptions);

    CARAPI SetNumParseExceptions(
        /* [in] */ Int64 numParseExceptions);

    /**
     * The SyncAdapter detected that there was an unrecoverable version conflict when it
     * attempted to update or delete a version of a resource on the server. This is expected
     * to clear itself automatically once the new state is retrieved from the server,
     * though it may remain until the user intervenes manually, perhaps by clearing the
     * local storage and starting over frmo scratch. This is considered a hard error.
     */
    CARAPI GetNumConflictDetectedExceptions(
        /* [out] */ Int64* numConflictDetectedExceptions);

    CARAPI SetNumConflictDetectedExceptions(
        /* [in] */ Int64 numConflictDetectedExceptions);

    /**
     * Counter for tracking how many inserts were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    CARAPI GetNumInserts(
        /* [out] */ Int64* numInserts);

    CARAPI SetNumInserts(
        /* [in] */ Int64 numInserts);

    /**
     * Counter for tracking how many updates were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    CARAPI GetNumUpdates(
        /* [out] */ Int64* numUpdates);

    CARAPI SetNumUpdates(
        /* [in] */ Int64 numUpdates);

    /**
     * Counter for tracking how many deletes were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    CARAPI GetNumDeletes(
        /* [out] */ Int64* numDeletes);

    CARAPI SetNumDeletes(
        /* [in] */ Int64 numDeletes);

    /**
     * Counter for tracking how many entries were affected by the sync operation, as defined
     * by the SyncAdapter.
     */
    CARAPI GetNumEntries(
        /* [out] */ Int64* numEntries);

    CARAPI SetNumEntries(
        /* [in] */ Int64 numEntries);

    /**
     * Counter for tracking how many entries, either from the server or the local store, were
     * ignored during the sync operation. This could happen if the SyncAdapter detected some
     * unparsable data but decided to skip it and move on rather than failing immediately.
     */
    CARAPI GetNumSkippedEntries(
        /* [out] */ Int64* numSkippedEntries);

    CARAPI SetNumSkippedEntries(
        /* [in] */ Int64 numSkippedEntries);

    CARAPI ToString(
        /* [out] */ String* str);

    /**
     * Reset all the counters to 0.
     */
    CARAPI Clear();

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI constructor();

private:
    /**
     * The SyncAdapter was unable to authenticate the {@link android.accounts.Account}
     * that was specified in the request. The user needs to take some action to resolve
     * before a future request can expect to succeed. This is considered a hard error.
     */
    Int64 mNumAuthExceptions;

    /**
     * The SyncAdapter had a problem, most likely with the network connectivity or a timeout
     * while waiting for a network response. The request may succeed if it is tried again
     * later. This is considered a soft error.
     */
    Int64 mNumIoExceptions;

    /**
     * The SyncAdapter had a problem with the data it received from the server or the storage
     * later. This problem will likely repeat if the request is tried again. The problem
     * will need to be cleared up by either the server or the storage layer (likely with help
     * from the user). If the SyncAdapter cleans up the data itself then it typically won't
     * increment this value although it may still do so in order to record that it had to
     * perform some cleanup. E.g., if the SyncAdapter received a bad entry from the server
     * when processing a feed of entries, it may choose to drop the entry and thus make
     * progress and still increment this value just so the SyncAdapter can record that an
     * error occurred. This is considered a hard error.
     */
    Int64 mNumParseExceptions;

    /**
     * The SyncAdapter detected that there was an unrecoverable version conflict when it
     * attempted to update or delete a version of a resource on the server. This is expected
     * to clear itself automatically once the new state is retrieved from the server,
     * though it may remain until the user intervenes manually, perhaps by clearing the
     * local storage and starting over frmo scratch. This is considered a hard error.
     */
    Int64 mNumConflictDetectedExceptions;

    /**
     * Counter for tracking how many inserts were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    Int64 mNumInserts;

    /**
     * Counter for tracking how many updates were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    Int64 mNumUpdates;

    /**
     * Counter for tracking how many deletes were performed by the sync operation, as defined
     * by the SyncAdapter.
     */
    Int64 mNumDeletes;

    /**
     * Counter for tracking how many entries were affected by the sync operation, as defined
     * by the SyncAdapter.
     */
    Int64 mNumEntries;

    /**
     * Counter for tracking how many entries, either from the server or the local store, were
     * ignored during the sync operation. This could happen if the SyncAdapter detected some
     * unparsable data but decided to skip it and move on rather than failing immediately.
     */
    Int64 mNumSkippedEntries;

};

}
}
}

#endif // __ELASTOS_DROID_CONTENT_CSYNCSTATS_H__
