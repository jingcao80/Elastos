
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
