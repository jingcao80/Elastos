
#ifndef  __ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__
#define  __ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__

#include "_Elastos.Droid.SystemUI.h"
#include "elastos/droid/systemui/Utils.h"
#include <elastos/core/Object.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IConfiguration;
using Elastos::Core::Object;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IPrintWriter;
// using Elastos::Utility::IMap;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace SystemUI {

class SystemUI
    : public Object
    , public ISystemUI
{
public:
    CAR_INTERFACE_DECL();

    SystemUI();

    virtual ~SystemUI();

    virtual CARAPI Start() = 0;

    virtual CARAPI OnConfigurationChanged(
        /* [in] */ IConfiguration* newConfig);

    virtual CARAPI Dump(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ IPrintWriter* pw,
        /* [in] */ ArrayOf<String>* args);

    virtual CARAPI OnBootCompleted();

    // @SuppressWarnings("unchecked")
    virtual CARAPI_(AutoPtr<IInterface>) GetComponent(
        /* [in] */ const String& interfaceType);

    virtual CARAPI PutComponent(
        /* [in] */ const String& interfaceType,
        /* [in] */ IInterface* component);

    virtual CARAPI SetContext(
        /* [in] */ IContext* context);

    virtual CARAPI GetContext(
        /* [out] */ IContext** context);

    virtual CARAPI SetComponents(
        /* [in] */ HashMap<String, AutoPtr<IInterface> >* components);

    virtual AutoPtr<HashMap<String, AutoPtr<IInterface> > > GetComponents();

public:
    AutoPtr<IContext> mContext;
    AutoPtr<HashMap<String, AutoPtr<IInterface> > > mComponents;
};

} // namespace SystemUI
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SYSTEMUI_SYSTEMUI_H__
