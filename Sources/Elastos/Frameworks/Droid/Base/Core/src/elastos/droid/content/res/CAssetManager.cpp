
#define THROW_ON_BAD_ID 0

#include <Elastos.CoreLibrary.Utility.h>
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/content/res/CAssetManager.h"
#include "elastos/droid/content/res/XmlBlock.h"
#include "elastos/droid/content/res/CAssetFileDescriptor.h"
#include "elastos/droid/text/TextUtils.h"
#include "elastos/droid/utility/CTypedValue.h"
#include "elastos/droid/utility/CSparseArray.h"
#include "elastos/droid/os/CParcelFileDescriptor.h"
#include <elastos/core/Math.h>
#include <elastos/core/AutoLock.h>
#include <elastos/core/StringBuilder.h>
#include <elastos/core/CoreUtils.h>
#include <elastos/utility/logging/Logger.h>
#include <androidfw/AssetManager.h>
#include <androidfw/ResourceTypes.h>

#include <private/android_filesystem_config.h> // for AID_SYSTEM

#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <linux/capability.h>
extern "C" int capget(cap_user_header_t hdrp, cap_user_data_t datap);
extern "C" int capset(cap_user_header_t hdrp, const cap_user_data_t datap);


using Elastos::Droid::Text::TextUtils;
using Elastos::Droid::Utility::CTypedValue;
using Elastos::Droid::Utility::CSparseArray;
using Elastos::Droid::Os::ParcelFileDescriptor;
using Elastos::Droid::Os::CParcelFileDescriptor;
using Elastos::Core::CInteger32;
using Elastos::Core::IInteger32;
using Elastos::Core::CoreUtils;
using Elastos::Core::StringBuilder;
using Elastos::Core::AutoLock;
using Elastos::IO::ICloseable;
using Elastos::IO::EIID_ICloseable;
using Elastos::IO::EIID_IInputStream;
using Elastos::IO::CFileDescriptor;
using Elastos::IO::CFile;
using Elastos::IO::IFile;
using Elastos::Utility::CArrayList;
using Elastos::Utility::Logging::Logger;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

// This is called by zygote (running as user root) as part of preloadResources.
static void VerifySystemIdmaps()
{
    pid_t pid;
    char system_id[10];

    snprintf(system_id, sizeof(system_id), "%d", AID_SYSTEM);

    switch (pid = fork()) {
        case -1:
            ALOGE("failed to fork for idmap: %s", strerror(errno));
            break;
        case 0: // child
            {
                struct __user_cap_header_struct capheader;
                struct __user_cap_data_struct capdata;

                memset(&capheader, 0, sizeof(capheader));
                memset(&capdata, 0, sizeof(capdata));

                capheader.version = _LINUX_CAPABILITY_VERSION;
                capheader.pid = 0;

                if (capget(&capheader, &capdata) != 0) {
                    ALOGE("capget: %s\n", strerror(errno));
                    exit(1);
                }

                capdata.effective = capdata.permitted;
                if (capset(&capheader, &capdata) != 0) {
                    ALOGE("capset: %s\n", strerror(errno));
                    exit(1);
                }

                if (setgid(AID_SYSTEM) != 0) {
                    ALOGE("setgid: %s\n", strerror(errno));
                    exit(1);
                }

                if (setuid(AID_SYSTEM) != 0) {
                    ALOGE("setuid: %s\n", strerror(errno));
                    exit(1);
                }

                execl(android::AssetManager::IDMAP_BIN, android::AssetManager::IDMAP_BIN, "--scan",
                    android::AssetManager::OVERLAY_DIR, android::AssetManager::TARGET_PACKAGE_NAME,
                    android::AssetManager::TARGET_APK_PATH, android::AssetManager::IDMAP_DIR, (char*)NULL);
                ALOGE("failed to execl for idmap: %s", strerror(errno));
                exit(1); // should never get here
            }
            break;
        default: // parent
            waitpid(pid, NULL, 0);
            break;
    }
}


//==============================================================================
//                  CAssetManager::AssetInputStream
//==============================================================================

CAR_INTERFACE_IMPL(CAssetManager::AssetInputStream, InputStream, IAssetInputStream)

CAssetManager::AssetInputStream::AssetInputStream(
    /* [in] */ CAssetManager* assetManager,
    /* [in] */ Int64 asset)
    : mAssetManager(assetManager)
    , mAsset(asset)
{
    mLength = mAssetManager->GetAssetLength(asset);
    mMarkPos = 0;
}

CAssetManager::AssetInputStream::~AssetInputStream()
{
    Close();
}

ECode CAssetManager::AssetInputStream::GetAssetInt32(
    /* [out] */ Int32* value)
{
    return E_UNSUPPORTED_OPERATION_EXCEPTION;
}

ECode CAssetManager::AssetInputStream::GetNativeAsset(
    /* [out] */ Int64* value)
{
    VALIDATE_NOT_NULL(value)
    *value = mAsset;
    return NOERROR;
}

ECode CAssetManager::AssetInputStream::Read(
    /* [out] */ Int32* value)
{
    return mAssetManager->ReadAssetChar(mAsset, value);
}

ECode CAssetManager::AssetInputStream::IsMarkSupported(
    /* [out] */ Boolean* supported)
{
    VALIDATE_NOT_NULL(supported);

    *supported = TRUE;
    return NOERROR;
}

ECode CAssetManager::AssetInputStream::Available(
    /* [out] */ Int32* bytes)
{
    VALIDATE_NOT_NULL(bytes);

    Int64 len = mAssetManager->GetAssetRemainingLength(mAsset);
    *bytes = len > Elastos::Core::Math::INT32_MAX_VALUE ? Elastos::Core::Math::INT32_MAX_VALUE : (Int32)len;
    return NOERROR;
}

ECode CAssetManager::AssetInputStream::Close()
{
    AutoLock lock(mAssetManager);

    if (mAsset != 0) {
        mAssetManager->DestroyAsset(mAsset);
        mAsset = 0;
        mAssetManager->DecRefsLocked((Int32)this, "AssetInputStream::Close()");
    }
    return NOERROR;
}

ECode CAssetManager::AssetInputStream::Mark(
    /* [in] */ Int32 readLimit)
{
    return mMarkPos = mAssetManager->SeekAsset(mAsset, 0, 0);
}

ECode CAssetManager::AssetInputStream::Reset()
{
    return mAssetManager->SeekAsset(mAsset, mMarkPos, -1);
}

ECode CAssetManager::AssetInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [out] */ Int32* number)
{
    return mAssetManager->ReadAsset(mAsset, buffer,
        0, buffer->GetLength(), number);
}

ECode CAssetManager::AssetInputStream::ReadBytes(
    /* [out] */ ArrayOf<Byte>* buffer,
    /* [in] */ Int32 offset,
    /* [in] */ Int32 length,
    /* [out] */ Int32* number)
{
    return mAssetManager->ReadAsset(mAsset, buffer,
            offset, length, number);
}

ECode CAssetManager::AssetInputStream::Skip(
    /* [in] */ Int64 length,
    /* [out] */ Int64* number)
{
    VALIDATE_NOT_NULL(number);

    Int64 pos = mAssetManager->SeekAsset(mAsset, 0, 0);
    if ((pos + length) > mLength) {
        length = mLength - pos;
    }
    if (length > 0) {
        mAssetManager->SeekAsset(mAsset, length, 0);
    }
    *number = length;
    return NOERROR;
}


//==============================================================================
//                  CAssetManager
//==============================================================================

INIT_PROI_3 AutoPtr<CAssetManager> CAssetManager::sSystem;
const Int32 CAssetManager::STYLE_NUM_ENTRIES;
const Int32 CAssetManager::STYLE_TYPE;
const Int32 CAssetManager::STYLE_DATA;
const Int32 CAssetManager::STYLE_ASSET_COOKIE;
const Int32 CAssetManager::STYLE_RESOURCE_ID;
const Int32 CAssetManager::STYLE_CHANGING_CONFIGURATIONS;
const Int32 CAssetManager::STYLE_DENSITY;

const String CAssetManager::TAG("CAssetManager");
const Boolean CAssetManager::LocalLOGV = FALSE;
const Boolean CAssetManager::DEBUG_REFS = TRUE;
INIT_PROI_3 Object CAssetManager::sSync;

CAR_INTERFACE_IMPL_2(CAssetManager, Object, IAssetManager, ICloseable)

CAR_OBJECT_IMPL(CAssetManager)

CAssetManager::CAssetManager()
    : mObject(0)
    , mThemeSupport(FALSE)
    , mIconPackCookie(0)
    , mCommonResCookie(0)
    , mNumRefs(1)
    , mOpen(TRUE)
{
    CTypedValue::New((ITypedValue**)&mValue);
    mOffsets = ArrayOf<Int64>::Alloc(2);
    CArrayList::New(2, (IArrayList**)&mThemeCookies);
}

CAssetManager::~CAssetManager()
{
    Close();
    if (DEBUG_REFS && mNumRefs > 1 && this != sSystem) {
        Logger::V(TAG, "==== CAssetManager::~CAssetManager, mNumRefs: %d, this: %p ====",
            mNumRefs, this);

        HashMap<Int64, String>::Iterator it;
        for (it = mRefStacks.Begin(); it != mRefStacks.End(); ++it) {
            Logger::V(TAG, "==== CAssetManager reference through [%s]", it->mSecond.string());
        }
        assert(0);
    }

    Finalize();
}

ECode CAssetManager::constructor()
{
    if (DEBUG_REFS) {
        AutoLock lock(this);
        mNumRefs = 0;
        IncRefsLocked(GetHashCode(), "CAssetManager::constructor()");
    }

    Init(FALSE);
    if (LocalLOGV) Logger::V(TAG, "New asset manager: %p", this);
    return EnsureSystemAssets();
}

ECode CAssetManager::constructor(
    /* [in] */ Boolean isSystem)
{
    if (DEBUG_REFS) {
        AutoLock lock(this);

        mNumRefs = 0;
        IncRefsLocked(GetHashCode(), "CAssetManager::constructor()");
    }

    Init(TRUE);
    if (LocalLOGV) Logger::V(TAG, "New asset manager: %p", this);
    return NOERROR;
}

ECode CAssetManager::EnsureSystemAssets()
{
    AutoLock lock(sSync);

    if (sSystem == NULL) {
        AutoPtr<CAssetManager> system;
        FAIL_RETURN(CAssetManager::NewByFriend(TRUE, (CAssetManager**)&system));
        system->MakeStringBlocks(NULL);
        sSystem = system;

        if (LocalLOGV) Logger::V(TAG, "EnsureSystemAssets: %p", system->Probe(EIID_IAssetManager));
    }
    return NOERROR;
}

AutoPtr<CAssetManager> CAssetManager::GetSystem()
{
    ASSERT_SUCCEEDED(EnsureSystemAssets());
    return sSystem;
}

ECode CAssetManager::Close()
{
    AutoLock lock(this);

    if (mOpen) {
        mOpen = FALSE;
        DecRefsLocked(GetHashCode(), "CAssetManager::Close()");
    }
    return NOERROR;
}

AutoPtr<ICharSequence> CAssetManager::GetResourceText(
    /* [in] */Int32 ident)
{
    AutoLock lock(this);

    AutoPtr<ITypedValue> tmpValue = mValue;
    Int32 block;
    ECode ec = LoadResourceValue(ident, (Int16)0, tmpValue, TRUE, &block);
    if (SUCCEEDED(ec) && block >= 0) {
        Int32 type = 0;
        tmpValue->GetType(&type);
        if (type == ITypedValue::TYPE_STRING) {
            Int32 data = 0;
            mValue->GetData(&data);
            return (*mStringBlocks)[block]->Get(data);
        }
        AutoPtr<ICharSequence> text;
        tmpValue->CoerceToString((ICharSequence**)&text);
        return text;
    }
    return NULL;
}

