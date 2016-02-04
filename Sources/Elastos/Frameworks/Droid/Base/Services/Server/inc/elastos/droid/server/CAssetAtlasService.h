
#ifndef __ELASTOS_DROID_SERVER_CASSETATLAS_SERVICE_H__
#define __ELASTOS_DROID_SERVER_CASSETATLAS_SERVICE_H__

#include "elastos/droid/ext/frameworkext.h"
#include "_Elastos_Droid_Server_CAssetAtlasService.h"
#include "elastos/droid/server/SystemService.h"
#include <elastos/droid/os/Runnable.h>
#include <elastos/utility/etl/List.h>
#include <Elastos.Droid.View.h>
#include <Elastos.Droid.Content.h>
#include <Elastos.Droid.Graphics.h>
#include <Elastos.CoreLibrary.Utility.Concurrent.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Pm::IPackageInfo;
using Elastos::Droid::Content::Pm::IPackageManager;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::IAtlas;
using Elastos::Droid::Graphics::IAtlasEntry;
using Elastos::Droid::Graphics::AtlasType;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Graphics::ICanvas;
using Elastos::Droid::Graphics::IPaint;
using Elastos::Droid::Graphics::IPixelFormat;
using Elastos::Droid::Graphics::IPorterDuffXfermode;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Os::IEnvironment;
using Elastos::Droid::Os::ISystemProperties;
using Elastos::Droid::Utility::IInt64SparseArray;
using Elastos::Droid::View::IGraphicBuffer;
using Elastos::Droid::View::IIAssetAtlas;

using Elastos::Core::IComparator;
using Elastos::IO::IBufferedReader;
using Elastos::IO::IBufferedWriter;
using Elastos::IO::IFile;
using Elastos::IO::IFileInputStream;
using Elastos::IO::IFileOutputStream;
using Elastos::IO::IInputStreamReader;
using Elastos::IO::IOutputStreamWriter;
using Elastos::Utility::IArrayList;
using Elastos::Utility::ICollections;
using Elastos::Utility::IList;
using Elastos::Utility::Etl::List;
using Elastos::Utility::Concurrent::ICountDownLatch;
using Elastos::Utility::Concurrent::ITimeUnit;
using Elastos::Utility::Concurrent::Atomic::IAtomicBoolean;

namespace Elastos {
namespace Droid {
namespace Server {

/**
 * This service is responsible for packing preloaded bitmaps into a single
 * atlas texture. The resulting texture can be shared across processes to
 * reduce overall memory usage.
 *
 * @hide
 */
CarClass(CAssetAtlasService)
    , public Object
    , public IIAssetAtlas
    , public IBinder
{
private:

    class BitmapComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* o1,
            /* [in] */ IInterface* o2,
            /* [out] */ Int32* result);
    };

    class WorkerResultComparator
        : public Object
        , public IComparator
    {
    public:
        CAR_INTERFACE_DECL()

        //@Override
        CARAPI Compare(
            /* [in] */ IInterface* o1,
            /* [in] */ IInterface* o2,
            /* [out] */ Int32* result);
    };

    /**
     * The renderer does all the work:
     */
    class Renderer
        : public Runnable
    {
    public:

        Renderer(
            /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
            /* [in] */ Int32 pixelCount,
            /* [in] */ CAssetAtlasService* host);

        /**
         * 1. On first boot or after every update, brute-force through all the
         *    possible atlas configurations and look for the best one (maximimize
         *    number of packed assets and minimize texture size)
         *    a. If a best configuration was computed, write it out to disk for
         *       future use
         * 2. Read best configuration from disk
         * 3. Compute the packing using the best configuration
         * 4. Allocate a GraphicBuffer
         * 5. Render assets in the buffer
         */
        //@Override
        CARAPI Run();

        /**
         * Renders a list of bitmaps into the atlas. The position of each bitmap
         * was decided by the packing algorithm and will be honored by this
         * method. If need be this method will also rotate bitmaps.
         *
         * @param buffer The buffer to render the atlas entries into
         * @param atlas The atlas to pack the bitmaps into
         * @param packCount The number of bitmaps that will be packed in the atlas
         *
         * @return TRUE if the atlas was rendered, FALSE otherwise
         */
        //@SuppressWarnings("MismatchedReadAndWriteOfArray")
    private:
        Boolean RenderAtlas(
            /* [in] */ IGraphicBuffer* buffer,
            /* [in] */ IAtlas* atlas,
            /* [in] */ Int32 packCount);

        /**
         * Returns a Canvas for the specified buffer. If {@link #DEBUG_ATLAS_TEXTURE}
         * is turned on, the returned Canvas will render into a local bitmap that
         * will then be saved out to disk for debugging purposes.
         * @param width
         * @param height
         */
        AutoPtr<ICanvas> AcquireCanvas(
            /* [in] */ Int32 width,
            /* [in] */ Int32 height);

        /**
         * Releases the canvas used to render into the buffer. Calling this method
         * will release any resource previously acquired. If {@link #DEBUG_ATLAS_TEXTURE}
         * is turend on, calling this method will write the content of the atlas
         * to disk in /data/system/atlas.png for debugging.
         */
        void ReleaseCanvas(
            /* [in] */ ICanvas* canvas);

    private:
        AutoPtr< List<AutoPtr<IBitmap> > > mBitmaps;
        Int32 mPixelCount;

        Int64 mNativeBitmap;

        // Used for debugging only
        AutoPtr<IBitmap> mAtlasBitmap;

        CAssetAtlasService* mHost;
    };

