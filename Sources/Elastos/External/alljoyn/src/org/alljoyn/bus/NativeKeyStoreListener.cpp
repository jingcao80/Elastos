//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

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