AutoPtr<ICharSequence> CAssetManager::GetResourceBagText(
    /* [in] */Int32 ident,
    /* [in] */Int32 bagEntryId)
{
    AutoLock lock(this);

    AutoPtr<ITypedValue> tmpValue = mValue;
    Int32 block;
    ECode ec = LoadResourceBagValue(ident, bagEntryId, tmpValue, TRUE, &block);
    if (SUCCEEDED(ec) && block >= 0) {
        Int32 type = 0;
        tmpValue->GetType(&type);
        if (type == ITypedValue::TYPE_STRING) {
            Int32 data = 0;
            tmpValue->GetData(&data);
            return (*mStringBlocks)[block]->Get(data);
        }
        AutoPtr<ICharSequence> text;
        tmpValue->CoerceToString((ICharSequence**)&text);
        return text;
    }
    return NULL;
}

AutoPtr< ArrayOf<String> > CAssetManager::GetResourceStringArray(
    /* [in] */ Int32 id)
{
    AutoPtr< ArrayOf<String> > result;
    GetArrayStringResource(id, (ArrayOf<String>**)&result);
    return result;
}

Boolean CAssetManager::GetResourceValue(
    /* [in] */ Int32 ident,
    /* [in] */ Int32 density,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs)
{
    Int32 block;
    ECode ec = LoadResourceValue(ident, (Int16)density, outValue, resolveRefs, &block);
    if (SUCCEEDED(ec) && block >= 0) {
        Int32 type = 0;
        outValue->GetType(&type);
        if (type != ITypedValue::TYPE_STRING) {
            return TRUE;
        }
        Int32 data = 0;
        outValue->GetData(&data);
        AutoPtr<ICharSequence> str = (*mStringBlocks)[block]->Get(data);
        outValue->SetString(str);
        return TRUE;
    }
    return FALSE;
}

AutoPtr< ArrayOf<ICharSequence*> > CAssetManager::GetResourceTextArray(
    /* [in] */ Int32 id)
{
    AutoPtr< ArrayOf<Int32> > rawInfoArray;
    ECode ec = GetArrayStringInfo(id, (ArrayOf<Int32>**)&rawInfoArray);
    if (FAILED(ec) || rawInfoArray == NULL) return NULL;
    Int32 rawInfoArrayLen = rawInfoArray->GetLength();
    const Int32 infoArrayLen = rawInfoArrayLen / 2;
    Int32 block = 0;
    Int32 index = 0;
    AutoPtr< ArrayOf<ICharSequence*> > retArray = ArrayOf<ICharSequence*>::Alloc(infoArrayLen);
    for (Int32 i = 0, j = 0; i < rawInfoArrayLen; i = i + 2, j++) {
        block = (*rawInfoArray)[i];
        index = (*rawInfoArray)[i + 1];
        AutoPtr<ICharSequence> cs = index >= 0 ? (*mStringBlocks)[block]->Get(index) : NULL;
        retArray->Set(j, cs);
    }
    return retArray;
}

Boolean CAssetManager::GetThemeValue(
    /* [in] */ Int64 theme,
    /* [in] */ Int32 ident,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolveRefs)
{
    Int32 block;
    ECode ec = LoadThemeAttributeValue(theme, ident, outValue, resolveRefs, &block);
    if (SUCCEEDED(ec) && block >= 0) {
        Int32 type = 0;
        outValue->GetType(&type);
        if (type != ITypedValue::TYPE_STRING) {
            return TRUE;
        }
        AutoPtr< ArrayOf<StringBlock*> > blocks = mStringBlocks;
        if (blocks == NULL) {
            EnsureStringBlocks();
            blocks = mStringBlocks;
        }
        Int32 data = 0;
        outValue->GetData(&data);
        AutoPtr<ICharSequence> str = (*blocks)[block]->Get(data);
        outValue->SetString(str);
        return TRUE;
    }
    return FALSE;
}

void CAssetManager::EnsureStringBlocks()
{
    if (mStringBlocks == NULL) {
        AutoLock lock(this);

        if (mStringBlocks == NULL) {
            MakeStringBlocks(GetSystem()->mStringBlocks);
        }
    }
}

void CAssetManager::RecreateStringBlocks()
{
    {    AutoLock syncLock(this);
        MakeStringBlocks(sSystem->mStringBlocks);
    }
}

void CAssetManager::MakeStringBlocks(
    /* [in] */ ArrayOf<StringBlock*>* seed)
{
    const Int32 seedNum = seed ? seed->GetLength() : 0;
    Int32 num = GetStringBlockCount();
    AutoPtr<ArrayOf<StringBlock*> > temp = seed;// seed maybe mStringBlocks, hold ref here.
    mStringBlocks = ArrayOf<StringBlock*>::Alloc(num);
    if (LocalLOGV) {
        Logger::V(TAG, "Making string blocks for %p: size %d, seedNum:%d", this, num, seedNum);
    }

    for (Int32 i = 0; i < num; i++) {
        if (i < seedNum) {
            mStringBlocks->Set(i, (*temp)[i]);
        }
        else {
            AutoPtr<StringBlock> block = new StringBlock(GetNativeStringBlock(i), TRUE);
            mStringBlocks->Set(i, block);
        }
    }
}

AutoPtr<ICharSequence> CAssetManager::GetPooledStringForCookie(
    /* [in] */ Int32 cookie,
    /* [in] */ Int32 id)
{
    // Cookies map to string blocks starting at 1.
    return (*mStringBlocks)[cookie - 1]->Get(id);
}

ECode CAssetManager::Open(
    /* [in] */ const String& fileName,
    /* [out] */ IInputStream** stream)
{
    return Open(fileName, ACCESS_STREAMING, stream);
}