    /**
     * Atlas configuration. Specifies the algorithm, dimensions and flags to use.
     */
    class Configuration
        : public Object
    {
    public:
        Configuration(
            /* [in] */ AtlasType type,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 count);

        Configuration(
            /* [in] */ AtlasType type,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 count,
            /* [in] */ Int32 flags);

        //@Override
        CARAPI ToString(
        /* [out] */ String* str);

    public:
        AtlasType mType;
        Int32 mWidth;
        Int32 mHeight;
        Int32 mCount;
        Int32 mFlags;
    };

    /**
     * Used during the brute-force search to gather information about each
     * variant of the packing algorithm.
     */
    class WorkerResult
        : public Object
    {
    public:
        AtlasType mType;
        Int32 mWidth;
        Int32 mHeight;
        Int32 mCount;

        WorkerResult(
            /* [in] */ AtlasType type,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ Int32 count);

        //@Override
        CARAPI ToString(
            /* [out] */ String* str);
    };

    /**
     * A compute worker will try a finite number of variations of the packing
     * algorithms and save the results in a supplied list.
     */
    class ComputeWorker
        : public Runnable
    {
    public:
        /**
         * Creates a new compute worker to brute-force through a range of
         * packing algorithms variants.
         *
         * @param start The minimum texture width to try
         * @param end The maximum texture width to try
         * @param step The number of pixels to increment the texture width by at each step
         * @param bitmaps The list of bitmaps to pack in the atlas
         * @param pixelCount The total number of pixels occupied by the list of bitmaps
         * @param results The list of results in which to save the brute-force search results
         * @param signal Latch to decrement when this worker is done, may be NULL
         */
        ComputeWorker(
            /* [in] */ Int32 start,
            /* [in] */ Int32 end,
            /* [in] */ Int32 step,
            /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
            /* [in] */ Int32 pixelCount,
            /* [in] */ IList* results,
            /* [in] */ ICountDownLatch* signal);

        //@Override
        CARAPI Run();

    private:
        Int32 PackBitmaps(
            /* [in] */ AtlasType type,
            /* [in] */ Int32 width,
            /* [in] */ Int32 height,
            /* [in] */ IAtlasEntry* entry);

    private:
        Int32 mStart;
        Int32 mEnd;
        Int32 mStep;
        AutoPtr< List<AutoPtr<IBitmap> > > mBitmaps;
        AutoPtr<IList> mResults;
        AutoPtr<ICountDownLatch> mSignal;
        Int32 mThreshold;
    };

public:
    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    CAssetAtlasService();

    /**
     * Creates a new service. Upon creating, the service will gather the list of
     * assets to consider for packing into the atlas and spawn a new thread to
     * start the packing work.
     *
     * @param context The context giving access to preloaded resources
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * Callback invoked by the server thread to indicate we can now run
     * 3rd party code.
     */
    CARAPI SystemRunning();


    //@Override
    CARAPI IsCompatible(
        /* [in] */ Int32 ppid,
        /* [out] */ Boolean* result);

    //@Override
    CARAPI GetBuffer(
        /* [out] */ IGraphicBuffer** gb);

    //@Override
    CARAPI GetMap(
        /* [out, callee] */ ArrayOf<Int64>** map);

    CARAPI ToString(
        /* [out] */ String* str)
    {
        return Object::ToString(str);
    }
private:

    /**
     * Queries the version name stored in framework's AndroidManifest.
     * The version name can be used to identify possible changes to
     * framework resources.
     *
     * @see #GetBuildIdentifier(String)
     */
    static String QueryVersionName(
        /* [in] */ IContext* context);

    static Int64 NativeAcquireAtlasCanvas(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int32 width,
        /* [in] */ Int32 height);

    static void NativeReleaseAtlasCanvas(
        /* [in] */ ICanvas* canvas,
        /* [in] */ Int64 bitmap);

    static Boolean NavtiveUploadAtlas(
        /* [in] */ IGraphicBuffer* buffer,
        /* [in] */ Int64 bitmap);

