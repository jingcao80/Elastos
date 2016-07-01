
#ifndef __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CONTACTDISPLAYUTILS_H__
#define __ELASTOS_APPS_CONTACTS_COMMON_UTIL_CONTACTDISPLAYUTILS_H__

namespace Elastos{
namespace Apps{
namespace Contacts {
namespace Common {
namespace Util {

/**
 * Methods for handling various contact data labels.
 */
class ContactDisplayUtils
    : public Object
{
public:
    /**
     * Checks if the given data type is a custom type.
     *
     * @param type Phone data type.
     * @return {@literal true} if the type is custom.  {@literal false} if not.
     */
    static CARAPI_(Boolean) IsCustomPhoneType(
        /* [in] */ IInteger32* type);

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
    static CARAPI_(AutoPtr<ICharSequence>) GetLabelForCallOrSms(
        /* [in] */ IInteger32* type,
        /* [in] */ ICharSequence* customLabel,
        /* [in] */ Int32 interactionType,
        /* [in] */ IContext* context);

    /**
     * Find a label for calling.
     *
     * @param type The type of number.
     * @return An appropriate string label.
     */
    static CARAPI_(Int32) GetPhoneLabelResourceId(
        /* [in] */ IInteger32* type);

    /**
     * Find a label for sending an sms.
     *
     * @param type The type of number.
     * @return An appropriate string label.
     */
    static CARAPI_(Int32) GetSmsLabelResourceId(
        /* [in] */ IInteger32* type);

private:
    static const String TAG; // = ContactDisplayUtils.class.getSimpleName();
};

} // Util
} // Common
} // Contacts
} // Apps
} // Elastos

#endif //__ELASTOS_APPS_CONTACTS_COMMON_UTIL_CONTACTDISPLAYUTILS_H__