ECode CAssetManager::Open(
    /* [in] */ const String& fileName,
    /* [in] */ Int32 accessMode,
    /* [out] */ IInputStream** stream)
{
    VALIDATE_NOT_NULL(stream);

    AutoLock lock(this);

    if (!mOpen) {
        // Logger::E(TAG, "Assetmanager has been closed");
        *stream = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    Int64 asset;
    ECode ec = OpenAsset(fileName, accessMode, &asset);
    if (SUCCEEDED(ec) && asset != 0) {
        *stream = new AssetInputStream(this, asset);
        REFCOUNT_ADD(*stream);
        IncRefsLocked((Int32)*stream, "CAssetManager::OpenEx");
        return NOERROR;
    }
    if (LocalLOGV) Logger::E(TAG, "Asset file: %s", fileName.string());
    *stream = NULL;
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode CAssetManager::OpenFd(
    /* [in] */const String& fileName,
    /* [out] */ IAssetFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    AutoLock lock(this);

    if (!mOpen) {
        // Logger::E(TAG, "Assetmanager has been closed");
        *fd = NULL;
        return E_RUNTIME_EXCEPTION;
    }

    AutoPtr<IParcelFileDescriptor> pfd = OpenAssetFd(fileName, mOffsets);
    if (pfd != NULL) {
        return CAssetFileDescriptor::New(pfd, (*mOffsets)[0], (*mOffsets)[1], fd);
    }
    if (LocalLOGV) Logger::E(TAG, "Asset file: %s", fileName.string());
    *fd = NULL;
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode CAssetManager::List(
    /* [in] */ const String& path,
    /* [out, callee] */ ArrayOf<String>** names)
{
    VALIDATE_NOT_NULL(names);
    *names = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return E_ILLEGAL_STATE_EXCEPTION;
    }

    if (path.IsNull()) {
        return NOERROR;
    }

    android::AssetDir* dir = am->openDir(path.string());

    if (dir == NULL) {
        // Logger::E(TAG, path.string());
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    size_t N = dir->getFileCount();

    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(N);
    if (array == NULL) {
        delete dir;
        return E_OUT_OF_MEMORY;
    }

    for (size_t i = 0; i < N; i++) {
        const android::String8& name = dir->getFileName(i);
        (*array)[i] = (const char*)name;
    }

    delete dir;

    *names = array;
    REFCOUNT_ADD(*names);
    return NOERROR;
}

ECode CAssetManager::OpenNonAsset(
    /* [in] */ const String& fileName,
    /* [out] */ IInputStream** stream)
{
    return OpenNonAsset(0, fileName, ACCESS_STREAMING, stream);
}

ECode CAssetManager::OpenNonAsset(
    /* [in] */ const String& fileName,
    /* [in] */ Int32 accessMode,
    /* [out] */ IInputStream ** stream)
{
    return OpenNonAsset(0, fileName, accessMode, stream);
}

ECode CAssetManager::OpenNonAsset(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [out] */ IInputStream ** stream)
{
    return OpenNonAsset(cookie, fileName, ACCESS_STREAMING, stream);
}

ECode CAssetManager::OpenNonAsset(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [in] */ Int32 accessMode,
    /* [out] */ IInputStream ** stream)
{
    {
        AutoLock lock(this);

        if (!mOpen) {
//            throw new RuntimeException("Assetmanager has been closed");
            *stream = NULL;
            return E_RUNTIME_EXCEPTION;
        }

        Int64 asset;
        FAIL_RETURN(OpenNonAssetNative(cookie, fileName, accessMode, &asset));
        if (asset != 0) {
            *stream = new AssetInputStream(this, asset);
            REFCOUNT_ADD(*stream);
            IncRefsLocked((Int32)*stream, "CAssetManager::OpenNonAssetEx3");
            return NOERROR;
        }
    }
    if (LocalLOGV) Logger::E(TAG, "Asset absolute file: %s", fileName.string());
    *stream = NULL;
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode CAssetManager::OpenNonAssetFd(
    /* [in] */ const String& fileName,
    /* [out] */ IAssetFileDescriptor** fd)
{
    return OpenNonAssetFd(0, fileName, fd);
}

ECode CAssetManager::OpenNonAssetFd(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [out] */ IAssetFileDescriptor** fd)
{
    VALIDATE_NOT_NULL(fd);

    {
        AutoLock lock(this);

        if (!mOpen) {
            // Logger::E(TAG, "Assetmanager has been closed");
            *fd = NULL;
            return E_RUNTIME_EXCEPTION;
        }
        AutoPtr<IParcelFileDescriptor> pfd = OpenNonAssetFdNative(cookie, fileName, mOffsets);
        if (pfd != NULL) {
            return CAssetFileDescriptor::New(pfd, (*mOffsets)[0], (*mOffsets)[1], fd);
        }
    }
    if (LocalLOGV) Logger::E(TAG, "Asset absolute file: %s", fileName.string());
    *fd = NULL;
    return E_FILE_NOT_FOUND_EXCEPTION;
}

ECode CAssetManager::OpenXmlResourceParser(
    /* [in] */ const String& fileName,
    /* [out] */ IXmlResourceParser** parser)
{
    return OpenXmlResourceParser(0, fileName, parser);
}

ECode CAssetManager::OpenXmlResourceParser(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [out] */ IXmlResourceParser** parser)
{
    VALIDATE_NOT_NULL(parser);
    *parser = NULL;

    AutoPtr<XmlBlock> block;
    FAIL_RETURN(OpenXmlBlockAsset(cookie, fileName, (XmlBlock**)&block));

    AutoPtr<IXmlResourceParser> rp = block->NewParser();
    block->Close();
    *parser = rp;
    REFCOUNT_ADD(*parser);
    return NOERROR;
}

ECode CAssetManager::OpenXmlBlockAsset(
    /* [in] */ const String& fileName,
    /* [out] */ XmlBlock** xmlBlock)
{
    return OpenXmlBlockAsset(0, fileName, xmlBlock);
}

ECode CAssetManager::OpenXmlBlockAsset(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [out] */ XmlBlock** res)
{
    VALIDATE_NOT_NULL(res);
    *res = NULL;

    AutoLock lock(this);

    if (!mOpen) {
        // Logger::E(TAG, "Assetmanager has been closed");
        return E_RUNTIME_EXCEPTION;
    }

    Int64 xmlBlock;
    FAIL_RETURN(OpenXmlAssetNative(cookie, fileName, &xmlBlock));
    if (xmlBlock != 0) {
        *res = new XmlBlock(this, xmlBlock);
        REFCOUNT_ADD(*res);
        IncRefsLocked((Int32)*res, "CAssetManager::OpenXmlBlockAsset");
        return NOERROR;
    }
    if (LocalLOGV) Logger::E(TAG, String("Asset XML file: ") + fileName);
    return E_FILE_NOT_FOUND_EXCEPTION;
}

void CAssetManager::XmlBlockGone(
    /* [in] */ Int32 id)
{
    AutoLock lock(this);

    DecRefsLocked(id, "CAssetManager::XmlBlockGone");
}

ECode CAssetManager::CreateTheme(
    /* [out] */ Int64* res)
{
    VALIDATE_NOT_NULL(res);
    *res = 0;

    {
        AutoLock lock(this);

        if (!mOpen) {
            // Logger::E(TAG, "Assetmanager has been closed");
            return E_RUNTIME_EXCEPTION;
        }

        *res = NewTheme();
        IncRefsLocked(*res, "CAssetManager::CreateTheme");
        return NOERROR;
    }
}

ECode CAssetManager::ReleaseTheme(
    /* [in] */ Int64 theme)
{
    AutoLock lock(this);

    DeleteTheme(theme);
    DecRefsLocked(theme, "CAssetManager::ReleaseTheme");
    return NOERROR;
}

void CAssetManager::Finalize()
{
    // try {
    // if (DEBUG_REFS && mNumRefs != 0) {
    //     Log.w(TAG, "AssetManager " + this
    //             + " finalized with non-zero refs: " + mNumRefs);
    //     if (mRefStacks != null) {
    //         for (RuntimeException e : mRefStacks.values()) {
    //             Log.w(TAG, "Reference from here", e);
    //         }
    //     }
    // }
    Destroy();
    // } finally {
    //     super.finalize();
    // }
}

ECode CAssetManager::AddAssetPath(
    /* [in] */ const String& path,
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie);
    *cookie = 0;

    AutoLock lock(this);

    if (path.IsNull()) {
        return NOERROR;
    }

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    int32_t c;
    bool res = am->addAssetPath(android::String8(path.string()), &c);

    if (mStringBlocks.Get() != NULL) {
        MakeStringBlocks(mStringBlocks);
    }
    *cookie = (res) ? c : 0;
    return NOERROR;
}

ECode CAssetManager::AddAssetPaths(
    /* [in] */ ArrayOf<String>* paths,
    /* [out, callee] */ ArrayOf<Int32>** cookies)
{
    VALIDATE_NOT_NULL(cookies);

    if (paths == NULL) {
        *cookies = NULL;
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > arr = ArrayOf<Int32>::Alloc(paths->GetLength());
    for (Int32 i = 0; i < paths->GetLength(); i++) {
        Int32 cookie;
        AddAssetPath((*paths)[i], &cookie);
        (*arr)[i] = cookie;
    }

    *cookies = arr;
    REFCOUNT_ADD(*cookies);
    return NOERROR;
}

ECode CAssetManager::SetThemeSupport(
    /* [in] */ Boolean themeSupport)
{
    mThemeSupport = themeSupport;
    return NOERROR;
}

ECode CAssetManager::HasThemeSupport(
    /* [out] */ Boolean* hasThemeSupport)
{
    VALIDATE_NOT_NULL(hasThemeSupport)
    *hasThemeSupport = mThemeSupport;
    return NOERROR;
}

ECode CAssetManager::GetIconPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mIconPackageName;
    return NOERROR;
}

ECode CAssetManager::SetIconPackageName(
    /* [in] */ const String& packageName)
{
    mIconPackageName = packageName;
    return NOERROR;
}

ECode CAssetManager::GetCommonResPackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mCommonResPackageName;
    return NOERROR;
}

ECode CAssetManager::SetCommonResPackageName(
    /* [in] */ const String& packageName)
{
    mCommonResPackageName = packageName;
    return NOERROR;
}

ECode CAssetManager::GetThemePackageName(
    /* [out] */ String* packageName)
{
    VALIDATE_NOT_NULL(packageName)
    *packageName = mThemePackageName;
    return NOERROR;
}

ECode CAssetManager::SetThemePackageName(
    /* [in] */ const String& packageName)
{
    mThemePackageName = packageName;
    return NOERROR;
}

ECode CAssetManager::GetThemeCookies(
    /* [out] */ IArrayList** cookies)
{
    VALIDATE_NOT_NULL(cookies)
    *cookies = mThemeCookies;
    REFCOUNT_ADD(*cookies)
    return NOERROR;
}

ECode CAssetManager::SetIconPackCookie(
    /* [in] */ Int32 cookie)
{
    mIconPackCookie = cookie;
    return NOERROR;
}

ECode CAssetManager::GetIconPackCookie(
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie)
    *cookie = mIconPackCookie;
    return NOERROR;
}

ECode CAssetManager::SetCommonResCookie(
    /* [in] */ Int32 cookie)
{
    mCommonResCookie = cookie;
    return NOERROR;
}

ECode CAssetManager::GetCommonResCookie(
    /* [out] */ Int32* cookie)
{
    VALIDATE_NOT_NULL(cookie)
    *cookie = mCommonResCookie;
    return NOERROR;
}

ECode CAssetManager::AddThemeCookie(
    /* [in] */ Int32 cookie)
{
    AutoPtr<IInteger32> i;
    CInteger32::New(cookie, (IInteger32**)&i);
    mThemeCookies->Add(i);
    return NOERROR;
}

ECode CAssetManager::GetAppName(
    /* [out] */ String* appName)
{
    VALIDATE_NOT_NULL(appName)
    *appName = mAppName;
    return NOERROR;
}

ECode CAssetManager::SetAppName(
    /* [in] */ const String& pkgName)
{
    mAppName = pkgName;
    return NOERROR;
}

ECode CAssetManager::HasThemedAssets(
    /* [out] */ Boolean* hasThemeAssets)
{
    VALIDATE_NOT_NULL(hasThemeAssets)
    Int32 size;
    mThemeCookies->GetSize(&size);
    *hasThemeAssets = size > 0;
    return NOERROR;
}

ECode CAssetManager::AddOverlayPath(
    /* [in] */ const String& idmapPath,
    /* [in] */ const String& themeApkPath,
    /* [in] */ const String& resApkPath,
    /* [in] */ const String& targetPkgPath,
    /* [in] */ const String& prefixPath,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    {    AutoLock syncLock(this);
        if(idmapPath.IsNull()) {
            return NOERROR;
        }

        if(themeApkPath.IsNull()) {
            return NOERROR;
        }

        if(resApkPath.IsNull()) {
            return NOERROR;
        }

        if(targetPkgPath.IsNull()) {
            return NOERROR;
        }

        if(prefixPath.IsNull()) {
            return NOERROR;
        }

        android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
        if (am == NULL) {
            return NOERROR;
        }

        android::String8 idmapPath8(idmapPath.string());
        android::String8 themeApkPath8(themeApkPath.string());
        android::String8 resApkPath8(resApkPath.string());
        android::String8 targetPkgPath8(targetPkgPath.string());
        android::String8 prefixPath8(prefixPath.string());
        int32_t cookie;
        bool res = am->addOverlayPath(
            idmapPath8, themeApkPath8, &cookie, resApkPath8, targetPkgPath8, prefixPath8);
        if (res) {
            *result = cookie;
        }
    }
    return NOERROR;
}

ECode CAssetManager::AddCommonOverlayPath(
    /* [in] */ const String& themeApkPath,
    /* [in] */ const String& resApkPath,
    /* [in] */ const String& prefixPath,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    {    AutoLock syncLock(this);
        AutoPtr<IFile> file;
        CFile::New(themeApkPath, (IFile**)&file);
        Boolean exists;
        if (file->Exists(&exists), exists) {
            AutoPtr<IFile> resFile;
            CFile::New(resApkPath, (IFile**)&resFile);
            Boolean resExists;
            if (resFile->Exists(&resExists), resExists) {
                if (themeApkPath.IsNull()) {
                    return NOERROR;
                }

                if (resApkPath.IsNull()) {
                    return NOERROR;
                }

                if (prefixPath.IsNull()) {
                    return NOERROR;
                }

                android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
                if (am == NULL) {
                    return NOERROR;
                }

                android::String8 themeApkPath8(themeApkPath.string());
                android::String8 resApkPath8(resApkPath.string());
                android::String8 prefixPath8(prefixPath.string());
                int32_t cookie;
                bool res = am->addCommonOverlayPath(themeApkPath8, &cookie,
                        resApkPath8, prefixPath8);

                *result = (res) ? cookie : 0;
            }
        }
    }
    return NOERROR;
}

ECode CAssetManager::AddIconPath(
    /* [in] */ const String& idmapPath,
    /* [in] */ const String& resApkPath,
    /* [in] */ const String& prefixPath,
    /* [in] */ Int32 pkgIdOverride,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    {    AutoLock syncLock(this);
        if(idmapPath.IsNull()) {
            return NOERROR;
        }

        if(resApkPath.IsNull()) {
            return NOERROR;
        }

        if(prefixPath.IsNull()) {
            return NOERROR;
        }

        android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
        if (am == NULL) {
            return NOERROR;
        }

        android::String8 idmapPath8(idmapPath.string());
        android::String8 resApkPath8(resApkPath.string());
        android::String8 prefixPath8(prefixPath.string());
        int32_t cookie;
        bool res = am->addIconPath(idmapPath8, &cookie, resApkPath8,
                prefixPath8, pkgIdOverride);

        *result = (res) ? cookie : 0;
    }
    return NOERROR;
}

ECode CAssetManager::RemoveOverlayPath(
    /* [in] */ const String& packageName,
    /* [in] */ Int32 cookie,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    {    AutoLock syncLock(this);
        if (packageName.IsNull()) {
            return E_NULL_POINTER_EXCEPTION;
        }

        android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
        if (am == NULL) {
            return NOERROR;
        }

        android::String8 packageName8(packageName.string());
        bool res = am->removeOverlayPath(packageName8, cookie);

        *result = res;
    }
    return NOERROR;
}

ECode CAssetManager::IsUpToDate(
    /* [out] */ Boolean* isUpdated)
{
    VALIDATE_NOT_NULL(isUpdated);

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *isUpdated = TRUE;
        return NOERROR;
    }
    *isUpdated = am->isUpToDate() ? TRUE : FALSE;
    return NOERROR;
}

ECode CAssetManager::SetLocale(
    /* [in] */ const String& locale)
{
    if (locale.IsNull()) {
        return NOERROR;
    }

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    am->setLocale(locale.string());
    return NOERROR;
}

