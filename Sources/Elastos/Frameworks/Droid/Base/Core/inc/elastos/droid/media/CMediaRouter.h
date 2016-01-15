
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__

#include "_Elastos_Droid_Media_CMediaRouter.h"
#include "elastos/droid/ext/frameworkext.h"
#include <elastos/utility/etl/List.h>
#include <elastos/utility/etl/HashMap.h>
#include "elastos/droid/content/BroadcastReceiver.h"

using Elastos::Droid::Hardware::Display::IDisplayListener;
using Elastos::Droid::Hardware::Display::IDisplayManager;
using Elastos::Droid::View::IDisplay;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::Res::IResources;
using Elastos::Droid::Os::IHandler;
using Elastos::Utility::Etl::List;
using Elastos::Droid::Hardware::Display::IWifiDisplayStatus;
using Elastos::Droid::Content::IIntent;
using Elastos::Core::ICharSequence;
using Elastos::Droid::Hardware::Display::IWifiDisplay;
using Elastos::Utility::Etl::HashMap;
using Elastos::Core::IRunnable;
using Elastos::Droid::Content::BroadcastReceiver;

namespace Elastos {
namespace Droid {
namespace Media {

CarClass(CMediaRouter)
{
public:
    /*static*/
    class CallbackInfo : public ElRefBase
    {
    public:
        CallbackInfo(
            /* [in] */ ICallback* cb,
            /* [in] */ Int32 type,
            /* [in] */ IMediaRouter* router);

    public:
        AutoPtr<ICallback> mCb;

        Int32 mType;

        AutoPtr<IMediaRouter> mRouter;
    };

    /*static*/
    class Static // implements DisplayManager.DisplayListener
        : public ElRefBase
        , public IDisplayListener
    {
    friend class CMediaRouter;

    private:
        class MyAudioRoutesObserver
            : public ElRefBase
            , public IIAudioRoutesObserver
        {
        public:
            CAR_INTERFACE_DECL();

            MyAudioRoutesObserver(
                /* [in] */ Static* owner);

            CARAPI DispatchAudioRoutesChanged(
                /* [in] */ IAudioRoutesInfo* newRoutes);

        private:
            Static* mOwner;
        };

        class MyRunnable
            : public ElRefBase
            , public IRunnable
        {
        public:
            CAR_INTERFACE_DECL();

            MyRunnable(
                /* [in] */ IAudioRoutesInfo* newRoutes,
                /* [in] */ Static* owner);

            CARAPI Run();

        private:
            AutoPtr<IAudioRoutesInfo> mNewRoutes;

            Static* mOwner;
        };

    public:
        CAR_INTERFACE_DECL();

        Static(
            /* [in] */ IContext* appContext,
            /* [in] */ CMediaRouter* owner);

        //@Override
        CARAPI OnDisplayAdded(
            /* [in] */ Int32 displayId);

        //@Override
        CARAPI OnDisplayChanged(
            /* [in] */ Int32 displayId);

        //@Override
        CARAPI OnDisplayRemoved(
            /* [in] */ Int32 displayId);

        CARAPI_(AutoPtr< ArrayOf<IDisplay*> >) GetAllPresentationDisplays();

    private:
        // Called after sStatic is initialized
        CARAPI_(void) StartMonitoringRoutes(
            /* [in] */ IContext* appContext);

        CARAPI_(void) UpdateAudioRoutes(
            /* [in] */ IAudioRoutesInfo* newRoutes);

        CARAPI_(void) UpdatePresentationDisplays(
            /* [in] */ Int32 changedDisplayId);

    public:
        AutoPtr<IResources> mResources;

        AutoPtr<IIAudioService> mAudioService;

        AutoPtr<IHandler> mHandler;

    private:
        AutoPtr<IDisplayManager> mDisplayService;

        List< AutoPtr<CallbackInfo> > mCallbacks; // = new CopyOnWriteArrayList<CallbackInfo>();
        List< AutoPtr<IRouteInfo> > mRoutes; // = new ArrayList<RouteInfo>();
        List< AutoPtr<IRouteCategory> > mCategories; // = new ArrayList<RouteCategory>();

        AutoPtr<IRouteCategory> mSystemCategory;
        AutoPtr<IAudioRoutesInfo> mCurAudioRoutesInfo; // = new AudioRoutesInfo();
        AutoPtr<IRouteInfo> mDefaultAudioVideo;
        AutoPtr<IRouteInfo> mBluetoothA2dpRoute;
        AutoPtr<IRouteInfo> mSelectedRoute;
        AutoPtr<IWifiDisplayStatus> mLastKnownWifiDisplayStatus;
        AutoPtr<IIAudioRoutesObserver> mAudioRoutesObserver; // = new MyAudioRoutesObserver();

        CMediaRouter* mOwner;
    };

    /**
     * Stub implementation of {@link MediaRouter.Callback}.
     * Each abstract method is defined as a no-op. Override just the ones
     * you need.
     */
    /*static*/
    class SimpleCallback
        : public ElRefBase
        , public ICallback
    {
    public:
        CARAPI OnRouteSelected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteUnselected(
            /* [in] */ IMediaRouter* router,
            /* [in] */ Int32 type,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteAdded(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteRemoved(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info);

        CARAPI OnRouteGrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info,
            /* [in] */ IRouteGroup* group,
            /* [in] */ Int32 index);

        CARAPI OnRouteUngrouped(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info,
            /* [in] */ IRouteGroup* group);

        CARAPI OnRouteVolumeChanged(
            /* [in] */ IMediaRouter* router,
            /* [in] */ IRouteInfo* info);
    };

    class VolumeCallbackInfo : public ElRefBase
    {
    public:
        VolumeCallbackInfo(
            /* [in] */ IVolumeCallback* vcb,
            /* [in] */ IRouteInfo* route);

    public:
        AutoPtr<IVolumeCallback> mVcb;

        AutoPtr<IRouteInfo> mRoute;
    };

    /*static*/ class VolumeChangeReceiver : public BroadcastReceiver
    {
    public:
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CMediaRouter::VolumeChangeReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    };

    /*static*/ class WifiDisplayStatusChangedReceiver : public BroadcastReceiver
    {
    public:
        CARAPI OnReceive(
            /* [in] */ IContext* context,
            /* [in] */ IIntent* intent);

        CARAPI ToString(
            /* [out] */ String* info)
        {
            VALIDATE_NOT_NULL(info);
            *info = String("CMediaRouter::WifiDisplayStatusChangedReceiver: ");
            (*info).AppendFormat("%p", this);
            return NOERROR;
        }
    };

    CMediaRouter();

    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @hide for use by framework routing UI
     */
    CARAPI GetSystemAudioRoute(
        /* [out] */ IRouteInfo** result);

    /**
     * @hide for use by framework routing UI
     */
    CARAPI GetSystemAudioCategory(
        /* [out] */ IRouteCategory** result);

    /**
     * Return the currently selected route for the given types
     *
     * @param type route types
     * @return the selected route
     */
    CARAPI GetSelectedRoute(
        /* [in] */ Int32 type,
        /* [out] */ IRouteInfo** result);

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
        /* [in] */ ICallback* cb);

    /**
     * Remove the specified callback. It will no longer receive events about media routing.
     *
     * @param cb Callback to remove
     */
    CARAPI RemoveCallback(
        /* [in] */ ICallback* cb);

    /**
     * Select the specified route to use for output of the given media types.
     *
     * @param types type flags indicating which types this route should be used for.
     *              The route must support at least a subset.
     * @param route Route to select
     */
    CARAPI SelectRoute(
        /* [in] */ Int32 types,
        /* [in] */ IRouteInfo* route);

    /**
     * @hide internal use
     */
    CARAPI SelectRouteInt(
        /* [in] */ Int32 types,
        /* [in] */ IRouteInfo* route);

    /**
     * Add an app-specified route for media to the MediaRouter.
     * App-specified route definitions are created using {@link #createUserRoute(RouteCategory)}
     *
     * @param info Definition of the route to add
     * @see #createUserRoute()
     * @see #removeUserRoute(UserRouteInfo)
     */
    CARAPI AddUserRoute(
        /* [in] */ IUserRouteInfo* info);

    /**
     * @hide Framework use only
     */
    CARAPI AddRouteInt(
        /* [in] */ IRouteInfo* info);

    /**
     * Remove an app-specified route for media from the MediaRouter.
     *
     * @param info Definition of the route to remove
     * @see #addUserRoute(UserRouteInfo)
     */
    CARAPI RemoveUserRoute(
        /* [in] */ IUserRouteInfo* info);

    /**
     * Remove all app-specified routes from the MediaRouter.
     *
     * @see #removeUserRoute(UserRouteInfo)
     */
    CARAPI ClearUserRoutes() ;

    /**
     * @hide internal use only
     */
    CARAPI RemoveRouteInt32(
        /* [in] */ IRouteInfo* info);

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
        /* [out] */ IRouteCategory** result);

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
        /* [out] */ IRouteInfo** result);

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
        /* [in] */ IRouteCategory* category,
        /* [out] */ IUserRouteInfo** result);

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
        /* [out] */ IRouteCategory** result);

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
        /* [out] */ IRouteCategory** result);

    static CARAPI_(String) TypesToString(
        /* [in] */ Int32 types);

    static CARAPI_(void) UpdateRoute(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(Int32) GetRouteCountStatic();

    static CARAPI_(AutoPtr<IRouteInfo>) GetRouteAtStatic(
        /* [in] */ Int32 index);

    static CARAPI_(void) DispatchRouteVolumeChanged(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRouteGrouped(
        /* [in] */ IRouteInfo* info,
        /* [in] */ IRouteGroup* group,
        /* [in] */ Int32 index);

    static CARAPI_(void) DispatchRouteUngrouped(
        /* [in] */ IRouteInfo* info,
        /* [in] */ IRouteGroup* group);

    static CARAPI_(void) RemoveRoute(
        /* [in] */ IRouteInfo* info);

private:
    static CARAPI_(void) SelectRouteStatic(
        /* [in] */ Int32 types,
        /* [in] */ IRouteInfo* route);

    /**
     * Compare the device address of a display and a route.
     * Nulls/no device address will match another null/no address.
     */
    static CARAPI_(Boolean) MatchesDeviceAddress(
        /* [in] */ IWifiDisplay* display,
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) AddRouteStatic(
        /* [in] */ IRouteInfo* info);

    CARAPI_(void) RemoveRouteAt(
        /* [in] */ Int32 routeIndex);

    static CARAPI_(void) DispatchRouteSelected(
        /* [in] */ Int32 type,
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRouteUnselected(
        /* [in] */ Int32 type,
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRouteChanged(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRouteAdded(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRouteRemoved(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) DispatchRoutePresentationDisplayChanged(
        /* [in] */ IRouteInfo* info);

    static CARAPI_(void) SystemVolumeChanged(
        /* [in] */ Int32 newValue);

    static CARAPI_(void) UpdateWifiDisplayStatus(
        /* [in] */ IWifiDisplayStatus* newStatus);

    static CARAPI_(AutoPtr<IRouteInfo>) MakeWifiDisplayRoute(
        /* [in] */ IWifiDisplay* display,
        /* [in] */ Boolean available);

    static CARAPI_(void) UpdateWifiDisplayRoute(
        /* [in] */ IRouteInfo* route,
        /* [in] */ IWifiDisplay* display,
        /* [in] */ Boolean available,
        /* [in] */ IWifiDisplayStatus* wifiDisplayStatus);

    static CARAPI_(AutoPtr<IWifiDisplay>) FindMatchingDisplay(
        /* [in] */ IWifiDisplay* d,
        /* [in] */ ArrayOf<IWifiDisplay*>* displays);

    static CARAPI_(AutoPtr<IRouteInfo>) FindWifiDisplayRoute(
        /* [in] */ IWifiDisplay* d);

    static CARAPI_(AutoPtr<IDisplay>) ChoosePresentationDisplayForRoute(
        /* [in] */ IRouteInfo* route,
        /* [in] */ ArrayOf<IDisplay*>* displays);

public:
    static AutoPtr<Static> sStatic;

    static const String TAG; // = "MediaRouter";

private:
    static HashMap< AutoPtr<IContext>, AutoPtr<IMediaRouter> > sRouters; // = new HashMap<Context, MediaRouter>();

    static Object mStaticClass;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_MEDIA_CMEDIAROUTER_H__
