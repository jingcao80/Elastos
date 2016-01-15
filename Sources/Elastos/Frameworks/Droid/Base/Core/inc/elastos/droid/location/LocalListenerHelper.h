
#ifndef __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__
#define __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__

#include "elastos/droid/ext/frameworkext.h"
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Location.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Elastos::Utility::IHashSet;

namespace Elastos {
namespace Droid {
namespace Location {

class LocalListenerHelper
    : public Object
    , public ILocalListenerHelper
{
public:
    CAR_INTERFACE_DECL()

    CARAPI Add(
        /* [in] */ IInterface* listener,
        /* [out] */ Boolean* result);

    CARAPI Remove(
        /* [in] */ IInterface* listener);

    virtual CARAPI RegisterWithServer(
        /* [out] */ Boolean* result) = 0;

    virtual CARAPI UnregisterFromServer() = 0;

    CARAPI Foreach(
        /* [in] */ ILocalListenerHelperListenerOperation* operation);

protected:
    LocalListenerHelper(
        /* [in] */ const String& name);

private:
    AutoPtr<IHashSet> mListeners;
    const String mTag;
};

}//namespace Location
}//namespace Droid
}//namespace Elastos

#endif // __ELASTOS_DROID_LOCATION_LOCALLISTENERHELPER_H__