ECode CAssetManager::GetLocales(
    /* [out, callee] */ ArrayOf<String>** locales)
{
    VALIDATE_NOT_NULL(locales);

    android::Vector<android::String8> locs;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *locales = NULL;
        return NOERROR;
    }

    am->getLocales(&locs);
    const int N = locs.size();

    AutoPtr< ArrayOf<String> > result = ArrayOf<String>::Alloc(N);

    for (Int32 i = 0; i < N; i++) {
        (*result)[i] = locs[i].string();
    }

    *locales = result;
    REFCOUNT_ADD(*locales);
    return NOERROR;
}

ECode CAssetManager::SetConfiguration(
    /* [in] */Int32 mcc,
    /* [in] */Int32 mnc,
    /* [in] */const String& locale,
    /* [in] */Int32 orientation,
    /* [in] */Int32 touchscreen,
    /* [in] */Int32 density,
    /* [in] */Int32 keyboard,
    /* [in] */Int32 keyboardHidden,
    /* [in] */Int32 navigation,
    /* [in] */Int32 screenWidth,
    /* [in] */Int32 screenHeight,
    /* [in] */Int32 smallestScreenWidthDp,
    /* [in] */Int32 screenWidthDp,
    /* [in] */Int32 screenHeightDp,
    /* [in] */Int32 screenLayout,
    /* [in] */Int32 uiMode,
    /* [in] */Int32 majorVersion)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    android::ResTable_config config;
    memset(&config, 0, sizeof(config));

    const char* locale8 = !locale.IsNull() ? locale.string() : NULL;

    config.mcc = (uint16_t)mcc;
    config.mnc = (uint16_t)mnc;
    config.orientation = (uint8_t)orientation;
    config.touchscreen = (uint8_t)touchscreen;
    config.density = (uint16_t)density;
    config.keyboard = (uint8_t)keyboard;
    config.inputFlags = (uint8_t)keyboardHidden;
    config.navigation = (uint8_t)navigation;
    config.screenWidth = (uint16_t)screenWidth;
    config.screenHeight = (uint16_t)screenHeight;
    config.smallestScreenWidthDp = (uint16_t)smallestScreenWidthDp;
    config.screenWidthDp = (uint16_t)screenWidthDp;
    config.screenHeightDp = (uint16_t)screenHeightDp;
    config.screenLayout = (uint8_t)screenLayout;
    config.uiMode = (uint8_t)uiMode;
    config.sdkVersion = (uint16_t)majorVersion;
    config.minorVersion = 0;

    am->setConfiguration(config, locale8);
    return NOERROR;
}

Int32 CAssetManager::GetResourceIdentifier(
    /* [in] */ const String& name,
    /* [in] */ const String& defType,
    /* [in] */ const String& defPackage)
{
    if (name.IsNull()) {
        return 0;
    }

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return 0;
    }

    const android::String16 n16(name.string());
    const char16_t* name16 = (const char16_t*)n16;
    size_t nameLen = n16.size();

    const char16_t* defType16;
    size_t defTypeLen;
    android::String16 t16;
    if (!defType.IsNull()) {
        t16 = android::String16(defType.string());
        defType16 = (const char16_t*)t16;
        defTypeLen = t16.size();
    }
    else {
        defType16 = (const char16_t*)NULL;
        defTypeLen = 0;
    }

    const char16_t* defPackage16;
    size_t defPackageLen;
    android::String16 p16;
    if (!defPackage.IsNull()) {
        p16 = android::String16(defPackage.string());
        defPackage16 = (const char16_t*)p16;
        defPackageLen = p16.size();
    }
    else {
        defPackage16 = (const char16_t*)NULL;
        defPackageLen = 0;
    }

    return (Int32)am->getResources().identifierForName(
        name16, nameLen, defType16, defTypeLen, defPackage16, defPackageLen);
}

String CAssetManager::GetResourceName(
    /* [in] */ Int32 resid)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return String(NULL);
    }

    android::ResTable::resource_name name;
    if (!am->getResources().getResourceName(resid, true, &name)) {
        return String(NULL);
    }

    android::String16 str16;
    if (name.package != NULL) {
        str16.setTo(name.package, name.packageLen);
    }

    if (name.type8 != NULL || name.type != NULL) {
        if (str16.size() > 0) {
            char16_t div = ':';
            str16.append(&div, 1);
        }
        if (name.type8 != NULL) {
            str16.append(android::String16(name.type8, name.typeLen));
        } else {
            str16.append(name.type, name.typeLen);
        }
    }
    if (name.name8 != NULL || name.name != NULL) {
        if (str16.size() > 0) {
            char16_t div = '/';
            str16.append(&div, 1);
        }
        if (name.name8 != NULL) {
            str16.append(android::String16(name.name8, name.nameLen));
        } else {
            str16.append(name.name, name.nameLen);
        }
    }

    android::String8 str8(str16);
    return String(str8.string());
}

String CAssetManager::GetResourcePackageName(
    /* [in] */ Int32 resid)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return String(NULL);
    }

    android::ResTable::resource_name name;
    if (!am->getResources().getResourceName(resid, true, &name)) {
        return String(NULL);
    }

    if (name.package != NULL) {
        android::String8 str8(name.package);
        return String(str8.string());
    }

    return String(NULL);
}

String CAssetManager::GetResourceTypeName(
    /* [in] */ Int32 resid)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return String(NULL);
    }

    android::ResTable::resource_name name;
    if (!am->getResources().getResourceName(resid, true, &name)) {
        return String(NULL);
    }

    if (name.type8 != NULL) {
        return String(name.type8);
    }

    if (name.type != NULL) {
        android::String8 str8(name.type);
        return String(str8.string());
    }

    return String(NULL);
}

String CAssetManager::GetResourceEntryName(
    /* [in] */ Int32 resid)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return String(NULL);
    }

    android::ResTable::resource_name name;
    if (!am->getResources().getResourceName(resid, true, &name)) {
        return String(NULL);
    }

    if (name.name8 != NULL) {
        return String(name.type8);
    }

    if (name.name != NULL) {
        android::String8 str8(name.name);
        return String(str8.string());
    }

    return String(NULL);
}

ECode CAssetManager::OpenAsset(
    /* [in] */ const String& fileName,
    /* [in] */ Int32 accessMode,
    /* [out] */ Int64* asset)
{
    VALIDATE_NOT_NULL(asset);
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *asset = 0;
        return NOERROR;
    }

    if (LocalLOGV) Logger::V(TAG, "openAsset in %p", am);

    if (fileName.IsNull()) {
        *asset = -1;
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    if (accessMode != android::Asset::ACCESS_UNKNOWN && accessMode != android::Asset::ACCESS_RANDOM
        && accessMode != android::Asset::ACCESS_STREAMING && accessMode != android::Asset::ACCESS_BUFFER) {
        *asset = -1;
        return E_IO_EXCEPTION;
    }

    android::Asset* a = am->open(fileName.string(), (android::Asset::AccessMode)accessMode);

    if (a == NULL) {
        *asset = -1;
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    *asset = (Int32)a;
    return NOERROR;
}

static AutoPtr<IParcelFileDescriptor> ReturnParcelFileDescriptor(
    /* [in] */ android::Asset* a,
    /* [in, out] */ ArrayOf<Int64>* outOffsets)
{
    off64_t startOffset, length;
    int fd = a->openFileDescriptor(&startOffset, &length);
    delete a;

    if (fd < 0) {
        // Logger::E(TAG, "This file can not be opened as a file descriptor; it is probably compressed");
        return NULL;
    }

    if (outOffsets == NULL) {
        close(fd);
        return NULL;
    }

    (*outOffsets)[0] = startOffset;
    (*outOffsets)[1] = length;

    AutoPtr<IFileDescriptor> fileDesc;
    CFileDescriptor::New((IFileDescriptor**)&fileDesc);
    if (fileDesc == NULL) {
        close(fd);
        return NULL;
    }
    fileDesc->SetDescriptor(fd);

    AutoPtr<IParcelFileDescriptor> pfd;
    CParcelFileDescriptor::New((IParcelFileDescriptor**)&pfd);
    ((ParcelFileDescriptor*)pfd.Get())->constructor(fileDesc);
    return pfd;
}

AutoPtr<IParcelFileDescriptor> CAssetManager::OpenAssetFd(
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Int64>* outOffsets)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NULL;
    }

    if (LocalLOGV) Logger::V(TAG, "openAssetFd in %p", am);

    if (fileName.IsNull()) {
        return NULL;
    }

    android::Asset* a = am->open(fileName.string(), android::Asset::ACCESS_RANDOM);

    if (a == NULL) {
        // jniThrowException(env, "java/io/FileNotFoundException", fileName8.c_str());
        return NULL;
    }

    //printf("Created Asset Stream: %p\n", a);

    return ReturnParcelFileDescriptor(a, outOffsets);
}

ECode CAssetManager::OpenNonAssetNative(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [in] */ Int32 accessMode,
    /* [out] */ Int64* value)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *value = 0;
        return NOERROR;
    }

    if (LocalLOGV) Logger::V(TAG, "openNonAssetNative in %p", am);

    if (fileName.IsNull()) {
        *value = -1;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    if (accessMode != android::Asset::ACCESS_UNKNOWN && accessMode != android::Asset::ACCESS_RANDOM
        && accessMode != android::Asset::ACCESS_STREAMING && accessMode != android::Asset::ACCESS_BUFFER) {
        // jniThrowException(env, "java/lang/IllegalArgumentException", "Bad access mode");
        *value = -1;
        return E_ILLEGAL_ARGUMENT_EXCEPTION;
    }

    android::Asset* a = cookie
        ? am->openNonAsset(static_cast<int32_t>(cookie), fileName.string(),
                (android::Asset::AccessMode)accessMode)
        : am->openNonAsset(fileName.string(), (android::Asset::AccessMode)accessMode);

    if (a == NULL) {
        // jniThrowException(env, "java/io/FileNotFoundException", fileName8.c_str());
        *value = -1;
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    //printf("Created Asset Stream: %p\n", a);

    *value = (Int32)a;
    return NOERROR;
}

AutoPtr<IParcelFileDescriptor> CAssetManager::OpenNonAssetFdNative(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [in] */ ArrayOf<Int64>* outOffsets)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NULL;
    }

    if (LocalLOGV) Logger::V(TAG, "OpenNonAssetFdNative in %p", am);

    if (fileName.IsNull()) {
        return NULL;
    }

    android::Asset* a = cookie
        ? am->openNonAsset(static_cast<int32_t>(cookie), fileName.string(), android::Asset::ACCESS_RANDOM)
        : am->openNonAsset(fileName.string(), android::Asset::ACCESS_RANDOM);

    if (a == NULL) {
        // jniThrowException(env, "java/io/FileNotFoundException", fileName8.c_str());
        return NULL;
    }

    //printf("Created Asset Stream: %p\n", a);

    return ReturnParcelFileDescriptor(a, outOffsets);
}

void CAssetManager::DestroyAsset(
    /* [in] */ Int64 asset)
{
    android::Asset* a = (android::Asset*)asset;

    //printf("Destroying Asset Stream: %p\n", a);

    if (a == NULL) {
        // jniThrowNullPointerException(env, "asset");
        return;
    }

    delete a;
}

ECode CAssetManager::ReadAssetChar(
    /* [in] */ Int64 asset,
    /* [out] */ Int32* value)
{
    android::Asset* a = (android::Asset*)asset;

    if (a == NULL) {
        *value = -1;
        return E_NULL_POINTER_EXCEPTION;
    }

    uint8_t b;
    ssize_t res = a->read(&b, 1);
    *value = res == 1 ? (Int32)b : -1;
    return NOERROR;
}

