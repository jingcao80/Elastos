
#ifndef __ELASTOS_DROID_CONTENT_PM_INSTRUMENTATIONINFO_H__
#define __ELASTOS_DROID_CONTENT_PM_INSTRUMENTATIONINFO_H__

#include "elastos/droid/content/pm/PackageItemInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

/**
 * Information you can retrieve about a particular piece of test
 * instrumentation.  This corresponds to information collected
 * from the AndroidManifest.xml's &lt;instrumentation&gt; tag.
 */
class InstrumentationInfo
    : public PackageItemInfo
    , public IInstrumentationInfo
    , public IParcelable
{
public:
    CAR_INTERFACE_DECL()

    InstrumentationInfo();

    virtual ~InstrumentationInfo();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ IInstrumentationInfo* orig);

    CARAPI ToString(
        /* [out] */ String* str);

    CARAPI ReadFromParcel(
        /* [in] */ IParcel* source);

    CARAPI WriteToParcel(
        /* [in] */ IParcel* dest);

    CARAPI GetTargetPackage(
        /* [out] */ String* pkg);

    CARAPI SetTargetPackage(
        /* [in] */ const String& pkg);

    CARAPI GetSourceDir(
        /* [out] */ String* sourceDir);

    CARAPI SetSourceDir(
        /* [in] */ const String& sourceDir);

    CARAPI GetPublicSourceDir(
        /* [out] */ String* pubSrcDir);

    CARAPI SetPublicSourceDir(
        /* [in] */ const String& pubSrcDir);

    CARAPI GetDataDir(
        /* [out] */ String* dataDir);

    CARAPI SetDataDir(
        /* [in] */ const String& dataDir);

    CARAPI GetNativeLibraryDir(
        /* [out] */ String* libraryDir);

    CARAPI SetNativeLibraryDir(
        /* [in] */ const String& libraryDir);

    CARAPI GetHandleProfiling(
        /* [out] */ Boolean* handleProfiling);

    CARAPI SetHandleProfiling(
        /* [in] */ Boolean handleProfiling);

    CARAPI GetFunctionalTest(
        /* [out] */ Boolean* functionalTest);

    CARAPI SetFunctionalTest(
        /* [in] */ Boolean functionalTest);

    CARAPI SetSplitSourceDirs(
        /* [in] */ ArrayOf<String>* dirs);

    CARAPI GetSplitSourceDirs(
        /* [out, callee] */ ArrayOf<String>** dirs);

    CARAPI SetSplitPublicSourceDirs(
        /* [in] */ ArrayOf<String>* dirs);

    CARAPI GetSplitPublicSourceDirs(
        /* [out, callee] */ ArrayOf<String>** dirs);

public:
    /**
     * The name of the application package being instrumented.  From the
     * "package" attribute.
     */
    String mTargetPackage;

    /**
     * Full path to the base APK for this application.
     */
    String mSourceDir;

    /**
     * Full path to the publicly available parts of {@link #sourceDir},
     * including resources and manifest. This may be different from
     * {@link #sourceDir} if an application is forward locked.
     */
    String mPublicSourceDir;

    /**
     * Full paths to zero or more split APKs that, when combined with the base
     * APK defined in {@link #sourceDir}, form a complete application.
     */
    AutoPtr<ArrayOf<String> > mSplitSourceDirs;

    /**
     * Full path to the publicly available parts of {@link #splitSourceDirs},
     * including resources and manifest. This may be different from
     * {@link #splitSourceDirs} if an application is forward locked.
     */
    AutoPtr<ArrayOf<String> > mSplitPublicSourceDirs;

    /**
     * Full path to a directory assigned to the package for its persistent
     * data.
     */
    String mDataDir;

    /**
     * Full path to the directory where the native JNI libraries are stored.
     *
     * {@hide}
     */
    String mNativeLibraryDir;

    /**
     * Specifies whether or not this instrumentation will handle profiling.
     */
    Boolean mHandleProfiling;

    /** Specifies whether or not to run this instrumentation as a functional test */
    Boolean mFunctionalTest;
};

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_PM_INSTRUMENTATIONINFO_H__
