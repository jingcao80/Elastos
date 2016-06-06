
#ifndef __ELASTOS_DROID_INTERNAL_TELEPHONY_INTRANGEMANAGER_H__
#define __ELASTOS_DROID_INTERNAL_TELEPHONY_INTRANGEMANAGER_H__

#include "_Elastos.Droid.Internal.h"
#include "Elastos.CoreLibrary.Utility.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Telephony {

/**
 * Clients can enable reception of SMS-CB messages for specific ranges of
 * message identifiers (channels). This class keeps track of the currently
 * enabled message identifiers and calls abstract methods to update the
 * radio when the range of enabled message identifiers changes.
 *
 * An update is a call to {@link #startUpdate} followed by zero or more
 * calls to {@link #addRange} followed by a call to {@link #finishUpdate}.
 * Calls to {@link #enableRange} and {@link #disableRange} will perform
 * an incremental update operation if the enabled ranges have changed.
 * A full update operation (i.e. after a radio reset) can be performed
 * by a call to {@link #updateRanges}.
 *
 * Clients are identified by String (the name associated with the User ID
 * of the caller) so that a call to remove a range can be mapped to the
 * client that enabled that range (or else rejected).
 */
class IntRangeManager
    : public Object
    , public IIntRangeManager
{
private:
    class ClientRange;

    /**
     * One or more clients forming the continuous range [startId, endId].
     * <p>When a client is added, the IntRange may merge with one or more
     * adjacent IntRanges to form a single combined IntRange.
     * <p>When a client is removed, the IntRange may divide into several
     * non-contiguous IntRanges.
     */
    class IntRange: public Object
    {
    public:
        /**
         * Create a new IntRange with a single client.
         * @param startId the first id included in the range
         * @param endId the last id included in the range
         * @param client the client requesting the enabled range
         */
        IntRange(
            /* [in] */ Int32 startId,
            /* [in] */ Int32 endId,
            /* [in] */ const String& client);

        /**
         * Create a new IntRange for an existing ClientRange.
         * @param clientRange the initial ClientRange to add
         */
        IntRange(
            /* [in] */ ClientRange* clientRange);

        /**
         * Create a new IntRange from an existing IntRange. This is used for
         * removing a ClientRange, because new IntRanges may need to be created
         * for any gaps that open up after the ClientRange is removed. A copy
         * is made of the elements of the original IntRange preceding the element
         * that is being removed. The following elements will be added to this
         * IntRange or to a new IntRange when a gap is found.
         * @param intRange the original IntRange to copy elements from
         * @param numElements the number of elements to copy from the original
         */
        IntRange(
            /* [in] */ IntRange* intRange,
            /* [in] */ Int32 numElements);

        /**
         * Insert new ClientRange in order by start id, then by end id
         * <p>If the new ClientRange is known to be sorted before or after the
         * existing ClientRanges, or at a particular index, it can be added
         * to the clients array list directly, instead of via this method.
         * <p>Note that this can be changed from linear to binary search if the
         * number of clients grows large enough that it would make a difference.
         * @param range the new ClientRange to insert
         */
        CARAPI_(void) Insert(
            /* [in] */ ClientRange* range);

    public:
        Int32 mStartId;
        Int32 mEndId;
        // sorted by earliest start id
        AutoPtr<IArrayList> mClients; //ArrayList<ClientRange>
    };

    /**
     * The message id range for a single client.
     */
    class ClientRange
        : public Object
        , public IClientRange
    {
    public:
        CAR_INTERFACE_DECL()

        ClientRange(
            /* [in] */ Int32 startId,
            /* [in] */ Int32 endId,
            /* [in] */ const String& client);

        // @Override
        CARAPI Equals(
            /* [in] */ IInterface* o,
            /* [out] */ Boolean* result);

        // @Override
        CARAPI GetHashCode(
            /* [out] */ Int32* code);

    public:
        Int32 mStartId;
        Int32 mEndId;
        String mClient;
    };

public:
    CAR_INTERFACE_DECL()

    /**
     * Enable a range for the specified client and update ranges
     * if necessary. If {@link #finishUpdate} returns failure,
     * FALSE is returned and the range is not added.
     *
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param client the client requesting the enabled range
     * @return TRUE if successful, FALSE otherwise
     */
    CARAPI EnableRange(
        /* [in] */ Int32 startId,
        /* [in] */ Int32 endId,
        /* [in] */ const String& client,
        /* [out] */ Boolean* result);

    /**
     * Disable a range for the specified client and update ranges
     * if necessary. If {@link #finishUpdate} returns failure,
     * FALSE is returned and the range is not removed.
     *
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param client the client requesting to disable the range
     * @return TRUE if successful, FALSE otherwise
     */
    CARAPI DisableRange(
        /* [in] */ Int32 startId,
        /* [in] */ Int32 endId,
        /* [in] */ const String& client,
        /* [out] */ Boolean* result);

    /**
     * Perform a complete update operation (enable all ranges). Useful
     * after a radio reset. Calls {@link #startUpdate}, followed by zero or
     * more calls to {@link #addRange}, followed by {@link #finishUpdate}.
     * @return TRUE if successful, FALSE otherwise
     */
    CARAPI UpdateRanges(
        /* [out] */ Boolean* result);

    /**
     * Returns whether the list of ranges is completely empty.
     * @return TRUE if there are no enabled ranges
     */
    CARAPI IsEmpty(
        /* [out] */ Boolean* result);

protected:
    IntRangeManager();

    /**
     * Enable or disable a single range of message identifiers.
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param selected TRUE to enable range, FALSE to disable range
     * @return TRUE if successful, FALSE otherwise
     */
    virtual CARAPI_(Boolean) TryAddRanges(
        /* [in] */ Int32 startId,
        /* [in] */ Int32 endId,
        /* [in] */ Boolean selected);

    /**
     * Called when the list of enabled ranges has changed. This will be
     * followed by zero or more calls to {@link #addRange} followed by
     * a call to {@link #finishUpdate}.
     */
    virtual CARAPI_(void) StartUpdate() = 0;

    /**
     * Called after {@link #startUpdate} to indicate a range of enabled
     * or disabled values.
     *
     * @param startId the first id included in the range
     * @param endId the last id included in the range
     * @param selected TRUE to enable range, FALSE to disable range
     */
    virtual CARAPI_(void) AddRange(
        /* [in] */ Int32 startId,
        /* [in] */ Int32 endId,
        /* [in] */ Boolean selected) = 0;

    /**
     * Called to indicate the end of a range update started by the
     * previous call to {@link #startUpdate}.
     * @return TRUE if successful, FALSE otherwise
     */
    virtual CARAPI_(Boolean) FinishUpdate() = 0;

private:
    /**
     * Called when attempting to add a single range of message identifiers
     * Populate all ranges of message identifiers.
     */
    CARAPI_(void) PopulateAllRanges();

    /**
     * Called when attempting to add a single range of message identifiers
     * Populate all ranges of message identifiers using clients' ranges.
     */
    CARAPI_(void) PopulateAllClientRanges();

private:
    /**
     * Initial capacity for IntRange clients array list. There will be
     * few cell broadcast listeners on a typical device, so this can be small.
     */
    static const Int32 INITIAL_CLIENTS_ARRAY_SIZE;

    /**
     * List of integer ranges, one per client, sorted by start id.
     */
    AutoPtr<IArrayList> mRanges; // ArrayList<IntRange>

    Object mLock;
};

} // namespace Telephony
} // namespace Internal
} // namespace Droid
} // namespace Elastos

DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Telephony::IntRangeManager::ClientRange, IInterface);
DEFINE_CONVERSION_FOR(Elastos::Droid::Internal::Telephony::IntRangeManager::IntRange, IInterface);

#endif // __ELASTOS_DROID_INTERNAL_TELEPHONY_INTRANGEMANAGER_H__