ECode CAssetManager::ReadAsset(
    /* [in] */ Int64 asset,
    /* [in] */ ArrayOf<Byte>* b,
    /* [in] */ Int32 off,
    /* [in] */ Int32 len,
    /* [out] */ Int32* number)
{
    android::Asset* a = (android::Asset*)asset;

    if (a == NULL || b == NULL) {
        *number = -1;
        return E_NULL_POINTER_EXCEPTION;
    }

    if (len == 0) {
        *number = 0;
        return NOERROR;
    }

    Int32 bLen = b->GetLength();
    if (off < 0 || off >= bLen || len < 0 || len > bLen || (off + len) > bLen) {
        *number = -1;
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Byte* bt = b->GetPayload();
    ssize_t res = a->read(bt + off, len);

    if (res > 0) {
        *number = res;
        return NOERROR;
    }

    *number = -1;
    return res < 0 ? E_IO_EXCEPTION : NOERROR;
}

Int64 CAssetManager::SeekAsset(
    /* [in] */ Int64 asset,
    /* [in] */ Int64 offset,
    /* [in] */ Int32 whence)
{
    android::Asset* a = (android::Asset*)asset;

    if (a == NULL) {
        // jniThrowNullPointerException(env, "asset");
        return -1;
    }

    return a->seek(
        offset, (whence > 0) ? SEEK_END : (whence < 0 ? SEEK_SET : SEEK_CUR));
}

Int64 CAssetManager::GetAssetLength(
    /* [in] */ Int64 asset)
{
    android::Asset* a = (android::Asset*)asset;

    if (a == NULL) {
        // jniThrowNullPointerException(env, "asset");
        return -1;
    }

    return a->getLength();
}

Int64 CAssetManager::GetAssetRemainingLength(
    /* [in] */ Int64 asset)
{
    android::Asset* a = (android::Asset*)asset;

    if (a == NULL) {
        // jniThrowNullPointerException(env, "asset");
        return -1;
    }

    return a->getRemainingLength();
}

ECode CAssetManager::LoadResourceValue(
    /* [in] */ Int32 ident,
    /* [in] */ Int16 density,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolve,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());

    android::Res_value value;
    android::ResTable_config config;
    uint32_t typeSpecFlags;
    ssize_t block = res.getResource((uint32_t)ident, &value, false, density, &typeSpecFlags, &config);
#if THROW_ON_BAD_ID
    if (block == android::BAD_INDEX) {
        Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
        return E_ILLEGAL_STATE_EXCEPTION;
    }
#endif
    uint32_t ref = ident;
    if (resolve) {
        block = res.resolveReference(&value, block, &ref, &typeSpecFlags, &config);
#if THROW_ON_BAD_ID
        if (block == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif
    }
    if (block >= 0) {
        outValue->SetType((Int32)value.dataType);
        outValue->SetAssetCookie((Int32)res.getTableCookie(block));
        outValue->SetData((Int32)value.data);
        outValue->SetString(NULL);
        outValue->SetResourceId((Int32)ref);
        outValue->SetChangingConfigurations((Int32)typeSpecFlags);
        outValue->SetDensity((Int32)config.density);
    }
    *result = block;
    return NOERROR;
}

ECode CAssetManager::LoadResourceBagValue(
    /* [in] */ Int32 ident,
    /* [in] */ Int32 bagEntryId,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolve,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());

    // Now lock down the resource object and start pulling stuff from it.
    res.lock();

    ssize_t block = -1;
    android::Res_value value;

    const android::ResTable::bag_entry* entry = NULL;
    uint32_t typeSpecFlags;
    ssize_t entryCount = res.getBagLocked((uint32_t)ident, &entry, &typeSpecFlags);

    for (ssize_t i = 0; i < entryCount; i++) {
        if (((uint32_t)bagEntryId) == entry->map.name.ident) {
            block = entry->stringBlock;
            value = entry->map.value;
        }
        entry++;
    }

    res.unlock();

    if (block < 0) {
        *result = block;
        return NOERROR;
    }

    uint32_t ref = (uint32_t)ident;
    if (resolve) {
        block = res.resolveReference(&value, block, &ref, &typeSpecFlags);
#if THROW_ON_BAD_ID
        if (block == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif
    }
    if (block >= 0) {
        outValue->SetType((Int32)value.dataType);
        outValue->SetAssetCookie((Int32)res.getTableCookie(block));
        outValue->SetData((Int32)value.data);
        outValue->SetString(NULL);
        outValue->SetResourceId((Int32)ref);
        outValue->SetChangingConfigurations((Int32)typeSpecFlags);
    }
    *result = block;
    return NOERROR;
}

ECode CAssetManager::ApplyStyle(
    /* [in] */ Int64 themeToken,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [in] */ Int64 xmlParserToken,
    /* [in] */ const ArrayOf<Int32>& inAttrs,
    /* [in] */ ArrayOf<Int32>* outValues,
    /* [in] */ ArrayOf<Int32>* outIndices,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (themeToken == 0) {
        // jniThrowNullPointerException(env, "theme token");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (outValues == NULL) {
        // jniThrowNullPointerException(env, "out values");
        return E_NULL_POINTER_EXCEPTION;
    }

//      DEBUG_STYLES(LOGI("APPLY STYLE: theme=0x%x defStyleAttr=0x%x defStyleRes=0x%x xml=0x%x",
//          themeToken, defStyleAttr, defStyleRes, xmlParserToken));

    android::ResTable::Theme* theme = (android::ResTable::Theme*)themeToken;
    const android::ResTable& res = theme->getResTable();
    android::ResXMLParser* xmlParser = (android::ResXMLParser*)xmlParserToken;
    android::ResTable_config config;
    android::Res_value value;

    const Int32 NI = inAttrs.GetLength();
    const Int32 NV = outValues->GetLength();
    if (NV < (NI * STYLE_NUM_ENTRIES)) {
//          jniThrowException(env, "java/lang/IndexOutOfBoundsException", "out values too small");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32* src = inAttrs.GetPayload();
    if (src == NULL) {
//          jniThrowException(env, "java/lang/OutOfMemoryError", "");
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32* baseDest = outValues->GetPayload();
    Int32* dest = baseDest;
    if (dest == NULL) {
//          jniThrowException(env, "java/lang/OutOfMemoryError", "");
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32* indices = NULL;
    Int32 indicesIdx = 0;
    if (outIndices != NULL) {
        if (outIndices->GetLength() > NI) {
            indices = outIndices->GetPayload();
        }
    }

    // Load default style from attribute, if specified...
    uint32_t defStyleBagTypeSetFlags = 0;
    if (defStyleAttr != 0) {
        android::Res_value value;
        if (theme->getAttribute(defStyleAttr, &value, &defStyleBagTypeSetFlags) >= 0) {
            if (value.dataType == android::Res_value::TYPE_REFERENCE) {
                defStyleRes = value.data;
            }
        }
    }

    // Retrieve the style class associated with the current XML tag.
    Int32 style = 0;
    uint32_t styleBagTypeSetFlags = 0;
    if (xmlParser != NULL) {
        ssize_t idx = xmlParser->indexOfStyle();
        if (idx >= 0 && xmlParser->getAttributeValue(idx, &value) >= 0) {
            if (value.dataType == value.TYPE_ATTRIBUTE) {
                if (theme->getAttribute(value.data, &value, &styleBagTypeSetFlags) < 0) {
                    value.dataType = android::Res_value::TYPE_NULL;
                }
            }
            if (value.dataType == value.TYPE_REFERENCE) {
                style = value.data;
            }
        }
    }

    // Now lock down the resource object and start pulling stuff from it.
    res.lock();

    // Retrieve the default style bag, if requested.
    const android::ResTable::bag_entry* defStyleEnt = NULL;
    uint32_t defStyleTypeSetFlags = 0;
    ssize_t bagOff = defStyleRes != 0
            ? res.getBagLocked(defStyleRes, &defStyleEnt, &defStyleTypeSetFlags, TRUE) : -1;
    defStyleTypeSetFlags |= defStyleBagTypeSetFlags;
    const android::ResTable::bag_entry* endDefStyleEnt = defStyleEnt +
        (bagOff >= 0 ? bagOff : 0);

    // Retrieve the style class bag, if requested.
    const android::ResTable::bag_entry* styleEnt = NULL;
    uint32_t styleTypeSetFlags = 0;
    bagOff = style != 0 ? res.getBagLocked(style, &styleEnt, &styleTypeSetFlags, TRUE) : -1;
    styleTypeSetFlags |= styleBagTypeSetFlags;
    const android::ResTable::bag_entry* endStyleEnt = styleEnt +
        (bagOff >= 0 ? bagOff : 0);

    // Retrieve the XML attributes, if requested.
    const Int32 NX = xmlParser ? xmlParser->getAttributeCount() : 0;
    Int32 ix = 0;
    uint32_t curXmlAttr = xmlParser ? xmlParser->getAttributeNameResID(ix) : 0;

    static const ssize_t kXmlBlock = 0x10000000;

    // Now iterate through all of the attributes that the client has requested,
    // filling in each with whatever data we can find.
    ssize_t block = 0;
    uint32_t typeSetFlags;
    for (Int32 ii = 0; ii < NI; ii++) {
        const uint32_t curIdent = (uint32_t)src[ii];

        // DEBUG_STYLES(Logger::I(TAG, "RETRIEVING ATTR 0x%08x...", (Int32)curIdent));

        // Try to find a value for this attribute...  we prioritize values
        // coming from, first XML attributes, then XML style, then default
        // style, and finally the theme.
        value.dataType = android::Res_value::TYPE_NULL;
        value.data = 0;
        typeSetFlags = 0;
        config.density = 0;

        // Skip through XML attributes until the end or the next possible match.
        while (ix < NX && curIdent > curXmlAttr) {
            ix++;
            curXmlAttr = xmlParser->getAttributeNameResID(ix);
        }
        // Retrieve the current XML attribute if it matches, and step to next.
        if (ix < NX && curIdent == curXmlAttr) {
            block = kXmlBlock;
            xmlParser->getAttributeValue(ix, &value);
            ix++;
            curXmlAttr = xmlParser->getAttributeNameResID(ix);
//              DEBUG_STYLES(LOGI("-> From XML: type=0x%x, data=0x%08x",
//                      value.dataType, value.data));
        }

        // Skip through the style values until the end or the next possible match.
        while (styleEnt < endStyleEnt && curIdent > styleEnt->map.name.ident) {
            styleEnt++;
        }
        // Retrieve the current style attribute if it matches, and step to next.
        if (styleEnt < endStyleEnt && curIdent == styleEnt->map.name.ident) {
            if (value.dataType == android::Res_value::TYPE_NULL) {
                block = styleEnt->stringBlock;
                typeSetFlags = styleTypeSetFlags;
                value = styleEnt->map.value;
//                  DEBUG_STYLES(LOGI("-> From style: type=0x%x, data=0x%08x",
//                          value.dataType, value.data));
            }
            styleEnt++;
        }

        // Skip through the default style values until the end or the next possible match.
        while (defStyleEnt < endDefStyleEnt && curIdent > defStyleEnt->map.name.ident) {
            defStyleEnt++;
        }
        // Retrieve the current default style attribute if it matches, and step to next.
        if (defStyleEnt < endDefStyleEnt && curIdent == defStyleEnt->map.name.ident) {
            if (value.dataType == android::Res_value::TYPE_NULL) {
                block = defStyleEnt->stringBlock;
                typeSetFlags = defStyleTypeSetFlags;
                value = defStyleEnt->map.value;
//                  DEBUG_STYLES(LOGI("-> From def style: type=0x%x, data=0x%08x",
//                          value.dataType, value.data));
            }
            defStyleEnt++;
        }

        uint32_t resid = 0;
        if (value.dataType != android::Res_value::TYPE_NULL) {
            // Take care of resolving the found resource to its final value.
            ssize_t newBlock = theme->resolveAttributeReference(&value, block,
                    &resid, &typeSetFlags, &config);
            if (newBlock >= 0) block = newBlock;
//              DEBUG_STYLES(LOGI("-> Resolved attr: type=0x%x, data=0x%08x",
//                      value.dataType, value.data));
        }
        else {
            // If we still don't have a value for this attribute, try to find
            // it in the theme!
            ssize_t newBlock = theme->getAttribute(curIdent, &value, &typeSetFlags);
            if (newBlock >= 0) {
//                  DEBUG_STYLES(LOGI("-> From theme: type=0x%x, data=0x%08x",
//                          value.dataType, value.data));
                newBlock = res.resolveReference(&value, block, &resid,
                        &typeSetFlags, &config);
#if THROW_ON_BAD_ID
                if (newBlock == android::BAD_INDEX) {
                    Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
#endif
                if (newBlock >= 0) block = newBlock;
//                  DEBUG_STYLES(LOGI("-> Resolved theme: type=0x%x, data=0x%08x",
//                          value.dataType, value.data));
            }
        }

        // Deal with the special @null value -- it turns back to TYPE_NULL.
        if (value.dataType == android::Res_value::TYPE_REFERENCE && value.data == 0) {
//              DEBUG_STYLES(LOGI("-> Setting to @null!"));
            value.dataType = android::Res_value::TYPE_NULL;
            block = kXmlBlock;
        }

//          DEBUG_STYLES(LOGI("Attribute 0x%08x: type=0x%x, data=0x%08x",
//                  curIdent, value.dataType, value.data));

        // Write the final value back to Java.
        dest[STYLE_TYPE] = (Int32)value.dataType;
        dest[STYLE_DATA] = (Int32)value.data;
        dest[STYLE_ASSET_COOKIE] =
            block != kXmlBlock ? reinterpret_cast<Int32>(res.getTableCookie(block)) : -1;
        dest[STYLE_RESOURCE_ID] = (Int32)resid;
        dest[STYLE_CHANGING_CONFIGURATIONS] = (Int32)typeSetFlags;
        dest[STYLE_DENSITY] = (Int32)config.density;

        if (indices != NULL && value.dataType != android::Res_value::TYPE_NULL) {
            indicesIdx++;
            indices[indicesIdx] = (Int32)ii;
        }

        dest += STYLE_NUM_ENTRIES;
    }

    res.unlock();

    if (indices != NULL) {
        indices[0] = indicesIdx;
    }

    *result = TRUE;
    return NOERROR;
}

ECode CAssetManager::ResolveAttrs(
    /* [in] */ Int64 themeToken,
    /* [in] */ Int32 defStyleAttr,
    /* [in] */ Int32 defStyleRes,
    /* [in] */ const ArrayOf<Int32>& inValues,
    /* [in] */ const ArrayOf<Int32>& attrs,
    /* [in] */ ArrayOf<Int32>* outValues,
    /* [in] */ ArrayOf<Int32>* outIndices,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (themeToken == 0) {
        // jniThrowNullPointerException(env, "theme token");
        return E_NULL_POINTER_EXCEPTION;
    }
    // if (attrs == NULL) {
    //     // jniThrowNullPointerException(env, "attrs");
    //     return E_NULL_POINTER_EXCEPTION;
    // }
    if (outValues == NULL) {
        // jniThrowNullPointerException(env, "out values");
        return E_NULL_POINTER_EXCEPTION;
    }

//     DEBUG_STYLES(ALOGI("APPLY STYLE: theme=0x%x defStyleAttr=0x%x defStyleRes=0x%x",
//         themeToken, defStyleAttr, defStyleRes));

    android::ResTable::Theme* theme = reinterpret_cast<android::ResTable::Theme*>(themeToken);
    const android::ResTable& res = theme->getResTable();
    android::ResTable_config config;
    android::Res_value value;

    const Int32 NI = attrs.GetLength();
    const Int32 NV = outValues->GetLength();
    if (NV < (NI*STYLE_NUM_ENTRIES)) {
        // jniThrowException(env, "java/lang/IndexOutOfBoundsException", "out values too small");
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32* src = attrs.GetPayload();
    if (src == NULL) {
        return NOERROR;
    }

    Int32* srcValues = inValues.GetPayload();
    const Int32 NSV = srcValues == NULL ? 0 : inValues.GetLength();

    Int32* baseDest = outValues->GetPayload();
    Int32* dest = baseDest;
    if (dest == NULL) {
        return NOERROR;
    }

    Int32* indices = NULL;
    Int32 indicesIdx = 0;
    if (outIndices != NULL) {
        if (outIndices->GetLength() > NI) {
            indices = outIndices->GetPayload();
        }
    }

    // Load default style from attribute, if specified...
    uint32_t defStyleBagTypeSetFlags = 0;
    if (defStyleAttr != 0) {
        android::Res_value value;
        if (theme->getAttribute(defStyleAttr, &value, &defStyleBagTypeSetFlags) >= 0) {
            if (value.dataType == android::Res_value::TYPE_REFERENCE) {
                defStyleRes = value.data;
            }
        }
    }

    // Now lock down the resource object and start pulling stuff from it.
    res.lock();

    // Retrieve the default style bag, if requested.
    const android::ResTable::bag_entry* defStyleEnt = NULL;
    uint32_t defStyleTypeSetFlags = 0;
    ssize_t bagOff = defStyleRes != 0
            ? res.getBagLocked(defStyleRes, &defStyleEnt, &defStyleTypeSetFlags, TRUE) : -1;
    defStyleTypeSetFlags |= defStyleBagTypeSetFlags;
    const android::ResTable::bag_entry* endDefStyleEnt = defStyleEnt +
        (bagOff >= 0 ? bagOff : 0);;

    // Now iterate through all of the attributes that the client has requested,
    // filling in each with whatever data we can find.
    ssize_t block = 0;
    uint32_t typeSetFlags;
    for (Int32 ii=0; ii<NI; ii++) {
        const uint32_t curIdent = (uint32_t)src[ii];

//         DEBUG_STYLES(ALOGI("RETRIEVING ATTR 0x%08x...", curIdent));

        // Try to find a value for this attribute...  we prioritize values
        // coming from, first XML attributes, then XML style, then default
        // style, and finally the theme.
        value.dataType = android::Res_value::TYPE_NULL;
        value.data = 0;
        typeSetFlags = 0;
        config.density = 0;

        // Retrieve the current input value if available.
        if (NSV > 0 && srcValues[ii] != 0) {
            block = -1;
            value.dataType = android::Res_value::TYPE_ATTRIBUTE;
            value.data = srcValues[ii];
            // DEBUG_STYLES(ALOGI("-> From values: type=0x%x, data=0x%08x",
            //         value.dataType, value.data));
        }

        // Skip through the default style values until the end or the next possible match.
        while (defStyleEnt < endDefStyleEnt && curIdent > defStyleEnt->map.name.ident) {
            defStyleEnt++;
        }
        // Retrieve the current default style attribute if it matches, and step to next.
        if (defStyleEnt < endDefStyleEnt && curIdent == defStyleEnt->map.name.ident) {
            if (value.dataType == android::Res_value::TYPE_NULL) {
                block = defStyleEnt->stringBlock;
                typeSetFlags = defStyleTypeSetFlags;
                value = defStyleEnt->map.value;
                // DEBUG_STYLES(ALOGI("-> From def style: type=0x%x, data=0x%08x",
                //         value.dataType, value.data));
            }
            defStyleEnt++;
        }

        uint32_t resid = 0;
        if (value.dataType != android::Res_value::TYPE_NULL) {
            // Take care of resolving the found resource to its final value.
            ssize_t newBlock = theme->resolveAttributeReference(&value, block,
                    &resid, &typeSetFlags, &config);
            if (newBlock >= 0) block = newBlock;
            // DEBUG_STYLES(ALOGI("-> Resolved attr: type=0x%x, data=0x%08x",
            //         value.dataType, value.data));
        } else {
            // If we still don't have a value for this attribute, try to find
            // it in the theme!
            ssize_t newBlock = theme->getAttribute(curIdent, &value, &typeSetFlags);
            if (newBlock >= 0) {
                // DEBUG_STYLES(ALOGI("-> From theme: type=0x%x, data=0x%08x",
                //         value.dataType, value.data));
                newBlock = res.resolveReference(&value, block, &resid,
                        &typeSetFlags, &config);
#if THROW_ON_BAD_ID
                if (newBlock == android::BAD_INDEX) {
                    Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
                    return E_ILLEGAL_STATE_EXCEPTION;
                }
#endif
                if (newBlock >= 0) block = newBlock;
                // DEBUG_STYLES(ALOGI("-> Resolved theme: type=0x%x, data=0x%08x",
                //         value.dataType, value.data));
            }
        }

        // Deal with the special @null value -- it turns back to TYPE_NULL.
        if (value.dataType == android::Res_value::TYPE_REFERENCE && value.data == 0) {
            // DEBUG_STYLES(ALOGI("-> Setting to @null!"));
            value.dataType = android::Res_value::TYPE_NULL;
            block = -1;
        }

//         DEBUG_STYLES(ALOGI("Attribute 0x%08x: type=0x%x, data=0x%08x",
//                 curIdent, value.dataType, value.data));

        // Write the final value back to Java.
        dest[STYLE_TYPE] = value.dataType;
        dest[STYLE_DATA] = value.data;
        dest[STYLE_ASSET_COOKIE] =
            block != -1 ? reinterpret_cast<Int32>(res.getTableCookie(block)) : (Int32)-1;
        dest[STYLE_RESOURCE_ID] = resid;
        dest[STYLE_CHANGING_CONFIGURATIONS] = typeSetFlags;
        dest[STYLE_DENSITY] = config.density;

        if (indices != NULL && value.dataType != android::Res_value::TYPE_NULL) {
            indicesIdx++;
            indices[indicesIdx] = ii;
        }

        dest += STYLE_NUM_ENTRIES;
    }

    res.unlock();

    if (indices != NULL) {
        indices[0] = indicesIdx;
    }

    return NOERROR;
}

ECode CAssetManager::RetrieveAttributes(
    /* [in] */ Int64 xmlParserToken,
    /* [in] */ const ArrayOf<Int32>& inAttrs,
    /* [in] */ ArrayOf<Int32>* outValues,
    /* [in] */ ArrayOf<Int32>* outIndices,
    /* [out] */ Boolean* result)
{
    VALIDATE_NOT_NULL(result)
    *result = FALSE;

    if (xmlParserToken == 0) {
        Logger::E(TAG, "xmlParserToken is null!");
        return E_NULL_POINTER_EXCEPTION;
    }
    if (outValues == NULL) {
        Logger::E(TAG, "out values is null!");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());
    android::ResXMLParser* xmlParser = (android::ResXMLParser*)xmlParserToken;
    android::ResTable_config config;
    android::Res_value value;

    const Int32 NI = inAttrs.GetLength();
    const Int32 NV = outValues->GetLength();
    if (NV < (NI * STYLE_NUM_ENTRIES)) {
        Logger::E(TAG, "out values %d too small! It should be bigger than %d.", NV, NI * STYLE_NUM_ENTRIES);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }

    Int32* src = inAttrs.GetPayload();
    if (src == NULL) {
        Logger::E(TAG, "OutOfMemoryError");
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32* baseDest = outValues->GetPayload();
    Int32* dest = baseDest;
    if (dest == NULL) {
        Logger::E(TAG, "OutOfMemoryError");
        return E_OUT_OF_MEMORY_ERROR;
    }

    Int32* indices = NULL;
    Int32 indicesIdx = 0;
    if (outIndices != NULL) {
        if (outIndices->GetLength() > NI) {
            indices = outIndices->GetPayload();
        }
    }

    // Now lock down the resource object and start pulling stuff from it.
    res.lock();

    // Retrieve the XML attributes, if requested.
    const Int32 NX = xmlParser->getAttributeCount();
    Int32 ix = 0;
    uint32_t curXmlAttr = xmlParser->getAttributeNameResID(ix);

    static const ssize_t kXmlBlock = 0x10000000;

    // Now iterate through all of the attributes that the client has requested,
    // filling in each with whatever data we can find.
    ssize_t block = 0;
    uint32_t typeSetFlags;
    for (Int32 ii = 0; ii < NI; ii++) {
        const uint32_t curIdent = (uint32_t)src[ii];

        // Try to find a value for this attribute...
        value.dataType = android::Res_value::TYPE_NULL;
        value.data = 0;
        typeSetFlags = 0;
        config.density = 0;

        // Skip through XML attributes until the end or the next possible match.
        while (ix < NX && curIdent > curXmlAttr) {
            ix++;
            curXmlAttr = xmlParser->getAttributeNameResID(ix);
        }
        // Retrieve the current XML attribute if it matches, and step to next.
        if (ix < NX && curIdent == curXmlAttr) {
            block = kXmlBlock;
            xmlParser->getAttributeValue(ix, &value);
            ix++;
            curXmlAttr = xmlParser->getAttributeNameResID(ix);
        }

        //printf("Attribute 0x%08x: type=0x%x, data=0x%08x\n", curIdent, value.dataType, value.data);
        uint32_t resid = 0;
        if (value.dataType != android::Res_value::TYPE_NULL) {
            // Take care of resolving the found resource to its final value.
            //printf("Resolving attribute reference\n");
            ssize_t newBlock = res.resolveReference(&value, block, &resid,
                    &typeSetFlags, &config);
#if THROW_ON_BAD_ID
            if (newBlock == android::BAD_INDEX) {
                Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
#endif
            if (newBlock >= 0) block = newBlock;
        }

        // Deal with the special @null value -- it turns back to TYPE_NULL.
        if (value.dataType == android::Res_value::TYPE_REFERENCE && value.data == 0) {
            value.dataType = android::Res_value::TYPE_NULL;
        }

        //printf("Attribute 0x%08x: final type=0x%x, data=0x%08x\n", curIdent, value.dataType, value.data);

        // Write the final value back to Java.
        dest[STYLE_TYPE] = (Int32)value.dataType;
        dest[STYLE_DATA] = (Int32)value.data;
        dest[STYLE_ASSET_COOKIE] =
            block != kXmlBlock ? reinterpret_cast<Int32>(res.getTableCookie(block)) : -1;
        dest[STYLE_RESOURCE_ID] = (Int32)resid;
        dest[STYLE_CHANGING_CONFIGURATIONS] = (Int32)typeSetFlags;
        dest[STYLE_DENSITY] = (Int32)config.density;

        if (indices != NULL && value.dataType != android::Res_value::TYPE_NULL) {
            indicesIdx++;
            indices[indicesIdx] = (Int32)ii;
        }

        dest += STYLE_NUM_ENTRIES;
    }

    res.unlock();

    if (indices != NULL) {
        indices[0] = indicesIdx;
    }

    *result = TRUE;
    return NOERROR;
}

Int32 CAssetManager::GetArraySize(
    /* [in] */ Int32 resource)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return 0;
    }
    const android::ResTable& res(am->getResources());

    res.lock();
    const android::ResTable::bag_entry* defStyleEnt = NULL;
    ssize_t bagOff = res.getBagLocked(resource, &defStyleEnt);
    res.unlock();

    return bagOff;
}

ECode CAssetManager::RetrieveArray(
    /* [in] */ Int32 resource,
    /* [in] */ ArrayOf<Int32>* outValues,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    if (outValues == NULL) {
        Logger::E(TAG, "E_NULL_POINTER_EXCEPTION out values");
        // jniThrowNullPointerException(env, "out values");
        return E_NULL_POINTER_EXCEPTION;
    }

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());
    android::ResTable_config config;
    android::Res_value value;
    ssize_t block;

    const Int32 NV = outValues->GetLength();

    Int32* baseDest = outValues->GetPayload();
    Int32* dest = baseDest;
    if (dest == NULL) {
        // jniThrowException(env, "java/lang/OutOfMemoryError", "");
        Logger::E(TAG, "E_OUT_OF_MEMORY_ERROR.");
        return E_OUT_OF_MEMORY_ERROR;
    }

    // Now lock down the resource object and start pulling stuff from it.
    res.lock();

    const android::ResTable::bag_entry* arrayEnt = NULL;
    uint32_t arrayTypeSetFlags = 0;
    ssize_t bagOff = res.getBagLocked(resource, &arrayEnt, &arrayTypeSetFlags, TRUE);
    const android::ResTable::bag_entry* endArrayEnt = arrayEnt +
        (bagOff >= 0 ? bagOff : 0);

    Int32 i = 0;
    uint32_t typeSetFlags;
    while (i < NV && arrayEnt < endArrayEnt) {
        block = arrayEnt->stringBlock;
        typeSetFlags = arrayTypeSetFlags;
        config.density = 0;
        value = arrayEnt->map.value;

        uint32_t resid = 0;
        if (value.dataType != android::Res_value::TYPE_NULL) {
            // Take care of resolving the found resource to its final value.
            //printf("Resolving attribute reference\n");
            ssize_t newBlock = res.resolveReference(&value, block, &resid,
                    &typeSetFlags, &config);
#if THROW_ON_BAD_ID
            if (newBlock == android::BAD_INDEX) {
                Logger::E(TAG, "Bad resource at %d, type %d! line: %d", i, value.dataType, __LINE__);
                return E_ILLEGAL_STATE_EXCEPTION;
            }
#endif
            if (newBlock >= 0) block = newBlock;
        }

        // Deal with the special @null value -- it turns back to TYPE_NULL.
        if (value.dataType == android::Res_value::TYPE_REFERENCE && value.data == 0) {
            value.dataType = android::Res_value::TYPE_NULL;
        }

        //Logger::I(TAG, "Attribute 0x%08x: final type=0x%x, data=0x%08x\n", curIdent, value.dataType, value.data);

        // Write the final value back to Java.
        dest[STYLE_TYPE] = (Int32)value.dataType;
        dest[STYLE_DATA] = (Int32)value.data;
        dest[STYLE_ASSET_COOKIE] = reinterpret_cast<Int32>(res.getTableCookie(block));
        dest[STYLE_RESOURCE_ID] = (Int32)resid;
        dest[STYLE_CHANGING_CONFIGURATIONS] = (Int32)typeSetFlags;
        dest[STYLE_DENSITY] = (Int32)config.density;
        dest += STYLE_NUM_ENTRIES;
        i+= STYLE_NUM_ENTRIES;
        arrayEnt++;
    }

    i /= STYLE_NUM_ENTRIES;

    res.unlock();

    *result = i;
    return NOERROR;
}

Int32 CAssetManager::GetStringBlockCount()
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return 0;
    }
    return am->getResources().getTableCount();
}

Int64 CAssetManager::GetNativeStringBlock(
    /* [in] */ Int32 block)
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return 0;
    }
    return (Int32)am->getResources().getTableStringBlock(block);
}

ECode CAssetManager::GetCookieName(
    /* [in] */ Int32 cookie,
    /* [out] */ String* name)
{
    VALIDATE_NOT_NULL(name);

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *name = String(NULL);
        return NOERROR;
    }
    android::String8 n(am->getAssetPath(static_cast<int32_t>(cookie)));
    if (n.length() == 0) {
        // jniThrowException(env, "java/lang/IndexOutOfBoundsException", "Empty cookie name");
        *name = String(NULL);
        return E_INDEX_OUT_OF_BOUNDS_EXCEPTION;
    }
    *name = n.string();
    return NOERROR;
}

