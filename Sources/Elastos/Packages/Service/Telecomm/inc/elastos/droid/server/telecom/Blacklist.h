
#ifndef __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__
#define __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__

#include "_Elastos.Droid.Server.Telecom.h"
#include <elastos/core/Object.h>
#include <elastos/droid/ext/frameworkext.h>

using Elastos::Droid::Content::IContext;
using Elastos::IO::IObjectInput;
using Elastos::IO::IObjectOutput;

namespace Elastos {
namespace Droid {
namespace Server {
namespace Telecom {

/**
 * This class used to handle the blacklist data. Its
 * only remaining purpose is legacy data migration
 */
class Blacklist
    : public Object
{
private:
    class PhoneNumber
        : public Object
        // , public IExternalizable
    {
    public:
        CAR_INTERFACE_DECL()

        CARAPI constructor();

        CARAPI WriteExternal(
            /* [in] */ IObjectOutput* out);

        CARAPI ReadExternal(
            /* [in] */ IObjectInput* in);

        // @Override
        CARAPI HashCode(
            /* [out] */ Int32* result);

        static const Int64 serialVersionUID;

        String mPhone;
    };

public:
    static CARAPI MigrateOldDataIfPresent(
        /* [in] */ IContext* context);

private:
    static const String BLFILE;

    static const Int32 BLFILE_VER;
};

} // namespace Telecom
} // namespace Server
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SERVER_TELECOM_BLACKLIST_H__
