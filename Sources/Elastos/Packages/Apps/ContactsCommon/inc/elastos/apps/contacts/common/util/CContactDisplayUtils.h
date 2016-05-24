#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CCONTACTDISPLAYUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CCONTACTDISPLAYUTILS_H__

#include "_Elastos_Apps_Contacts_Common_Util_CContactDisplayUtils.h"
#include <elastos/core/Singleton.h>

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

CarClass(CContactDisplayUtils)
    , public Singleton
    , public IContactDisplayUtils
{
public:
    CAR_INTERFACE_DECL()

    CAR_SINGLETON_DECL()

    /**
     * Checks if the given data type is a custom type.
     *
     * @param type Phone data type.
     * @return {@literal true} if the type is custom.  {@literal false} if not.
     */
    CARAPI IsCustomPhoneType(
        /* [in] */ IInteger32* type,
        /* [out] */ Boolean* result);

    /**
     * Gets a display label for a given phone type.
     *
     * @param type The type of number.
     * @param customLabel A custom label to use if the phone is determined to be of custom type
     * determined by {@link #isCustomPhoneType(Integer))}
     * @param interactionType whether this is a call or sms.  Either {@link #INTERACTION_CALL} or
     * {@link #INTERACTION_SMS}.
     * @param context The application context.
     * @return An appropriate string label
     */
    CARAPI GetLabelForCallOrSms(
        /* [in] */ IInteger32* type,
        /* [in] */ ICharSequence* customLabel,
        /* [in] */ Int32 interactionType,
        /* [in] */ IContext* context,
        /* [out] */ ICharSequence** result);

    /**
     * Find a label for calling.
     *
     * @param type The type of number.
     * @return An appropriate string label.
     */
    CARAPI GetPhoneLabelResourceId(
        /* [in] */ IInteger32* type,
        /* [out] */ Int32* id);

    /**
     * Find a label for sending an sms.
     *
     * @param type The type of number.
     * @return An appropriate string label.
     */
    CARAPI GetSmsLabelResourceId(
        /* [in] */ IInteger32* type,
        /* [out] */ Int32* id);
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CCONTACTDISPLAYUTILS_H__