ECode CAssetManager::GetAssignedPackageIdentifiers(
    /* [out] */ ISparseArray** sparseArray)
{
    VALIDATE_NOT_NULL(sparseArray)
    *sparseArray = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    const android::ResTable& res = am->getResources();

    AutoPtr<ISparseArray> sa;
    CSparseArray::New((ISparseArray**)&sa);
    const size_t N = res.getBasePackageCount();
    for (size_t i = 0; i < N; i++) {
        Int32 id = res.getBasePackageId(i);
        android::String8 str8(res.getBasePackageName(i));
        String str(str8.string());
        AutoPtr<ICharSequence> csq = CoreUtils::Convert(str);
        sa->Put(id, csq);
    }

    *sparseArray = sa;
    REFCOUNT_ADD(*sparseArray)
    return NOERROR;
}

Int32 CAssetManager::GetGlobalAssetCount()
{
    return android::Asset::getGlobalCount();
}

String CAssetManager::GetAssetAllocations()
{
    android::String8 alloc = android::Asset::getAssetAllocations();
    if (alloc.length() <= 0) {
        return String(NULL);
    }

    String str(alloc.string());
    return str;
}

Int32 CAssetManager::GetGlobalAssetManagerCount()
{
    return android::AssetManager::getGlobalCount();
}

