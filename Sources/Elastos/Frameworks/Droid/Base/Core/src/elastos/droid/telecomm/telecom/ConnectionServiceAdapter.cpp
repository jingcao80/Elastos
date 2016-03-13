
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/telecomm/telecom/ConnectionServiceAdapter.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Droid::Telecomm::Internal::IIVideoProvider;
using Elastos::Utility::IIterator;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Telecomm {
namespace Telecom {

//===============================================================
// ConnectionServiceAdapter::
//===============================================================
CAR_INTERFACE_IMPL_2(ConnectionServiceAdapter, Object, IConnectionServiceAdapter, IProxyDeathRecipient)

ConnectionServiceAdapter::ConnectionServiceAdapter()
{
    assert(0 && "TODO");
    // mAdapters = Collections.newSetFromMap(
    //     new ConcurrentHashMap<IConnectionServiceAdapter, Boolean>(8, 0.9f, 1));
}

ECode ConnectionServiceAdapter::AddAdapter(
    /* [in] */ IIConnectionServiceAdapter* adapter)
{
    Boolean bAdd = FALSE;
    if ((mAdapters->Add(adapter, &bAdd), bAdd)) {
        AutoPtr<IProxy> proAdapter = (IProxy*)adapter->Probe(EIID_IProxy);
        if (proAdapter == NULL) {
            mAdapters->Remove(adapter);
        }
        else if (FAILED(proAdapter->LinkToDeath(this, 0))) {
            mAdapters->Remove(adapter);
        }
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::RemoveAdapter(
    /* [in] */ IIConnectionServiceAdapter* adapter)
{
    if (adapter != NULL && mAdapters->Remove(adapter)) {
        AutoPtr<IProxy> proAdapter = (IProxy*)adapter->Probe(EIID_IProxy);
        if (proAdapter != NULL) {
            Boolean b = FALSE;
            proAdapter->UnlinkToDeath(this, 0, &b);
        }
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::ProxyDied()
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> adapter;
        it->GetNext((IInterface**)&adapter);
        AutoPtr<IProxy> proAdapter = (IProxy*)adapter->Probe(EIID_IProxy);
        if (proAdapter != NULL) {
            Boolean bAlive = FALSE;
            proAdapter->IsStubAlive(&bAlive);
            if (!bAlive) {
                it->Remove();
                Boolean b = FALSE;
                proAdapter->UnlinkToDeath(this, 0, &b);
            }
        }
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::HandleCreateConnectionComplete(
    /* [in] */ const String& id,
    /* [in] */ IConnectionRequest* request,
    /* [in] */ IParcelableConnection* connection)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->HandleCreateConnectionComplete(id, request, connection);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetActive(
    /* [in] */ const String& callId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetActive(callId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetRinging(
    /* [in] */ const String& callId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetRinging(callId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetDialing(
    /* [in] */ const String& callId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetDialing(callId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetDisconnected(
    /* [in] */ const String& callId,
    /* [in] */ IDisconnectCause* disconnectCause)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetDisconnected(callId, disconnectCause);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetOnHold(
    /* [in] */ const String& callId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetOnHold(callId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetRingbackRequested(
    /* [in] */ const String& callId,
    /* [in] */ Boolean ringback)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetRingbackRequested(callId, ringback);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetCallCapabilities(
    /* [in] */ const String& callId,
    /* [in] */ Int32 capabilities)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetCallCapabilities(callId, capabilities);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetIsConferenced(
    /* [in] */ const String& callId,
    /* [in] */ const String& conferenceCallId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        Logger::D("ConnectionServiceAdapter", "sending connection %s with conference %s", (const char*)callId, (const char*)conferenceCallId);
        adapter->SetIsConferenced(callId, conferenceCallId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::RemoveCall(
    /* [in] */ const String& callId)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->RemoveCall(callId);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::OnPostDialWait(
    /* [in] */ const String& callId,
    /* [in] */ const String& remaining)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->OnPostDialWait(callId, remaining);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::AddConferenceCall(
    /* [in] */ const String& callId,
    /* [in] */ IParcelableConference* parcelableConference)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->AddConferenceCall(callId, parcelableConference);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::QueryRemoteConnectionServices(
    /* [in] */ IRemoteServiceCallback* clb)
{
    // Only supported when there is only one adapter.
    Int32 size = 0;
    mAdapters->GetSize(&size);
    if (size == 1) {
        AutoPtr<IIterator> it;
        mAdapters->GetIterator((IIterator**)&it);
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> sa = IIConnectionServiceAdapter::Probe(p);
        if (FAILED(sa->QueryRemoteConnectionServices(clb))) {
            Logger::E("ConnectionServiceAdapter", "Exception trying to query for remote CSs");
        }
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetVideoProvider(
    /* [in] */ const String& callId,
    /* [in] */ IConnectionVideoProvider* videoProvider)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        AutoPtr<IIVideoProvider> vp;
        if (videoProvider != NULL) {
            videoProvider->GetInterface((IIVideoProvider**)&vp);
        }
        adapter->SetVideoProvider(callId, vp);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetIsVoipAudioMode(
    /* [in] */ const String& callId,
    /* [in] */ Boolean isVoip)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetIsVoipAudioMode(callId, isVoip);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetStatusHints(
    /* [in] */ const String& callId,
    /* [in] */ IStatusHints* statusHints)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetStatusHints(callId, statusHints);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetAddress(
    /* [in] */ const String& callId,
    /* [in] */ IUri* address,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetAddress(callId, address, presentation);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetCallerDisplayName(
    /* [in] */ const String& callId,
    /* [in] */ const String& callerDisplayName,
    /* [in] */ Int32 presentation)
{
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetCallerDisplayName(callId, callerDisplayName, presentation);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetVideoState(
    /* [in] */ const String& callId,
    /* [in] */ Int32 videoState)
{
    Logger::V("ConnectionServiceAdapter", "setVideoState: %d", videoState);
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetVideoState(callId, videoState);
    }
    return NOERROR;
}

ECode ConnectionServiceAdapter::SetConferenceableConnections(
    /* [in] */ const String& callId,
    /* [in] */ IList* conferenceableCallIds)
{
    Logger::V("ConnectionServiceAdapter", "setConferenceableConnections: %s, %p", (const char*)callId, conferenceableCallIds);
    AutoPtr<IIterator> it;
    mAdapters->GetIterator((IIterator**)&it);
    Boolean bHasNxt = FALSE;
    while ((it->HasNext(&bHasNxt), bHasNxt)) {
        AutoPtr<IInterface> p;
        it->GetNext((IInterface**)&p);
        AutoPtr<IIConnectionServiceAdapter> adapter = IIConnectionServiceAdapter::Probe(p);
        adapter->SetConferenceableConnections(callId, conferenceableCallIds);
    }
    return NOERROR;
}

} // namespace Telecom
} // namespace Telecomm
} // namespace Droid
} // namespace Elastos