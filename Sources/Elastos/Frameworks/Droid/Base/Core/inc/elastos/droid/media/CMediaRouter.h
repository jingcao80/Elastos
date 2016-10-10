
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__

#include "_Elastos_Droid_Media_CMediaRouter.h"
#include "Elastos.Droid.Media.h"
#include "Elastos.Droid.Hardware.h"
#include "Elastos.Droid.Os.h"
#include "elastos/droid/media/VolumeProvider.h"
#include "elastos/droid/media/MediaRouterRouteInfo.h"
#include "elastos/droid/content/BroadcastReceiver.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/HashMap.h>
#include <elastos/utility/etl/List.h>
#include <elastos/core/Object.h>
#include <elastos/droid/os/Handler.h>

using Elastos::Droid::Content::BroadcastReceiver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Graphics::Drawable::IDrawable;
using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::View::IDisplay;
using Elastos::Utility::Etl::HashMap;
using Elastos::Utility::Etl::List;
using Elastos::Utility::IArrayList;
using Elastos::Utility::IList;
using Elastos::Core::ICharSequence;
using Elastos::Core::IRunnable;
using Elastos::Core::ICharSequence;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaRouter)
    , public Object
    , public IMediaRouter
{
  friend class MediaRouterRouteInfo;
  friend class MediaRouterUserRouteInfo;
  friend class MediaRouterRouteGroup;
  friend class CMediaRouterHelper;

public:
  class CallbackInfo;

  /*static*/
  class Static
    : public Object
    , public IDisplayListener
    , public IMediaRouterStatic
  {
  friend class CMediaRouter;

  public:
      class MyAudioRoutesObserver
          : public Object
          , public IIAudioRoutesObserver
          , public IBinder
      {
      public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
          /* [in] */ IMediaRouterStatic* owner);

        CARAPI DispatchAudioRoutesChanged(
          /* [in] */ IAudioRoutesInfo* newRoutes);

        CARAPI ToString(
            /* [out] */ String* str);

      private:
        Static* mOwner;
      };

      class MyStaticClient
          : public Object
          , public IIMediaRouterClient
          , public IBinder
      {
      public:
        CAR_INTERFACE_DECL()

        CARAPI constructor(
            /* [in] */ IMediaRouterStatic* owner);

        CARAPI OnStateChanged();

        CARAPI ToString(
            /* [out] */ String* str);

      private:
        Static* mOwner;
      };

  private:
      class MyRunnable
          : public Object
          , public IRunnable
      {
      public:
          CAR_INTERFACE_DECL()

          MyRunnable(
              /* [in] */ IAudioRoutesInfo* newRoutes,
              /* [in] */ Static* owner);

          CARAPI Run();

      private:
          Static* mOwner;
          AutoPtr<IAudioRoutesInfo> mNewRoutes;
      };

  public:
    CAR_INTERFACE_DECL()

    ~Static();

    Static(
      /* [in] */ IContext* appContext,
      /* [in] */ IMediaRouter* owner);

    //@Override
    CARAPI OnDisplayAdded(
      /* [in] */ Int32 displayId);

    //@Override
    CARAPI OnDisplayChanged(
      /* [in] */ Int32 displayId);

    //@Override
    CARAPI OnDisplayRemoved(
      /* [in] */ Int32 displayId);

    CARAPI GetAllPresentationDisplays(
      /* [out, callee] */ ArrayOf<IDisplay*>** result);

    // Called after sStatic is initialized
    CARAPI StartMonitoringRoutes(
      /* [in] */ IContext* appContext);

    CARAPI UpdateAudioRoutes(
      /* [in] */ IAudioRoutesInfo* newRoutes);

    CARAPI_(void) UpdatePresentationDisplays(
      /* [in] */ Int32 changedDisplayId);

    CARAPI IsBluetoothA2dpOn(
      /* [out] */ Boolean* result);

    CARAPI UpdateDiscoveryRequest();

    CARAPI SetSelectedRoute(
      /* [in] */ IMediaRouterRouteInfo* info,
      /* [in] */ Boolean bexplicit);

    CARAPI RebindAsUser(
      /* [in] */ Int32 userId);

    CARAPI PublishClientDiscoveryRequest();

    CARAPI PublishClientSelectedRoute(
      /* [in] */ Boolean bexplicit);

    CARAPI UpdateClientState();

    CARAPI RequestSetVolume(
      /* [in] */ IMediaRouterRouteInfo* route,
      /* [in] */ Int32 volume);

    CARAPI RequestUpdateVolume(
      /* [in] */ IMediaRouterRouteInfo* route,
      /* [in] */ Int32 direction);

    CARAPI MakeGlobalRoute(
      /* [in] */ IMediaRouterClientStateRouteInfo* globalRoute_,
      /* [out] */ IMediaRouterRouteInfo** result);

    CARAPI UpdateGlobalRoute(
      /* [in] */ IMediaRouterRouteInfo* route,
      /* [in] */ IMediaRouterClientStateRouteInfo* globalRoute);

    CARAPI FindGlobalRoute(
      /* [in] */ const String& globalRouteId,
      /* [out] */ IMediaRouterRouteInfo** result);

    public:
        AutoPtr<IContext> mAppContext;
        AutoPtr<IResources> mResources;
        AutoPtr<IIAudioService> mAudioService;
        AutoPtr<IDisplayManager> mDisplayService;
        AutoPtr<IIMediaRouterService> mMediaRouterService;
        static AutoPtr<IHandler> mHandler;
        List<AutoPtr<CallbackInfo> > mCallbacks; // = new CopyOnWriteArrayList<CallbackInfo>();

        List<AutoPtr<IMediaRouterRouteInfo> > mRoutes; // = new ArrayList<RouteInfo>();
        List<AutoPtr<IMediaRouterRouteCategory> > mCategories; // = new ArrayList<RouteCategory>();

        AutoPtr<IMediaRouterRouteCategory> mSystemCategory;

        AutoPtr<IAudioRoutesInfo> mCurAudioRoutesInfo; // = new AudioRoutesInfo();

        AutoPtr<IMediaRouterRouteInfo> mDefaultAudioVideo;
        AutoPtr<IMediaRouterRouteInfo> mBluetoothA2dpRoute;

        AutoPtr<IMediaRouterRouteInfo> mSelectedRoute;

        Boolean mCanConfigureWifiDisplays;
        Boolean mActivelyScanningWifiDisplays;
        String mPreviousActiveWifiDisplayAddress;

        Int32 mDiscoveryRequestRouteTypes;
        Boolean mDiscoverRequestActiveScan;

        Int32 mCurrentUserId;
        static AutoPtr<IIMediaRouterClient> mClient;
        AutoPtr<IMediaRouterClientState> mClientState;

        AutoPtr<IIAudioRoutesObserver> mAudioRoutesObserver;

    private:
        static CMediaRouter* mOwner;
    };

    /*static*/
    class RouteCategory
        : public Object
        , public IMediaRouterRouteCategory
    {
    public:
        RouteCategory();

        virtual ~RouteCategory();

        CAR_INTERFACE_DECL()

        CARAPI constructor(
           /* [in] */ ICharSequence* name,
           /* [in] */ Int32 types,
           /* [in] */ Boolean groupable,
           /* [in] */ IMediaRouter* host);

        CARAPI constructor(
           /* [in] */ Int32 nameResId,
           /* [in] */ Int32 types,
           /* [in] */ Boolean groupable,
           /* [in] */ IMediaRouter* host);

        CARAPI GetName(
            /* [out] */ ICharSequence** result);

        CARAPI GetName(
            /* [in] */ IContext* context,
            /* [out] */ ICharSequence** result);

        CARAPI GetName(
            /* [in] */ IResources* res,
            /* [out] */ ICharSequence** result);

        CARAPI GetRoutes(
            /* [in] */ IList* outList,
            /* [out] */ IList** result);

        CARAPI GetSupportedTypes(
            /* [out] */ Int32 * result);

        CARAPI IsGroupable(
            /* [out] */ Boolean * result);

        CARAPI IsSystem(
            /* [out] */ Boolean * result);

        CARAPI ToString(
            /* [out] */ String* result);

    public:
        AutoPtr<ICharSequence> mName;
        Int32 mNameResId;
        Int32 mTypes;
        Boolean mGroupable;
        Boolean mIsSystem;

    private:
        CMediaRouter* mHost;
    };

   /*static*/
    class CallbackInfo
        : public Object
        , public IMediaRouterRouteCallbackInfo
    {
        friend class CMediaRouter;
    public:
        CallbackInfo(
            /* [in] */ IMediaRouterCallback* cb,
            /* [in] */ Int32 type,
            /* [in] */ Int32 flags,
            /* [in] */ IMediaRouter* router);

        ~CallbackInfo();

        CAR_INTERFACE_DECL()

        CARAPI FilterRouteEvent(
           /* [in] */ IMediaRouterRouteInfo* route,
           /* [out] */ Boolean* result);

        CARAPI FilterRouteEvent(
           /* [in] */ Int32 supportedTypes,
           /* [out] */ Boolean* result);

    public:
        Int32 mType;
        Int32 mFlags;
        AutoPtr<IMediaRouterCallback> mCb;
        CMediaRouter* mRouter;
    };

   class VolumeCallbackInfo
       : public Object
       , public IMediaRouterVolumeCallbackInfo
   {
       friend class CMediaRouter;
   public:
       CAR_INTERFACE_DECL()

       VolumeCallbackInfo();

       virtual ~VolumeCallbackInfo();

       CARAPI constructor(
           /* [in] */ IMediaRouterVolumeCallback* vcb,
           /* [in] */ IMediaRouterRouteInfo* route);

       CARAPI SetVolumeCallback(
           /* [in] */ IMediaRouterVolumeCallback* vcb);

       CARAPI GetVolumeCallback(
           /* [out] */ IMediaRouterVolumeCallback** vcb);

       CARAPI SetRouteInfo(
           /* [in] */ IMediaRouterRouteInfo* route);

       CARAPI GetRouteInfo(
           /* [out] */ IMediaRouterRouteInfo** route);

   public:
       AutoPtr<IMediaRouterVolumeCallback> mVcb;

       AutoPtr<IMediaRouterRouteInfo> mRoute;
   };

    /*static*/
    class VolumeChangeReceiver
        : public BroadcastReceiver
    {
      public:
        CARAPI OnReceive(
          /* [in] */ IContext* context,
          /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CMediaRouter::VolumeChangeReceiver: ")

      private:
        CMediaRouter* mHost;
    };

    /*static*/
    class WifiDisplayStatusChangedReceiver
        : public BroadcastReceiver
    {
      public:
        CARAPI OnReceive(
          /* [in] */ IContext* context,
          /* [in] */ IIntent* intent);

        TO_STRING_IMPL("CMediaRouter::WifiDisplayStatusChangedReceiver: ")
    };

public:
  CMediaRouter();

  virtual ~CMediaRouter();

  CAR_INTERFACE_DECL()

  CAR_OBJECT_DECL()

  CARAPI constructor(
    /* [in] */ IContext* context);

  /**
   * @hide for use by framework routing UI
   */
  CARAPI GetSystemAudioRoute(
    /* [out] */ IMediaRouterRouteInfo** result);


  /**
  * @hide for use by framework routing UI
  */
  CARAPI GetSystemAudioCategory(
    /* [out] */ IMediaRouterRouteCategory** result);

  /**
  * Return the currently selected route for the given types
  *
  * @param type route types
  * @return the selected route
  */
  CARAPI GetSelectedRoute(
    /* [in] */ Int32 type,
    /* [out] */ IMediaRouterRouteInfo** result);

 /**
  * Add a callback to listen to events about specific kinds of media routes.
  * If the specified callback is already registered, its registration will be updated for any
  * additional route types specified.
  *
  * @param types Types of routes this callback is interested in
  * @param cb Callback to add
  */
  CARAPI AddCallback(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterCallback* cb);

  CARAPI AddCallback(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterCallback* cb,
    /* [in] */ Int32 flags);

  /**
  * Remove the specified callback. It will no longer receive events about media routing.
  *
  * @param cb Callback to remove
  */
  CARAPI RemoveCallback(
    /* [in] */ IMediaRouterCallback* cb);

  /**
  * Select the specified route to use for output of the given media types.
  *
  * @param types type flags indicating which types this route should be used for.
  *              The route must support at least a subset.
  * @param route Route to select
  */
  CARAPI SelectRoute(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route);

  /**
  * @hide internal use
  */
  CARAPI SelectRouteInt(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean isExplicit);

  /**
  * Add an app-specified route for media to the MediaRouter.
  * App-specified route definitions are created using {@link #createUserRoute(RouteCategory)}
  *
  * @param info Definition of the route to add
  * @see #createUserRoute()
  * @see #removeUserRoute(UserRouteInfo)
  */
  CARAPI AddUserRoute(
    /* [in] */ IMediaRouterUserRouteInfo* info);

  /**
  * @hide Framework use only
  */
  CARAPI AddRouteInt(
    /* [in] */ IMediaRouterRouteInfo* info);

  /**
  * Remove an app-specified route for media from the MediaRouter.
  *
  * @param info Definition of the route to remove
  * @see #addUserRoute(UserRouteInfo)
  */
  CARAPI RemoveUserRoute(
    /* [in] */ IMediaRouterUserRouteInfo* info);

  /**
  * Remove all app-specified routes from the MediaRouter.
  *
  * @see #removeUserRoute(UserRouteInfo)
  */
  CARAPI ClearUserRoutes();

  /**
  * @hide internal use only
  */
  CARAPI RemoveRouteInt32(
    /* [in] */ IMediaRouterRouteInfo* info);

  /**
  * Return the number of {@link MediaRouter.RouteCategory categories} currently
  * represented by routes known to this MediaRouter.
  *
  * @return the number of unique categories represented by this MediaRouter's known routes
  */
  CARAPI GetCategoryCount(
    /* [out] */ Int32* result);

  /**
  * Return the {@link MediaRouter.RouteCategory category} at the given index.
  * Valid indices are in the range [0-getCategoryCount).
  *
  * @param index which category to return
  * @return the category at index
  */
  CARAPI GetCategoryAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteCategory** result);

  /**
  * Return the number of {@link MediaRouter.RouteInfo routes} currently known
  * to this MediaRouter.
  *
  * @return the number of routes tracked by this router
  */
  CARAPI GetRouteCount(
    /* [out] */ Int32* result);

  /**
  * Return the route at the specified index.
  *
  * @param index index of the route to return
  * @return the route at index
  */
  CARAPI GetRouteAt(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo** result);

  /**
  * Create a new user route that may be modified and registered for use by the application.
  *
  * @param category The category the new route will belong to
  * @return A new UserRouteInfo for use by the application
  *
  * @see #addUserRoute(UserRouteInfo)
  * @see #removeUserRoute(UserRouteInfo)
  * @see #createRouteCategory(CharSequence)
  */
  CARAPI CreateUserRoute(
    /* [in] */ IMediaRouterRouteCategory* category,
    /* [out] */ IMediaRouterUserRouteInfo** result);

  /**
  * Create a new route category. Each route must belong to a category.
  *
  * @param name Name of the new category
  * @param isGroupable true if routes in this category may be grouped with one another
  * @return the new RouteCategory
  */
  CARAPI CreateRouteCategory(
    /* [in] */ ICharSequence* name,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IMediaRouterRouteCategory** result);

  /**
  * Create a new route category. Each route must belong to a category.
  *
  * @param nameResId Resource ID of the name of the new category
  * @param isGroupable true if routes in this category may be grouped with one another
  * @return the new RouteCategory
  */
  CARAPI CreateRouteCategory(
    /* [in] */ Int32 nameResId,
    /* [in] */ Boolean isGroupable,
    /* [out] */ IMediaRouterRouteCategory** result);

  CARAPI GetDefaultRoute(
    /* [out] */ IMediaRouterRouteInfo ** result);

  CARAPI GetSystemCategory(
    /* [out] */ IMediaRouterRouteCategory ** result);

  CARAPI GetSelectedRoute(
    /* [out] */ IMediaRouterRouteInfo ** result);

  CARAPI IsRouteAvailable(
    /* [in] */ Int32 types,
    /* [in] */ Int32 flags,
    /* [out] */ Boolean * result);

  CARAPI RebindAsUser(
    /* [in] */ Int32 userId);