    /**
     * Finds the best atlas configuration to pack the list of supplied bitmaps.
     * This method takes advantage of multi-core systems by spawning a number
     * of threads equal to the number of available cores.
     */
    static AutoPtr<Configuration> ComputeBestConfiguration(
        /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
        /* [in] */ Int32 pixelCount);

    /**
     * Returns the path to the file containing the best computed
     * atlas configuration.
     */
    static AutoPtr<IFile> GetDataFile();

    static void DeleteDataFile();

    AutoPtr<IFile> GetFrameworkResourcesFile();

    /**
     * Returns the best known atlas configuration. This method will either
     * read the configuration from disk or start a brute-force search
     * and save the result out to disk.
     */
    AutoPtr<Configuration> ChooseConfiguration(
        /* [in] */ List<AutoPtr<IBitmap> >* bitmaps,
        /* [in] */ Int32 pixelCount,
        /* [in] */ const String& versionName);

    /**
     * Writes the specified atlas configuration to the specified file.
     */
    void WriteConfiguration(
        /* [in] */ Configuration* config,
        /* [in] */ IFile* file,
        /* [in] */ const String& versionName);

    /**
     * Reads an atlas configuration from the specified file. This method
     * returns NULL if an error occurs or if the configuration is invalid.
     */
    AutoPtr<Configuration> ReadConfiguration(
        /* [in] */ IFile* file,
        /* [in] */ const String& versionName);

    static Int32 ReadInt(
        /* [in] */ IBufferedReader* reader,
        /* [in] */ Int32 min,
        /* [in] */ Int32 max);

    /**
     * Compares the next line in the specified buffered reader to the current
     * build identifier. Returns whether the two values are equal.
     *
     * @see #GetBuildIdentifier(String)
     */
    Boolean CheckBuildIdentifier(
        /* [in] */ IBufferedReader* reader,
        /* [in] */ const String& versionName);

    /**
     * Returns an identifier for the current build that can be used to detect
     * likely changes to framework resources. The build identifier is made of
     * several distinct values:
     *
     * build fingerprint/framework version name/file size of framework resources apk
     *
     * Only the build fingerprint should be necessary on user builds but
     * the other values are useful to detect changes on eng builds during
     * development.
     *
     * This identifier does not attempt to be exact: a new identifier does not
     * necessarily mean the preloaded drawables have changed. It is important
     * however that whenever the list of preloaded drawables changes, this
     * identifier changes as well.
     *
     * @see #CheckBuildIdentifier(java.io.BufferedReader, String)
     */
    String GetBuildIdentifier(
        /* [in] */ const String& versionName);

public:
    /**
     * Name of the <code>AssetAtlasService</code>.
     */
    static const String ASSET_ATLAS_SERVICE;

private:

    static const String TAG;

    // Turns debug logs on/off. Debug logs are kept to a minimum and should
    // remain on to diagnose issues
    static const Boolean DEBUG_ATLAS;

    // When set to TRUE the content of the atlas will be saved to disk
    // in /data/system/atlas.png. The shared GraphicBuffer may be empty
    static const Boolean DEBUG_ATLAS_TEXTURE;

    // Minimum size in pixels to consider for the resulting texture
    static const Int32 MIN_SIZE;
    // Maximum size in pixels to consider for the resulting texture
    static const Int32 MAX_SIZE;
    // Increment in number of pixels between size variants when looking
    // for the best texture dimensions
    static const Int32 STEP;

    // This percentage of the total number of pixels represents the minimum
    // number of pixels we want to be able to pack in the atlas
    static const Float PACKING_THRESHOLD;

    // Defines the number of Int32 fields used to represent a single entry
    // in the atlas map. This number defines the size of the array returned
    // by the GetMap(). See the mAtlasMap field for more information
    static const Int32 ATLAS_MAP_ENTRY_FIELD_COUNT;

    // Specifies how our GraphicBuffer will be used. To get proper swizzling
    // the buffer will be written to using OpenGL (from JNI) so we can leave
    // the software flag set to "never"
    static const Int32 GRAPHIC_BUFFER_USAGE;

    // This Boolean is set to TRUE if an atlas was successfully
    // computed and rendered
    AutoPtr<IAtomicBoolean> mAtlasReady;

    AutoPtr<IContext> mContext;

    // Version name of the current build, used to identify changes to assets list
    String mVersionName;

    // Holds the atlas' data. This buffer can be mapped to
    // OpenGL using an EGLImage
    AutoPtr<IGraphicBuffer> mBuffer;

    // Describes how bitmaps are placed in the atlas. Each bitmap is
    // represented by several entries in the array:
    // long0: SkBitmap*, the native bitmap object
    // long1: x position
    // long2: y position
    // long3: rotated, 1 if the bitmap must be rotated, 0 otherwise
    AutoPtr<ArrayOf<Int64> > mAtlasMap;
};


} // Server
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_SERVER_CALARMMANAGERSERVICE_H__
