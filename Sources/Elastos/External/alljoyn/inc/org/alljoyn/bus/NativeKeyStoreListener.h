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

#ifndef __ORG_ALLJOYN_BUS_NATIVEKEYSTORELISTENER_H__
#define __ORG_ALLJOYN_BUS_NATIVEKEYSTORELISTENER_H__

#include "_Org.Alljoyn.Bus.h"
#include <alljoyn/KeyStoreListener.h>

namespace Org {
namespace Alljoyn {
namespace Bus {

class NativeKeyStoreListener : public ajn::KeyStoreListener
{
public:
    NativeKeyStoreListener(
        /* [in] */ IKeyStoreListener* listener);

    ~NativeKeyStoreListener();

    QStatus LoadRequest(
        /* [in] */ ajn::KeyStore& keyStore);

    QStatus StoreRequest(
        /* [in] */ ajn::KeyStore& keyStore);

private:
    NativeKeyStoreListener(const NativeKeyStoreListener& other);
    NativeKeyStoreListener& operator =(const NativeKeyStoreListener& other);

private:
    AutoPtr<IWeakReference> mKeyStoreListener;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org;

#endif // __ORG_ALLJOYN_BUS_NATIVEKEYSTORELISTENER_H__