Int64 CAssetManager::NewTheme()
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return 0;
    }
    return (Int64)new android::ResTable::Theme(am->getResources());
}

void CAssetManager::DeleteTheme(
    /* [in] */ Int64 theme)
{
    delete (android::ResTable::Theme*)theme;
}

ECode CAssetManager::OpenXmlAssetNative(
    /* [in] */ Int32 cookie,
    /* [in] */ const String& fileName,
    /* [out] */ Int64* result)
{
    VALIDATE_NOT_NULL(result);
    *result = 0;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    if (LocalLOGV) {
        Logger::V(TAG, " >> openXmlAsset %s in %s, native object %p",
            fileName.string(), TO_CSTR(this), am);
    }

    if (fileName.IsNull()) {
//        jniThrowException(env, "java/lang/NullPointerException", "fileName");
        return E_NULL_POINTER_EXCEPTION;
    }

    const char* fileName8 = fileName.string();
    android::Asset* a = cookie
        ? am->openNonAsset(static_cast<int32_t>(cookie), fileName8, android::Asset::ACCESS_BUFFER)
        : am->openNonAsset(fileName8, android::Asset::ACCESS_BUFFER);

    if (a == NULL) {
//        jniThrowException(env, "java/io/FileNotFoundException", fileName8);
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    android::ResXMLTree* block = new android::ResXMLTree();
    android::status_t err = block->setTo(a->getBuffer(true), a->getLength(), true);
    a->close();
    delete a;

    if (err != android::NO_ERROR) {
//        jniThrowException(env, "java/io/FileNotFoundException", "Corrupt XML binary file");
        return E_FILE_NOT_FOUND_EXCEPTION;
    }

    *result = (Int32)block;
    return NOERROR;
}

ECode CAssetManager::GetArrayStringResource(
    /* [in] */ Int32 arrayRes,
    /* [out] */ ArrayOf<String>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());

    const android::ResTable::bag_entry* startOfBag;
    const ssize_t N = res.lockBag(arrayRes, &startOfBag);
    if (N < 0) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<String> > array = ArrayOf<String>::Alloc(N);

    android::Res_value value;
    const android::ResTable::bag_entry* bag = startOfBag;
    size_t strLen = 0;
    for (size_t i = 0; ((ssize_t)i) < N; i++, bag++) {
        value = bag->map.value;
        String str;

        // Take care of resolving the found resource to its final value.
        ssize_t block = res.resolveReference(&value, bag->stringBlock, NULL);
#if THROW_ON_BAD_ID
        if (block == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! line: %d", __LINE__);
            *result = array;
            REFCOUNT_ADD(*result);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif
        if (value.dataType == android::Res_value::TYPE_STRING) {
            const android::ResStringPool* pool = res.getTableStringBlock(block);
            const char* str8 = pool->string8At(value.data, &strLen);
            if (str8 != NULL) {
                str = str8;
            }
            else {
                const char16_t* str16 = pool->stringAt(value.data, &strLen);
                android::String8 s8(str16);
                str = s8.string();
            }

            // If one of our NewString{UTF} calls failed due to memory, an
            // exception will be pending.
            (*array)[i] = str;

            // str is not NULL at that point, otherwise ExceptionCheck would have been true.
            // If we have a large amount of strings in our array, we might
            // overflow the local reference table of the VM.
        }
    }
    res.unlockBag(startOfBag);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAssetManager::GetArrayStringInfo(
    /* [in] */ Int32 arrayRes,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());

    const android::ResTable::bag_entry* startOfBag;
    const ssize_t N = res.lockBag(arrayRes, &startOfBag);
    if (N < 0) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(N * 2);

    android::Res_value value;
    const android::ResTable::bag_entry* bag = startOfBag;
    for (size_t i = 0, j = 0; ((ssize_t)i) < N; i++, bag++) {
        uint32_t stringIndex = -1;
        ssize_t stringBlock = 0;
        value = bag->map.value;

        // Take care of resolving the found resource to its final value.
        stringBlock = res.resolveReference(&value, bag->stringBlock, NULL);
        if (value.dataType == android::Res_value::TYPE_STRING) {
            stringIndex = value.data;
        }

#if THROW_ON_BAD_ID
        if (stringBlock == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! line: %d", __LINE__);
            *result = array;
            REFCOUNT_ADD(*result);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif

        //todo: It might be faster to allocate a C array to contain
        //      the blocknums and indices, put them in there and then
        //      do just one SetIntArrayRegion()
        (*array)[j] = (Int32)stringBlock;
        (*array)[j + 1] = (Int32)stringIndex;
        j = j + 2;
    }
    res.unlockBag(startOfBag);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAssetManager::Init(
    /* [in] */ Boolean isSystem)
{
    // TODO
    if (isSystem) {
        VerifySystemIdmaps();
    }

    android::AssetManager* am = new android::AssetManager();
    if (am == NULL) {
        // jniThrowException(env, "java/lang/OutOfMemoryError", "");
        return E_OUT_OF_MEMORY_ERROR;
    }

    am->addDefaultAssets();

    if (LocalLOGV) Logger::V(TAG, "Created AssetManager %p for object %p\n", am, this);
    mObject = reinterpret_cast<Int64>(am);
    return NOERROR;
}

ECode CAssetManager::GetBasePackageCount(
    /* [out] */ Int32* pkgCount)
{
    VALIDATE_NOT_NULL(pkgCount)
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        *pkgCount = 0;
        return NOERROR;
    }

    *pkgCount = am->getResources().getBasePackageCount();
    return NOERROR;
}

ECode CAssetManager::GetBasePackageName(
    /* [in] */ Int32 index,
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = String(NULL);
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    *pkgName = String(am->getBasePackageName(index));
    return NOERROR;
}

ECode CAssetManager::GetBaseResourcePackageName(
    /* [in] */ Int32 index,
    /* [out] */ String* pkgName)
{
    VALIDATE_NOT_NULL(pkgName)
    *pkgName = String(NULL);
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    *pkgName = String((const char*)am->getResources().getBasePackageName(index).string());
    return NOERROR;
}

ECode CAssetManager::GetBasePackageId(
    /* [in] */ Int32 index,
    /* [out] */ Int32* pkgId)
{
    VALIDATE_NOT_NULL(pkgId)
    *pkgId = 0;
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    *pkgId = am->getResources().getBasePackageId(index);
    return NOERROR;
}

void CAssetManager::Destroy()
{
    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am != NULL) {
        delete am;
        mObject = 0;
        if (LocalLOGV) Logger::V(TAG, "Destroying AssetManager %p for object %p\n", am, this);
    }
}

ECode CAssetManager::GetArrayIntResource(
    /* [in] */ Int32 arrayRes,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }
    const android::ResTable& res(am->getResources());

    const android::ResTable::bag_entry* startOfBag;
    const ssize_t N = res.lockBag(arrayRes, &startOfBag);
    if (N < 0) {
        return NOERROR;
    }

    AutoPtr< ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(N);
    if (array == NULL) {
        res.unlockBag(startOfBag);
        return NOERROR;
    }

    android::Res_value value;
    const android::ResTable::bag_entry* bag = startOfBag;
    for (size_t i = 0; ((ssize_t)i) < N; i++, bag++) {
        value = bag->map.value;

        // Take care of resolving the found resource to its final value.
        ssize_t block = res.resolveReference(&value, bag->stringBlock, NULL);
#if THROW_ON_BAD_ID
        if (block == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! line: %d", __LINE__);
            *result = array;
            REFCOUNT_ADD(*result);
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif
        if (value.dataType >= android::Res_value::TYPE_FIRST_INT
                && value.dataType <= android::Res_value::TYPE_LAST_INT) {
            (*array)[i] = (Int32)value.data;
        }
    }
    res.unlockBag(startOfBag);
    *result = array;
    REFCOUNT_ADD(*result);
    return NOERROR;
}

ECode CAssetManager::GetStyleAttributes(
    /* [in] */ Int32 styleId,
    /* [out] */ ArrayOf<Int32>** result)
{
    VALIDATE_NOT_NULL(result)
    *result = NULL;

    android::AssetManager* am = reinterpret_cast<android::AssetManager*>(mObject);
    if (am == NULL) {
        return NOERROR;
    }

    const android::ResTable& res(am->getResources());
    const android::ResTable::bag_entry* startOfBag;
    const ssize_t N = res.lockBag(styleId, &startOfBag);
    if (N < 0) {
        return NOERROR;
    }

    AutoPtr<ArrayOf<Int32> > array = ArrayOf<Int32>::Alloc(N);
    if (array == NULL) {
        res.unlockBag(startOfBag);
        return NOERROR;
    }

    android::Res_value value;
    const android::ResTable::bag_entry* bag = startOfBag;
    for (size_t i=0; ((ssize_t)i)<N; i++, bag++) {
        Int32 resourceId = bag->map.name.ident;
        array->Set(i, resourceId);
    }
    res.unlockBag(startOfBag);

    *result = array;
    REFCOUNT_ADD(*result)
    return NOERROR;
}

void CAssetManager::ApplyThemeStyle(
    /* [in] */ Int64 themeToken,
    /* [in] */ Int32 styleRes,
    /* [in] */ Boolean force)
{
    android::ResTable::Theme* theme = (android::ResTable::Theme*)themeToken;
    theme->applyStyle(styleRes, force ? true : false);
}

void CAssetManager::CopyTheme(
    /* [in] */ Int64 destToken,
    /* [in] */ Int64 sourceToken)
{
    android::ResTable::Theme* dest = (android::ResTable::Theme*)destToken;
    android::ResTable::Theme* src = (android::ResTable::Theme*)sourceToken;
    dest->setTo(*src);
}

ECode CAssetManager::LoadThemeAttributeValue(
    /* [in] */ Int64 themeToken,
    /* [in] */ Int32 ident,
    /* [in] */ ITypedValue* outValue,
    /* [in] */ Boolean resolve,
    /* [out] */ Int32* result)
{
    VALIDATE_NOT_NULL(result)
    *result = 0;

    android::ResTable::Theme* theme = (android::ResTable::Theme*)themeToken;
    const android::ResTable& res(theme->getResTable());

    android::Res_value value;
    // XXX value could be different in different configs!
    uint32_t typeSpecFlags = 0;
    ssize_t block = theme->getAttribute(ident, &value, &typeSpecFlags);
    uint32_t ref = 0;
    if (resolve) {
        block = res.resolveReference(&value, block, &ref, &typeSpecFlags);
#if THROW_ON_BAD_ID
        if (block == android::BAD_INDEX) {
            Logger::E(TAG, "Bad resource! Line: %d", __LINE__);
            *result = 0;
            return E_ILLEGAL_STATE_EXCEPTION;
        }
#endif
    }
    if (block >= 0) {
        outValue->SetType((Int32)value.dataType);
        outValue->SetAssetCookie((Int32)res.getTableCookie(block));
        outValue->SetData((Int32)value.data);
        outValue->SetString(NULL);
        outValue->SetResourceId((Int32)ref);
        outValue->SetChangingConfigurations((Int32)typeSpecFlags);
    }
    *result = block;
    return NOERROR;
}

void CAssetManager::DumpTheme(
    /* [in] */ Int64 themeToken,
    /* [in] */ Int32 priority,
    /* [in] */ const String& tag,
    /* [in] */ const String& prefix)
{
    android::ResTable::Theme* theme = (android::ResTable::Theme*)themeToken;
    const android::ResTable& __attribute__((__unused__)) res(theme->getResTable());

    // XXX Need to use params.
    theme->dumpToLog();
}

void CAssetManager::IncRefsLocked(
    /* [in] */ Int64 id,
    /* [in] */ const char* info)
{
    // if (DEBUG_REFS) {
    //     if (mRefStacks == null) {
    //         mRefStacks = new HashMap<Integer, RuntimeException>();
    //         RuntimeException ex = new RuntimeException();
    //         ex.fillInStackTrace();
    //         mRefStacks.put(this.hashCode(), ex);
    //     }
    // }
    mNumRefs++;

    if (DEBUG_REFS) {
        String stack;
        stack.AppendFormat("CAssetManager::IncRefsLocked by %s, ref count: %d, obj:%p",
            info, mNumRefs, this);
        mRefStacks[id] = stack;
    }
}

void CAssetManager::DecRefsLocked(
    /* [in] */ Int64 id,
    /* [in] */ const char* info)
{
    // if (DEBUG_REFS && mRefStacks != null) {
    //     mRefStacks.remove(id);
    // }
    mNumRefs--;

    if (DEBUG_REFS) {
        mRefStacks.Erase(id);
    }

    //System.out.println("Dec streams: mNumRefs=" + mNumRefs
    //                   + " mReleased=" + mReleased);
    if (mNumRefs == 0) {
        Destroy();
    }
}

Int32 CAssetManager::GetHashCode()
{
    return (Int32)this;
}

Int64 CAssetManager::Ni()
{
    return mObject;
}

ECode CAssetManager::GetNativeAssetManager(
    /* [out] */ Int64* assetMgr)
{
    VALIDATE_NOT_NULL(assetMgr);
    *assetMgr = mObject;
    return NOERROR;
}

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos
