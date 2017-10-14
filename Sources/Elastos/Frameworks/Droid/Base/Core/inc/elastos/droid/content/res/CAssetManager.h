//=========================================================================
// Copyright (C) 2012 The Elastos Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//=========================================================================

#ifndef __ELASTOS_DROID_CONTENT_RES_CASSETMANAGER_H__
#define __ELASTOS_DROID_CONTENT_RES_CASSETMANAGER_H__

#include "_Elastos_Droid_Content_Res_CAssetManager.h"
#include "elastos/droid/content/res/StringBlock.h"
#include "elastos/droid/content/res/XmlBlock.h"
#include <elastos/io/InputStream.h>
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IParcelFileDescriptor;
using Elastos::Droid::Utility::ITypedValue;
using Elastos::Droid::Utility::ISparseArray;
using Elastos::IO::ICloseable;
using Elastos::IO::InputStream;
using Elastos::Utility::IArrayList;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace Content {
namespace Res {

/**
 * Provides access to an application's raw asset files; see {@link Resources}
 * for the way most applications will want to retrieve their resource data.
 * This class presents a lower-level API that allows you to open and read raw
 * files that have been bundled with the application as a simple stream of
 * bytes.
 */
CarClass(CAssetManager)
    , public Object
    , public IAssetManager
    , public ICloseable
{
public:
    class AssetInputStream
        : public InputStream
        , public IAssetInputStream
    {
    public:
        CAR_INTERFACE_DECL()

        AssetInputStream(
            /* [in] */ CAssetManager* assetManager,
            /* [in] */ Int64 asset);

        virtual ~AssetInputStream();

        CARAPI Available(
            /* [out] */ Int32* bytes);

        CARAPI Close();

        CARAPI Mark(
            /* [in] */ Int32 readLimit);

        CARAPI IsMarkSupported(
            /* [out] */ Boolean* supported);

        CARAPI Read(
            /* [out] */ Int32* value);

        CARAPI ReadBytes(
            /* [out] */ ArrayOf<Byte>* buffer,
            /* [out] */ Int32* number);

        CARAPI ReadBytes(
            /* [out] */ ArrayOf<Byte>* buffer,
            /* [in] */ Int32 offset,
            /* [in] */ Int32 length,
            /* [out] */ Int32* number);

        CARAPI Reset();

        CARAPI Skip(
            /* [in] */ Int64 count,
            /* [out] */ Int64* number);

        /**
         * @hide
         */
        CARAPI GetAssetInt32(
            /* [out] */ Int32* value);

        /**
         * @hide
         */
        CARAPI GetNativeAsset(
            /* [out] */ Int64* value);

    private:
        AutoPtr<CAssetManager> mAssetManager;
        Int64 mAsset;
        Int64 mLength;
        Int64 mMarkPos;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAssetManager();

    virtual ~CAssetManager();

    CARAPI constructor();

    CARAPI constructor(
        /* [in] */ Boolean isSystem);

    /**
     * Return a global shared asset manager that provides access to only
     * system assets (no application assets).
     * {@hide}
     */
    static CARAPI_(AutoPtr<CAssetManager>) GetSystem();

    /**
     * Close this asset manager.
     */
    CARAPI Close();

    /**
     * Retrieve the string value associated with a particular resource
     * identifier for the current configuration / skin.
     */
    /*package*/
    CARAPI_(AutoPtr<ICharSequence>) GetResourceText(
        /* [in] */ Int32 ident);

    /**
     * Retrieve the string value associated with a particular resource
     * identifier for the current configuration / skin.
     */
    /*package*/
    CARAPI_(AutoPtr<ICharSequence>) GetResourceBagText(
        /* [in] */ Int32 ident,
        /* [in] */ Int32 bagEntryId);

    /**
     * Retrieve the string array associated with a particular resource
     * identifier.
     * @param id Resource id of the string array
     */
    /*package*/
    CARAPI_(AutoPtr< ArrayOf<String> >) GetResourceStringArray(
        /* [in] */ Int32 id);

    /*package*/
    CARAPI_(Boolean) GetResourceValue(
        /* [in] */ Int32 ident,
        /* [in] */ Int32 density,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolveRefs);

    /**
     * Retrieve the text array associated with a particular resource
     * identifier.
     * @param id Resource id of the string array
     */
    /*package*/
    CARAPI_(AutoPtr< ArrayOf<ICharSequence*> >) GetResourceTextArray(
        /* [in] */ Int32 id);

    /*package*/
    CARAPI_(Boolean) GetThemeValue(
        /* [in] */ Int64 theme,
        /* [in] */ Int32 ident,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolveRefs);

    /*package*/
    CARAPI_(void) EnsureStringBlocks();

    /*package*/
    CARAPI_(void) RecreateStringBlocks();

    /*package*/
    CARAPI_(void) MakeStringBlocks(
        /* [in] */ ArrayOf<StringBlock*>* seed);

    /*package*/
    CARAPI_(AutoPtr<ICharSequence>) GetPooledStringForCookie(
        /* [in] */ Int32 block,
        /* [in] */ Int32 id);

    /**
     * Open an asset using ACCESS_STREAMING mode.  This provides access to
     * files that have been bundled with an application as assets -- that is,
     * files placed in to the "assets" directory.
     *
     * @param fileName The name of the asset to open.  This name can be
     *                 hierarchical.
     *
     * @see #open(String, int)
     * @see #list
     */
    CARAPI Open(
        /* [in] */ const String& fileName,
        /* [out] */ IInputStream** stream);

    /**
     * Open an asset using an explicit access mode, returning an InputStream to
     * read its contents.  This provides access to files that have been bundled
     * with an application as assets -- that is, files placed in to the
     * "assets" directory.
     *
     * @param fileName The name of the asset to open.  This name can be
     *                 hierarchical.
     * @param accessMode Desired access mode for retrieving the data.
     *
     * @see #ACCESS_UNKNOWN
     * @see #ACCESS_STREAMING
     * @see #ACCESS_RANDOM
     * @see #ACCESS_BUFFER
     * @see #open(String)
     * @see #list
     */
    CARAPI Open(
        /* [in] */ const String& fileName,
        /* [in] */ Int32 accessMode,
        /* [out] */ IInputStream** stream);

    CARAPI OpenFd(
        /* [in] */ const String& fileName,
        /* [out] */ IAssetFileDescriptor** fd);

    /**
     * Return a String array of all the assets at the given path.
     *
     * @param path A relative path within the assets, i.e., "docs/home.html".
     *
     * @return String[] Array of strings, one for each asset.  These file
     *         names are relative to 'path'.  You can open the file by
     *         concatenating 'path' and a name in the returned string (via
     *         File) and passing that to open().
     *
     * @see #open
     */
    CARAPI List(
        /* [in] */ const String& path,
        /* [out, callee] */ ArrayOf<String>** names);

    /**
     * {@hide}
     * Open a non-asset file as an asset using ACCESS_STREAMING mode.  This
     * provides direct access to all of the files included in an application
     * package (not only its assets).  Applications should not normally use
     * this.
     *
     * @see #open(String)
     */
    CARAPI OpenNonAsset(
        /* [in] */ const String& fileName,
        /* [out] */ IInputStream** stream);

    /**
     * {@hide}
     * Open a non-asset file as an asset using a specific access mode.  This
     * provides direct access to all of the files included in an application
     * package (not only its assets).  Applications should not normally use
     * this.
     *
     * @see #open(String, int)
     */
    CARAPI OpenNonAsset(
        /* [in] */ const String& fileName,
        /* [in] */ Int32 accessMode,
        /* [out] */ IInputStream ** stream);

    /**
     * {@hide}
     * Open a non-asset in a specified package.  Not for use by applications.
     *
     * @param cookie Identifier of the package to be opened.
     * @param fileName Name of the asset to retrieve.
     */
    CARAPI OpenNonAsset(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [out] */ IInputStream ** stream);

    /**
     * {@hide}
     * Open a non-asset in a specified package.  Not for use by applications.
     *
     * @param cookie Identifier of the package to be opened.
     * @param fileName Name of the asset to retrieve.
     * @param accessMode Desired access mode for retrieving the data.
     */
    CARAPI OpenNonAsset(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [in] */ Int32 accessMode,
        /* [out] */ IInputStream ** stream);

    CARAPI OpenNonAssetFd(
        /* [in] */ const String& fileName,
        /* [out] */ IAssetFileDescriptor ** fd);

    CARAPI OpenNonAssetFd(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [out] */ IAssetFileDescriptor ** fd);

    /**
     * Retrieve a parser for a compiled XML file.
     *
     * @param fileName The name of the file to retrieve.
     */
    CARAPI OpenXmlResourceParser(
        /* [in] */ const String& fileName,
        /* [out] */ IXmlResourceParser ** parser);

    /**
     * Retrieve a parser for a compiled XML file.
     *
     * @param cookie Identifier of the package to be opened.
     * @param fileName The name of the file to retrieve.
     */
    CARAPI OpenXmlResourceParser(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [out] */ IXmlResourceParser ** parser);

    /**
     * {@hide}
     * Retrieve a non-asset as a compiled XML file.  Not for use by
     * applications.
     *
     * @param fileName The name of the file to retrieve.
     */
    /*package*/
    CARAPI OpenXmlBlockAsset(
        /* [in] */ const String& fileName,
        /* [out] */ XmlBlock** xmlBlock);

    /**
     * {@hide}
     * Retrieve a non-asset as a compiled XML file.  Not for use by
     * applications.
     *
     * @param cookie Identifier of the package to be opened.
     * @param fileName Name of the asset to retrieve.
     */
    /*package*/
    CARAPI OpenXmlBlockAsset(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [out] */ XmlBlock** res);

    /*package*/
    CARAPI_(void) XmlBlockGone(
        /* [in] */ Int32 id);

    /*package*/
    CARAPI CreateTheme(
        /* [out] */ Int64* res);

    /*package*/
    CARAPI ReleaseTheme(
        /* [in] */ Int64 theme);

    /**
     * Add an additional set of assets to the asset manager.  This can be
     * either a directory or ZIP file.  Not for use by applications.  Returns
     * the cookie of the added asset, or 0 on failure.
     * {@hide}
     */
    CARAPI AddAssetPath(
        /* [in] */ const String& path,
        /* [out] */ Int32* cookie);

    /**
     * Add multiple sets of assets to the asset manager at once.  See
     * {@link #addAssetPath(String)} for more information.  Returns array of
     * cookies for each added asset with 0 indicating failure, or null if
     * the input array of paths is null.
     * {@hide}
     */
    CARAPI AddAssetPaths(
        /* [in] */ ArrayOf<String>* paths,
        /* [out, callee] */ ArrayOf<Int32>** cookies);

    /**
     * Sets a flag indicating that this AssetManager should have themes
     * attached, according to the initial request to create it by the
     * ApplicationContext.
     *
     * {@hide}
     */
    CARAPI SetThemeSupport(
        /* [in] */ Boolean themeSupport);

    /**
     * Should this AssetManager have themes attached, according to the initial
     * request to create it by the ApplicationContext?
     *
     * {@hide}
     */
    CARAPI HasThemeSupport(
        /* [out] */ Boolean* hasThemeSupport);

    /**
     * Get package name of current icon pack (may return null).
     * {@hide}
     */
    CARAPI GetIconPackageName(
        /* [out] */ String* packageName);

    /**
     * Sets icon package name
     * {@hide}
     */
    CARAPI SetIconPackageName(
        /* [in] */ const String& packageName);

    /**
     * Get package name of current common resources (may return null).
     * {@hide}
     */
    CARAPI GetCommonResPackageName(
        /* [out] */ String* packageName);

    /**
     * Sets common resources package name
     * {@hide}
     */
    CARAPI SetCommonResPackageName(
        /* [in] */ const String& packageName);

    /**
     * Get package name of current theme (may return null).
     * {@hide}
     */
    CARAPI GetThemePackageName(
        /* [out] */ String* packageName);

    /**
     * Sets package name and highest level style id for current theme (null, 0 is allowed).
     * {@hide}
     */
    CARAPI SetThemePackageName(
        /* [in] */ const String& packageName);

    /**
     * Get asset cookie for current theme (may return 0).
     * {@hide}
     */
    CARAPI GetThemeCookies(
        /* [out] */ IArrayList** cookies);

    /** {@hide} */
    CARAPI SetIconPackCookie(
        /* [in] */ Int32 cookie);

    /** {@hide} */
    CARAPI GetIconPackCookie(
        /* [out] */ Int32* cookie);

    /** {@hide} */
    CARAPI SetCommonResCookie(
        /* [in] */ Int32 cookie);

    /** {@hide} */
    CARAPI GetCommonResCookie(
        /* [out] */ Int32* cookie);

    /**
     * Sets asset cookie for current theme (0 if not a themed asset manager).
     * {@hide}
     */
    CARAPI AddThemeCookie(
        /* [in] */ Int32 cookie);

    /** {@hide} */
    CARAPI GetAppName(
        /* [out] */ String* appName);

    /** {@hide} */
    CARAPI SetAppName(
        /* [in] */ const String& pkgName);

    /** {@hide} */
    CARAPI HasThemedAssets(
        /* [out] */ Boolean* hasThemeAssets);

     /**
     * Add a set of assets to overlay an already added set of assets.
     *
     * This is only intended for application resources. System wide resources
     * are handled before any Java code is executed.
     *
     * {@hide}
     */
    // TODO: change the signature of this method to match addOverlayPathNative
    CARAPI AddOverlayPath(
        /* [in] */ const String& idmapPath,
        /* [in] */ const String& themeApkPath,
        /* [in] */ const String& resApkPath,
        /* [in] */ const String& targetPkgPath,
        /* [in] */ const String& prefixPath,
        /* [out] */ Int32* result);

    /**
     * Add a set of common assets.
     *
     * {@hide}
     */
    CARAPI AddCommonOverlayPath(
        /* [in] */ const String& themeApkPath,
        /* [in] */ const String& resApkPath,
        /* [in] */ const String& prefixPath,
        /* [out] */ Int32* result);

    /**
     * Add a set of assets as an icon pack. A pkgIdOverride value will change the package's id from
     * what is in the resource table to a new value. Manage this carefully, if icon pack has more
     * than one package then that next package's id will use pkgIdOverride+1.
     *
     * Icon packs are different from overlays as they have a different pkg id and
     * do not use idmap so no targetPkg is required
     *
     * {@hide}
     */
    CARAPI AddIconPath(
        /* [in] */ const String& idmapPath,
        /* [in] */ const String& resApkPath,
        /* [in] */ const String& prefixPath,
        /* [in] */ Int32 pkgIdOverride,
        /* [out] */ Int32* result);

    /**
    * Delete a set of overlay assets from the asset manager. Not for use by
    * applications. Returns true if succeeded or false on failure.
    *
    * Also works for icon packs
    *
    * {@hide}
    */
    CARAPI RemoveOverlayPath(
        /* [in] */ const String& packageName,
        /* [in] */ Int32 cookie,
        /* [out] */ Boolean* result);

    /**
     * Determine whether the state in this asset manager is up-to-date with
     * the files on the filesystem.  If false is returned, you need to
     * instantiate a new AssetManager class to see the new data.
     * {@hide}
     */
    CARAPI IsUpToDate(
        /* [out] */ Boolean* result);

    /**
     * Change the locale being used by this asset manager.  Not for use by
     * applications.
     * {@hide}
     */
    CARAPI SetLocale(
        /* [in] */ const String& locale);

    /**
     * Get the locales that this asset manager contains data for.
     */
    CARAPI GetLocales(
        /* [out, callee] */ ArrayOf<String>** locales);

    /**
     * Change the configuation used when retrieving resources.  Not for use by
     * applications.
     * {@hide}
     */
    CARAPI SetConfiguration(
        /* [in] */ Int32 mcc,
        /* [in] */ Int32 mnc,
        /* [in] */ const String& locale,
        /* [in] */ Int32 orientation,
        /* [in] */ Int32 touchscreen,
        /* [in] */ Int32 density,
        /* [in] */ Int32 keyboard,
        /* [in] */ Int32 keyboardHidden,
        /* [in] */ Int32 navigation,
        /* [in] */ Int32 screenWidth,
        /* [in] */ Int32 screenHeight,
        /* [in] */ Int32 smallestScreenWidthDp,
        /* [in] */ Int32 screenWidthDp,
        /* [in] */ Int32 screenHeightDp,
        /* [in] */ Int32 screenLayout,
        /* [in] */ Int32 uiMode,
        /* [in] */ Int32 majorVersion);

    /**
     * Retrieve the resource identifier for the given resource name.
     */
    CARAPI_(Int32) GetResourceIdentifier(
        /* [in] */ const String& name,
        /* [in] */ const String& defType,
        /* [in] */ const String& defPackage);

    CARAPI_(String) GetResourceName(
        /* [in] */ Int32 resid);

    CARAPI_(String) GetResourcePackageName(
        /* [in] */ Int32 resid);

    CARAPI_(String) GetResourceTypeName(
        /* [in] */ Int32 resid);

    CARAPI_(String) GetResourceEntryName(
        /* [in] */ Int32 resid);

    /*package*/
    static CARAPI ApplyStyle(
        /* [in] */ Int64 theme,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes,
        /* [in] */ Int64 xmlParser,
        /* [in] */ const ArrayOf<Int32>& inAttrs,
        /* [in] */ ArrayOf<Int32>* outValues,
        /* [in] */ ArrayOf<Int32>* outIndices,
        /* [out] */ Boolean* result);

    /*package*/
    static CARAPI ResolveAttrs(
        /* [in] */ Int64 theme,
        /* [in] */ Int32 defStyleAttr,
        /* [in] */ Int32 defStyleRes,
        /* [in] */ const ArrayOf<Int32>& inValues,
        /* [in] */ const ArrayOf<Int32>& inAttrs,
        /* [in] */ ArrayOf<Int32>* outValues,
        /* [in] */ ArrayOf<Int32>* outIndices,
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI RetrieveAttributes(
        /* [in] */ Int64 xmlParser,
        /* [in] */ const ArrayOf<Int32>& inAttrs,
        /* [in] */ ArrayOf<Int32>* outValues,
        /* [in] */ ArrayOf<Int32>* outIndices,
        /* [out] */ Boolean* result);

    /*package*/
    CARAPI_(Int32) GetArraySize(
        /* [in] */ Int32 resource);

    /*package*/
    CARAPI RetrieveArray(
        /* [in] */ Int32 resource,
        /* [in] */ ArrayOf<Int32>* outValues,
        /* [out] */ Int32* result);

    /**
     * {@hide}
     */
    CARAPI GetCookieName(
        /* [in] */ Int32 cookie,
        /* [out] */ String* name);

    /**
     * {@hide}
     */
    CARAPI GetAssignedPackageIdentifiers(
        /* [out] */ ISparseArray** array);

    CARAPI GetNativeAssetManager(
        /* [out] */ Int64* assetMgr);

    /**
     * {@hide}
     */
    static CARAPI_(Int32) GetGlobalAssetCount();

    /**
     * {@hide}
     */
    static CARAPI_(String) GetAssetAllocations();

    /**
     * {@hide}
     */
    static CARAPI_(Int32) GetGlobalAssetManagerCount();

    /*package*/
    static CARAPI_(void) ApplyThemeStyle(
            /* [in] */ Int64 theme,
            /* [in] */ Int32 styleRes,
            /* [in] */ Boolean force);

    /*package*/
    static CARAPI_(void) CopyTheme(
        /* [in] */ Int64 dest,
        /* [in] */ Int64 source);

    /*package*/
    static CARAPI LoadThemeAttributeValue(
        /* [in] */ Int64 theme,
        /* [in] */ Int32 ident,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolve,
        /* [out] */ Int32* result);

    /*package*/
    static CARAPI_(void) DumpTheme(
        /* [in] */ Int64 theme,
        /* [in] */ Int32 priority,
        /* [in] */ const String& tag,
        /* [in] */ const String& prefix);

    /*package*/
    CARAPI GetArrayIntResource(
        /* [in] */ Int32 arrayRes,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI GetStyleAttributes(
        /* [in] */ Int32 arrayRes,
        /* [out] */ ArrayOf<Int32>** result);

    /**
     * {@hide}
     */
    CARAPI GetBasePackageCount(
        /* [out] */ Int32* pkgCount);

    /**
     * {@hide}
     */
    CARAPI GetBasePackageName(
        /* [in] */ Int32 index,
        /* [out] */ String* pkgName);

    /**
     * {@hide}
     */
    CARAPI GetBaseResourcePackageName(
        /* [in] */ Int32 index,
        /* [out] */ String* pkgName);

    /**
     * {@hide}
     */
    CARAPI GetBasePackageId(
        /* [in] */ Int32 index,
        /* [out] */ Int32* pkgId);

    using Object::GetHashCode;

    CARAPI_(Int32) GetHashCode();

    CARAPI_(Int64) Ni();

protected:
    CARAPI_(void) Finalize();

private:
    static CARAPI EnsureSystemAssets();

    CARAPI OpenAsset(
        /* [in] */ const String& fileName,
        /* [in] */ Int32 accessMode,
        /* [out] */ Int64* asset);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) OpenAssetFd(
        /* [in] */ const String& fileName,
        /* [in] */ ArrayOf<Int64>* outOffsets);

    CARAPI OpenNonAssetNative(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [in] */ Int32 accessMode,
        /* [out] */ Int64* value);

    CARAPI_(AutoPtr<IParcelFileDescriptor>) OpenNonAssetFdNative(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [in] */ ArrayOf<Int64>* outOffsets);

    CARAPI_(void) DestroyAsset(
            /* [in] */ Int64 asset);

    CARAPI ReadAssetChar(
        /* [in] */ Int64 asset,
        /* [out] */ Int32* value);

    CARAPI ReadAsset(
        /* [in] */ Int64 asset,
        /* [out] */ ArrayOf<Byte>* b,
        /* [in] */ Int32 off,
        /* [in] */ Int32 len,
        /* [out] */ Int32* number);

    CARAPI_(Int64) SeekAsset(
        /* [in] */ Int64 asset,
        /* [in] */ Int64 offset,
        /* [in] */ Int32 whence);

    CARAPI_(Int64) GetAssetLength(
        /* [in] */ Int64 asset);

    CARAPI_(Int64) GetAssetRemainingLength(
        /* [in] */ Int64 asset);

    /** Returns true if the resource was found, filling in mRetStringBlock and
     *  mRetData. */
    CARAPI LoadResourceValue(
        /* [in] */ Int32 ident,
        /* [in] */ Int16 density,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolve,
        /* [out] */ Int32* result);

    /** Returns true if the resource was found, filling in mRetStringBlock and
     *  mRetData. */
    CARAPI LoadResourceBagValue(
        /* [in] */ Int32 ident,
        /* [in] */ Int32 bagEntryId,
        /* [in] */ ITypedValue* outValue,
        /* [in] */ Boolean resolve,
        /* [out] */ Int32* result);

    CARAPI_(Int32) GetStringBlockCount();

    CARAPI_(Int64) GetNativeStringBlock(
        /* [in] */ Int32 block);

    CARAPI_(Int64) NewTheme();

    CARAPI_(void) DeleteTheme(
            /* [in] */ Int64 theme);

    CARAPI OpenXmlAssetNative(
        /* [in] */ Int32 cookie,
        /* [in] */ const String& fileName,
        /* [out] */ Int64* result);

    CARAPI GetArrayStringResource(
        /* [in] */ Int32 arrayRes,
        /* [out] */ ArrayOf<String>** result);

    CARAPI GetArrayStringInfo(
        /* [in] */ Int32 arrayRes,
        /* [out] */ ArrayOf<Int32>** result);

    CARAPI Init(
        /* [in] */ Boolean isSystem);

    CARAPI_(void) Destroy();

    CARAPI_(void) IncRefsLocked(
        /* [in] */ Int64 id,
        /* [in] */ const char* info);

    CARAPI_(void) DecRefsLocked(
        /* [in] */ Int64 id,
        /* [in] */ const char* info);

public:
    static AutoPtr<CAssetManager> sSystem;

    static const Int32 STYLE_NUM_ENTRIES = 6;
    static const Int32 STYLE_TYPE = 0;
    static const Int32 STYLE_DATA = 1;
    static const Int32 STYLE_ASSET_COOKIE = 2;
    static const Int32 STYLE_RESOURCE_ID = 3;
    static const Int32 STYLE_CHANGING_CONFIGURATIONS = 4;
    static const Int32 STYLE_DENSITY = 5;

private:
    static const String TAG;
    static const Boolean LocalLOGV;
    static const Boolean DEBUG_REFS;

    static Object sSync;

    const AutoPtr<ITypedValue> mValue;
    AutoPtr< ArrayOf<Int64> > mOffsets;

    // For communication with native code.
    Int64 mObject;

    AutoPtr< ArrayOf<StringBlock*> > mStringBlocks;
    HashMap<Int64, String> mRefStacks;

    String mAppName;

    Boolean mThemeSupport;
    String mThemePackageName;
    String mIconPackageName;
    String mCommonResPackageName;
    AutoPtr<IArrayList> mThemeCookies;
    Int32 mIconPackCookie;
    Int32 mCommonResCookie;

    Int32 mNumRefs;
    Boolean mOpen;
//  HashMap< Int32, AutoPtr<IRuntimeException> > mRefStacks;
};

} // namespace Res
} // namespace Content
} // namespace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_CONTENT_RES_CASSETMANAGER_H__
