#ifndef __ELASTOS_DROID_PROVIDER_CCONTACTSSETTINGS_H__
#define __ELASTOS_DROID_PROVIDER_CCONTACTSSETTINGS_H__

#include "_Elastos_Droid_Provider_CContactsSettings.h"
#include <elastos/core/Singleton.h>

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Net::IUri;

namespace Elastos {
namespace Droid {
namespace Provider {

CarClass(CContactsSettings)
    , public Singleton
    , public IBaseColumns
    , public IContactsSettings
    , public IContactsSettingsColumns
{
public:
    CAR_SINGLETON_DECL()

    CAR_INTERFACE_DECL()

    /**
     * no public constructor since this is a utility class
     */
    CARAPI constructor();

    /**
     * The content:// style URL for this table
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetCONTENT_URI(
        /* [out] */ IUri** uri);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI GetSetting(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& account,
        /* [in] */ const String& key,
        /* [out] */ String* setting);

    /**
     * @deprecated see {@link android.provider.ContactsContract}
     */
    //@Deprecated
    CARAPI SetSetting(
        /* [in] */ IContentResolver* cr,
        /* [in] */ const String& account,
        /* [in] */ const String& key,
        /* [in] */ const String& value);
};

} // Provider
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_PROVIDER_CCONTACTSSETTINGS_H__
