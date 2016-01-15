
#ifndef __ELASTOS_DROID_INTERNAL_OS_INSTALLERCONNECTION_H__
#define __ELASTOS_DROID_INTERNAL_OS_INSTALLERCONNECTION_H__

#include "elastos/droid/ext/frameworkext.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Net.h"
#include <elastos/core/Object.h>
#include <Elastos.CoreLibrary.IO.h>

using Elastos::Droid::Net::ILocalSocket;
using Elastos::Core::Object;
using Elastos::IO::IInputStream;
using Elastos::IO::IOutputStream;

namespace Elastos {
namespace Droid {
namespace Internal {
namespace Os {

/**
 * Represents a connection to {@code installd}. Allows multiple connect and
 * disconnect cycles.
 *
 * @hide for internal use only
 */
class InstallerConnection
    : public Object
    , public IInstallerConnection
{
public:
    CARAPI constructor();

    CAR_INTERFACE_DECL()

    CARAPI Transact(
        /* [in] */ const String& cmd,
        /* [out] */ String* result);

    CARAPI Execute(
        /* [in] */ const String& cmd,
        /* [out] */ Int32* result);

    CARAPI Dexopt(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& instructionSet,
        /* [out] */ Int32* result);

    CARAPI Dexopt(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& pkgName,
        /* [in] */ const String& instructionSet,
        /* [in] */ Boolean vmSafeMode,
        /* [out] */ Int32* result);

    CARAPI Patchoat(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& instructionSet,
        /* [out] */ Int32* result);

    CARAPI Patchoat(
        /* [in] */ const String& apkPath,
        /* [in] */ Int32 uid,
        /* [in] */ Boolean isPublic,
        /* [in] */ const String& pkgName,
        /* [in] */ const String& instructionSet,
        /* [out] */ Int32* result);

    CARAPI Disconnect();

private:
    CARAPI_(Boolean) Connect();

    CARAPI_(Boolean) ReadFully(
        /* [in] */ ArrayOf<Byte>* buffer,
        /* [in] */ Int32 len);

    CARAPI_(Int32) ReadReply();

    CARAPI_(Boolean) WriteCommand(
        /* [in] */ const String& cmdString);

private:
    static const String TAG;
    static const Boolean LOCAL_DEBUG;

    AutoPtr<IInputStream> mIn;
    AutoPtr<IOutputStream> mOut;
    AutoPtr<ILocalSocket> mSocket;

    AutoPtr<ArrayOf<Byte> > mBuf;
};

} // namespace Os
} // namespace Internal
} // namespace Droid
} // namespace Elastos

#endif //__ELASTOS_DROID_INTERNAL_OS_INSTALLERCONNECTION_H__