public:
  static CARAPI UpdateRoute(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI GetRouteCountStatic(
    /* [out] */ Int32* result);

  static CARAPI GetRouteAtStatic(
    /* [in] */ Int32 index,
    /* [out] */ IMediaRouterRouteInfo** result);

  static CARAPI DispatchRouteVolumeChanged(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteGrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group,
    /* [in] */ Int32 index);

  static CARAPI DispatchRouteUngrouped(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ IMediaRouterRouteGroup* group);

  static CARAPI RemoveRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI_(String) TypesToString(
    /* [in] */ Int32 types);

  static CARAPI SelectRouteStatic(
    /* [in] */ Int32 types,
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ Boolean isexplicit);

  static CARAPI SelectDefaultRouteStatic();

  /**
  * Compare the device address of a display and a route.
  * Nulls/no device address will match another null/no address.
  */
  static CARAPI MatchesDeviceAddress(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [out] */ Boolean* result);

  static CARAPI AddRouteStatic(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteSelected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteUnselected(
    /* [in] */ Int32 type,
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteChanged(
    /* [in] */ IMediaRouterRouteInfo* info,
    /* [in] */ Int32 oldSupportedTypes);

  static CARAPI DispatchRouteAdded(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRouteRemoved(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI DispatchRoutePresentationDisplayChanged(
    /* [in] */ IMediaRouterRouteInfo* info);

  static CARAPI SystemVolumeChanged(
    /* [in] */ Int32 newValue);

  static CARAPI UpdateWifiDisplayStatus(
    /* [in] */ IWifiDisplayStatus* status);

  static CARAPI GetWifiDisplayStatusCode(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Int32* result);

  static CARAPI IsWifiDisplayEnabled(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ Boolean* result);

  static CARAPI MakeWifiDisplayRoute(
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [out] */ IMediaRouterRouteInfo** result);

private:
  static CARAPI_(void) UpdateWifiDisplayRoute(
    /* [in] */ IMediaRouterRouteInfo* route,
    /* [in] */ IWifiDisplay* display,
    /* [in] */ IWifiDisplayStatus* wfdStatus,
    /* [in] */ Boolean disconnected);

  static CARAPI_(AutoPtr<IWifiDisplay>) FindWifiDisplay(
    /* [in] */ ArrayOf<IWifiDisplay*>* displays,
    /* [in] */ const String& deviceAddress);

  static CARAPI_(AutoPtr<IMediaRouterRouteInfo>) FindWifiDisplayRoute(
    /* [in] */ IWifiDisplay* d);

  CARAPI_(Int32) FindCallbackInfo(
    /* [in] */ IMediaRouterCallback* cb);

  static CARAPI_(Boolean) ShouldShowWifiDisplay(
    /* [in] */ IWifiDisplay* d,
    /* [in] */ IWifiDisplay* activeDisplay);

private:
   static const String TAG;
   static const Boolean DEBUG;

public:
   static AutoPtr<IMediaRouterStatic> sStatic;
   static const Int32 ROUTE_TYPE_ANY;

private:
   static HashMap< AutoPtr<IContext>, AutoPtr<IMediaRouter> > sRouters; // = new HashMap<Context, MediaRouter>();//

   static Object mStaticClass;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__
