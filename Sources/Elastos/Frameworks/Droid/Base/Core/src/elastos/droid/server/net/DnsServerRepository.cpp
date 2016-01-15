#include "elastos/droid/server/net/DnsServerRepository.h"
#include "elastos/droid/server/net/CDnsServerEntry.h"
#include <elastos/core/AutoLock.h>

using Elastos::Droid::Server::Net::CDnsServerEntry;
using Elastos::Droid::Server::Net::IDnsServerEntry;
using Elastos::Droid::Server::Net::IDnsServerRepository;
using Elastos::Core::AutoLock;
using Elastos::Core::CSystem;
using Elastos::Core::ISystem;
using Elastos::Net::CInetAddressHelper;
using Elastos::Net::IInetAddressHelper;
using Elastos::Utility::CArrayList;
using Elastos::Utility::CHashMap;
using Elastos::Utility::CHashSet;
using Elastos::Utility::CCollections;
using Elastos::Utility::ICollection;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Net {

const String DnsServerRepository::TAG("DnsServerRepository");

CAR_INTERFACE_IMPL(DnsServerRepository, Object, IDnsServerRepository)

DnsServerRepository::DnsServerRepository()
{
}

ECode DnsServerRepository::constructor()
{
    CHashSet::New((ISet**)&mCurrentServers);
    CArrayList::New(IDnsServerRepository::NUM_SERVERS, (IArrayList**)&mAllServers);
    CHashMap::New(IDnsServerRepository::NUM_SERVERS, (IHashMap**)&mIndex);
    return NOERROR;
}

ECode DnsServerRepository::SetDnsServersOn(
    /* [in] */ ILinkProperties* lp)
{
    synchronized(this) {
        lp->SetDnsServers(ICollection::Probe(mCurrentServers));
    }
    return NOERROR;
}

ECode DnsServerRepository::AddServers(
    /* [in] */ Int64 lifetime,
    /* [in] */ ArrayOf<String>* addresses,
    /* [out] */ Boolean* isAdded)
{
    VALIDATE_NOT_NULL(isAdded)
    synchronized(this) {
        // The lifetime is actually an unsigned 32-bit number, but Java doesn't have unsigned.
        // Technically 0xffffffff (the maximum) is special and means "forever", but 2^32 seconds
        // (136 years) is close enough.
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 now;
        sys->GetCurrentTimeMillis(&now);
        Int64 expiry = now + 1000 * lifetime;

        // Go through the list of servers. For each one, update the entry if one exists, and
        // create one if it doesn't.
        for (Int32 i = 0; i < addresses->GetLength(); i++) {
            String addressString = (*addresses)[i];
            AutoPtr<IInetAddressHelper> iah;
            CInetAddressHelper::AcquireSingleton((IInetAddressHelper**)&iah);
            AutoPtr<IInetAddress> address;
            FAIL_RETURN(iah->ParseNumericAddress(addressString, (IInetAddress**)&address));
            if (!UpdateExistingEntry(address.Get(), expiry)) {
                // There was no entry for this server. Create one, unless it's already expired
                // (i.e., if the lifetime is zero; it cannot be < 0 because it's unsigned).
                if (expiry > now) {
                    AutoPtr<IDnsServerEntry> entry;
                    CDnsServerEntry::New(address, expiry, (IDnsServerEntry**)&entry);
                    mAllServers->Add(entry);
                    mIndex->Put(address.Get(), entry.Get());
                }
            }
        }

        // Sort the servers by expiry.
        AutoPtr<ICollections> collections;
        CCollections::AcquireSingleton((ICollections**)&collections);
        collections->Sort(IList::Probe(mAllServers));

        // Prune excess entries and update the current server list.
        *isAdded = UpdateCurrentServers();
    }
    return NOERROR;
}

Boolean DnsServerRepository::UpdateExistingEntry(
    /* [in] */ IInetAddress* address,
    /* [in] */ Int64 expiry)
{
    synchronized(this) {
        AutoPtr<IInterface> obj;
        mIndex->Get(address, (IInterface**)&obj);
        AutoPtr<IDnsServerEntry> existing = IDnsServerEntry::Probe(obj);
        if (existing != NULL) {
            existing->SetExpiry(expiry);
            return TRUE;
        }
    }
    return FALSE;
}

Boolean DnsServerRepository::UpdateCurrentServers()
{
    synchronized(this) {
        AutoPtr<ISystem> sys;
        CSystem::AcquireSingleton((ISystem**)&sys);
        Int64 now;
        sys->GetCurrentTimeMillis(&now);
        Boolean changed = FALSE;

        // Prune excess or expired entries.
        Int32 size;
        mAllServers->GetSize(&size);
        for (Int32 i = size - 1; i >= 0; i--) {
            AutoPtr<IInterface> serverObj;
            mAllServers->Get(i, (IInterface**)&serverObj);
            AutoPtr<IDnsServerEntry> server = IDnsServerEntry::Probe(serverObj);
            Int64 expiry;
            server->GetExpiry(&expiry);
            if (i >= IDnsServerRepository::NUM_SERVERS || expiry < now) {
                AutoPtr<IInterface> removedObj;
                mAllServers->Remove(i, (IInterface**)&removedObj);
                AutoPtr<IDnsServerEntry> removed = IDnsServerEntry::Probe(removedObj);
                AutoPtr<IInetAddress> address;
                removed->GetAddress((IInetAddress**)&address);
                mIndex->Remove(address);
                Boolean modified;
                mCurrentServers->Remove(address, &modified);
                changed |= modified;
            } else {
                break;
            }
        }

        // Add servers to the current set, in order of decreasing lifetime, until it has enough.
        // Prefer existing servers over new servers in order to minimize updates to the rest of the
        // system and avoid persistent oscillations.
        mAllServers->GetSize(&size);
        for (Int32 i = 0; i < size; i++) {
        // for (DnsServerEntry entry : mAllServers) {
            AutoPtr<IInterface> entryObj;
            mAllServers->Get(i, (IInterface**)&entryObj);
            AutoPtr<IDnsServerEntry> entry = IDnsServerEntry::Probe(entryObj);
            Int32 cssize;
            mCurrentServers->GetSize(&cssize);
            if (cssize < IDnsServerRepository::NUM_CURRENT_SERVERS) {
                AutoPtr<IInetAddress> address;
                entry->GetAddress((IInetAddress**)&address);
                Boolean modified;
                mCurrentServers->Add(address, &modified);
                changed |= modified;
            } else {
                break;
            }
        }
        return changed;
    }
    return FALSE;
}

} // namespace Net
} // namespace Server
} // namespace Droid
} // namespace Elastos
