
#ifndef __ORG_ALLJOYN_BUS_ABOUTICONOBJ_H__
#define __ORG_ALLJOYN_BUS_ABOUTICONOBJ_H__

#include "_Org.Alljoyn.Bus.h"
#include <elastos/core/Object.h>

using Elastos::Core::Object;
using Org::Alljoyn::Bus::Ifaces::IIcon;

namespace Org {
namespace Alljoyn {
namespace Bus {

class AboutIconObj
    : public Object
    , public IAboutIconObj
    , public IIcon
    , public IBusObject
{
public:
    CAR_INTERFACE_DECL();

    /**
     * Create an About Icon BusObject
     *
     * @param bus BusAttachment instance associated with this BusObject
     * @param icon instance of an AboutIcon which holds the icon image
     */
    CARAPI constructor(
        /* [in] */ IBusAttachment* bus,
        /* [in] */ IAboutIcon* icon);

    CARAPI GetVersion(
        /* [out] */ Int16* ver);

    CARAPI GetMimeType(
        /* [out] */ String* mimeType);

    CARAPI GetSize(
        /* [out] */ Int32* size);

    CARAPI GetUrl(
        /* [out] */ String* url);

    CARAPI GetContent(
        /* [out, callee] */ ArrayOf<Byte>** content);

public:
    /**
     * version of the org.alljoyn.Icon interface
     */
    static const Int16 VERSION = 1;

private:
    /**
     * AboutIcon used by the AboutIcon BusObject
     */
    AutoPtr<IAboutIcon> mIcon;
};

} // namespace Bus
} // namespace Alljoyn
} // namespace Org

#endif // __ORG_ALLJOYN_BUS_ABOUTICONOBJ_H__
