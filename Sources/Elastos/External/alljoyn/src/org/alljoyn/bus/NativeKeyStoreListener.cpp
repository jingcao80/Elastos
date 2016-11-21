
#include "org/alljoyn/bus/NativeKeyStoreListener.h"
#include <elastos/utility/logging/Logger.h>

using Elastos::Utility::Logging::Logger;

namespace Org {
namespace Alljoyn {
namespace Bus {

NativeKeyStoreListener::NativeKeyStoreListener(
    /* [in] */ IKeyStoreListener* listener)
{
    IWeakReferenceSource::Probe(listener)->GetWeakReference((IWeakReference**)&mKeyStoreListener);
}

NativeKeyStoreListener::~NativeKeyStoreListener()
{}

QStatus NativeKeyStoreListener::LoadRequest(
    /* [in] */ ajn::KeyStore& keyStore)
{
    AutoPtr<IKeyStoreListener> kl;
    mKeyStoreListener->Resolve(EIID_IKeyStoreListener, (IInterface**)&kl);
    if (kl == NULL) {
        Logger::E("NativeKeyStoreListener", "LoadRequest(): Can't get new local reference to SessionListener");
        return ER_FAIL;
    }

    AutoPtr< ArrayOf<Byte> > keys;
    ECode ec = kl->GetKeys((ArrayOf<Byte>**)&keys);
    if (FAILED(ec)) return ER_FAIL;

    qcc::String source;
    if (keys != NULL) {
        source = qcc::String((const char*)keys->GetPayload(), keys->GetLength());
    }

    AutoPtr< ArrayOf<Char32> > passwordChar;
    ec = kl->GetPassword((ArrayOf<Char32>**)&passwordChar);
    if (FAILED(ec) || passwordChar == NULL) return ER_FAIL;

    AutoPtr< ArrayOf<Byte> > password = String(*passwordChar).GetBytes();

    memset(passwordChar->GetPayload(), 0, passwordChar->GetLength() * sizeof(Char32));
    if (password == NULL) return ER_FAIL;

    QStatus status = PutKeys(keyStore, source, qcc::String((const char*)password->GetPayload(), password->GetLength()));
    memset(password->GetPayload(), 0, password->GetLength() * sizeof(Byte));

    return status;
}

QStatus NativeKeyStoreListener::StoreRequest(
    /* [in] */ ajn::KeyStore& keyStore)
{
    qcc::String sink;

    QStatus status = GetKeys(keyStore, sink);
    if (ER_OK != status) {
        return status;
    }

    AutoPtr< ArrayOf<Byte> > keys = String(sink.data(), sink.size()).GetBytes();
    if (keys == NULL) return ER_FAIL;

    AutoPtr<IKeyStoreListener> kl;
    mKeyStoreListener->Resolve(EIID_IKeyStoreListener, (IInterface**)&kl);
    if (kl == NULL) {
        Logger::E("NativeKeyStoreListener", "LoadRequest(): Can't get new local reference to SessionListener");
        return ER_FAIL;
    }

    ECode ec = kl->PutKeys(keys);
    if (FAILED(ec)) return ER_FAIL;

    return ER_OK;
}

} // namespace Bus
} // namespace Alljoyn
} // namespace Org;
