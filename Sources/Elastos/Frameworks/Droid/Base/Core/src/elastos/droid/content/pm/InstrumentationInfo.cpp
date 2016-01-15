
#include "elastos/droid/content/pm/InstrumentationInfo.h"

namespace Elastos {
namespace Droid {
namespace Content {
namespace Pm {

CAR_INTERFACE_IMPL_2(InstrumentationInfo, PackageItemInfo, IInstrumentationInfo, IParcelable)

InstrumentationInfo::InstrumentationInfo()
    : mHandleProfiling(FALSE)
    , mFunctionalTest(FALSE)
{}

InstrumentationInfo::~InstrumentationInfo()
{}

ECode InstrumentationInfo::constructor()
{
    return PackageItemInfo::constructor();
}

ECode InstrumentationInfo::constructor(
    /* [in] */ IInstrumentationInfo* orig)
{
    assert(orig != NULL);
    PackageItemInfo::constructor(IPackageItemInfo::Probe(orig));
    orig->GetTargetPackage(&mTargetPackage);
    orig->GetSourceDir(&mSourceDir);
    orig->GetPublicSourceDir(&mPublicSourceDir);
    orig->GetDataDir(&mDataDir);
    orig->GetNativeLibraryDir(&mNativeLibraryDir);
    orig->GetHandleProfiling(&mHandleProfiling);
    orig->GetFunctionalTest(&mFunctionalTest);
    return NOERROR;
}

ECode InstrumentationInfo::ToString(
    /* [out] */ String* str)
{
    VALIDATE_NOT_NULL(str);
    // return "InstrumentationInfo{"
    //         + Integer.toHexString(System.identityHashCode(this))
    //         + " " + packageName + "}";
    assert(0);
    return E_NOT_IMPLEMENTED;
}

ECode InstrumentationInfo::ReadFromParcel(
    /* [in] */ IParcel* source)
{
    assert(source != NULL);
    PackageItemInfo::ReadFromParcel(source);
    source->ReadString(&mTargetPackage);
    source->ReadString(&mSourceDir);
    source->ReadString(&mPublicSourceDir);
    source->ReadString(&mDataDir);
    source->ReadString(&mNativeLibraryDir);
    source->ReadBoolean(&mHandleProfiling);
    source->ReadBoolean(&mFunctionalTest);
    return NOERROR;
}

ECode InstrumentationInfo::WriteToParcel(
    /* [in] */ IParcel* dest)
{
    assert(dest != NULL);
    PackageItemInfo::WriteToParcel(dest);
    dest->WriteString(mTargetPackage);
    dest->WriteString(mSourceDir);
    dest->WriteString(mPublicSourceDir);
    dest->WriteString(mDataDir);
    dest->WriteString(mNativeLibraryDir);
    dest->WriteBoolean(mHandleProfiling);
    dest->WriteBoolean(mFunctionalTest);
    return NOERROR;
}

ECode InstrumentationInfo::GetTargetPackage(
    /* [out] */ String* pkg)
{
    VALIDATE_NOT_NULL(pkg);
    *pkg = mTargetPackage;
    return NOERROR;
}

ECode InstrumentationInfo::SetTargetPackage(
    /* [in] */ const String& pkg)
{
    mTargetPackage = pkg;
    return NOERROR;
}

ECode InstrumentationInfo::GetSourceDir(
    /* [out] */ String* sourceDir)
{
    VALIDATE_NOT_NULL(sourceDir);
    *sourceDir = mSourceDir;
    return NOERROR;
}

ECode InstrumentationInfo::SetSourceDir(
    /* [in] */ const String& sourceDir)
{
    mSourceDir = sourceDir;
    return NOERROR;
}

ECode InstrumentationInfo::GetPublicSourceDir(
    /* [out] */ String* pubSrcDir)
{
    VALIDATE_NOT_NULL(pubSrcDir);
    *pubSrcDir = mPublicSourceDir;
    return NOERROR;
}

ECode InstrumentationInfo::SetPublicSourceDir(
    /* [in] */ const String& pubSrcDir)
{
    mPublicSourceDir = pubSrcDir;
    return NOERROR;
}

ECode InstrumentationInfo::GetDataDir(
    /* [out] */ String* dataDir)
{
    VALIDATE_NOT_NULL(dataDir);
    *dataDir = mDataDir;
    return NOERROR;
}

ECode InstrumentationInfo::SetDataDir(
    /* [in] */ const String& dataDir)
{
    mDataDir = dataDir;
    return NOERROR;
}

ECode InstrumentationInfo::GetNativeLibraryDir(
    /* [out] */ String* libraryDir)
{
    VALIDATE_NOT_NULL(libraryDir);
    *libraryDir = mNativeLibraryDir;
    return NOERROR;
}

ECode InstrumentationInfo::SetNativeLibraryDir(
    /* [in] */ const String& libraryDir)
{
    mNativeLibraryDir = libraryDir;
    return NOERROR;
}

ECode InstrumentationInfo::GetHandleProfiling(
    /* [out] */ Boolean* handleProfiling)
{
    VALIDATE_NOT_NULL(handleProfiling);
    *handleProfiling = mHandleProfiling;
    return NOERROR;
}

ECode InstrumentationInfo::SetHandleProfiling(
    /* [in] */ Boolean handleProfiling)
{
    mHandleProfiling = handleProfiling;
    return NOERROR;
}

ECode InstrumentationInfo::GetFunctionalTest(
    /* [out] */ Boolean* functionalTest)
{
    VALIDATE_NOT_NULL(functionalTest);
    *functionalTest = mFunctionalTest;
    return NOERROR;
}

ECode InstrumentationInfo::SetFunctionalTest(
    /* [in] */ Boolean functionalTest)
{
    mFunctionalTest = functionalTest;
    return NOERROR;
}

ECode InstrumentationInfo::SetSplitSourceDirs(
    /* [in] */ ArrayOf<String>* dirs)
{
    mSplitSourceDirs = dirs;
    return NOERROR;
}

ECode InstrumentationInfo::GetSplitSourceDirs(
    /* [out, callee] */ ArrayOf<String>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = mSplitSourceDirs;
    REFCOUNT_ADD(*dirs)
    return NOERROR;
}

ECode InstrumentationInfo::SetSplitPublicSourceDirs(
    /* [in] */ ArrayOf<String>* dirs)
{
    mSplitPublicSourceDirs = dirs;
    return NOERROR;
}

ECode InstrumentationInfo::GetSplitPublicSourceDirs(
    /* [out, callee] */ ArrayOf<String>** dirs)
{
    VALIDATE_NOT_NULL(dirs)
    *dirs = mSplitPublicSourceDirs;
    REFCOUNT_ADD(*dirs)
    return NOERROR;
}

} // namespace Pm
} // namespace Content
} // namespace Droid
} // namespace Elastos
