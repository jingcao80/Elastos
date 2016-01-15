#ifndef __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGER_H__
#define __ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGER_H__

#include "Elastos.Droid.View.h"
#include "elastos/droid/database/ContentObserver.h"
#include "elastos/droid/os/Runnable.h"
#include <elastos/core/Object.h>

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Database::ContentObserver;
using Elastos::Droid::Database::IContentObserver;
using Elastos::Droid::Graphics::ITypeface;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Core::IRunnable;
using Elastos::Utility::ILocale;
using Elastos::Utility::IArrayList;

namespace Elastos {
namespace Droid {
namespace View {
namespace Accessibility {

class CaptioningManager
    : public Object
    , public ICaptioningManager
{
public:
    class CaptionStyle
        : public Object
        , public ICaptioningManagerCaptionStyle
    {
    public:
        CAR_INTERFACE_DECL();

        CaptionStyle();

        ~CaptionStyle();

        CARAPI constructor(
            /* [in] */ Int32 foregroundColor,
            /* [in] */ Int32 backgroundColor,
            /* [in] */ Int32 edgeType,
            /* [in] */ Int32 edgeColor,
            /* [in] */ Int32 windowColor,
            /* [in] */ const String& rawTypeface);

        CARAPI GetForegroundColor(
            /* [out] */ Int32* color);

        CARAPI GetBackgroundColor(
            /* [out] */ Int32* color);

        CARAPI GetEdgeType(
            /* [out] */ Int32* type);

        CARAPI GetEdgeColor(
            /* [out] */ Int32* color);

        CARAPI GetWindowColor(
            /* [out] */ Int32* color);

        CARAPI GetRawTypeface(
            /* [out] */ String* str);

        /**
         * Applies a caption style, overriding any properties that are specified
         * in the overlay caption.
         *
         * @param overlay The style to apply
         * @return A caption style with the overlay style applied
         * @hide
         */
        CARAPI ApplyStyle(
            /* [in] */ ICaptioningManagerCaptionStyle* overlay,
            /* [out] */ ICaptioningManagerCaptionStyle** style);

        /**
         * @return {@code true} if the user has specified a background color
         *         that should override the application default, {@code false}
         *         otherwise
         */
        CARAPI HasBackgroundColor(
            /* [out] */ Boolean* result);

        /**
         * @return {@code true} if the user has specified a foreground color
         *         that should override the application default, {@code false}
         *         otherwise
         */
        CARAPI HasForegroundColor(
            /* [out] */ Boolean* result);

        /**
         * @return {@code true} if the user has specified an edge type that
         *         should override the application default, {@code false}
         *         otherwise
         */
        CARAPI HasEdgeType(
            /* [out] */ Boolean* result);

        /**
         * @return {@code true} if the user has specified an edge color that
         *         should override the application default, {@code false}
         *         otherwise
         */
        CARAPI HasEdgeColor(
            /* [out] */ Boolean* result);

        /**
         * @return {@code true} if the user has specified a window color that
         *         should override the application default, {@code false}
         *         otherwise
         */
        CARAPI HasWindowColor(
            /* [out] */ Boolean* result);

        /**
         * @return the preferred {@link Typeface} for video captions, or null if
         *         not specified
         */
        CARAPI GetTypeface(
            /* [out] */ ITypeface** face);

        static CARAPI GetCustomStyle(
            /* [in] */ IContentResolver* cr,
            /* [out] */ ICaptioningManagerCaptionStyle** style);

    private:
        static CARAPI_(AutoPtr<ICaptioningManagerCaptionStyle>) InitSTYLE(
            /* [in] */ Int32 value1,
            /* [in] */ Int32 value2,
            /* [in] */ Int32 value3,
            /* [in] */ Int32 value4,
            /* [in] */ Int32 value5,
            /* [in] */ const String& value);

        static CARAPI_(AutoPtr< ArrayOf<ICaptioningManagerCaptionStyle*> >) InitPRESETS();

    public:
        /** The default caption style used to fill in unspecified values. @hide */
        static const AutoPtr<ICaptioningManagerCaptionStyle> DEFAULT;

        /** @hide */
        static const AutoPtr< ArrayOf<ICaptioningManagerCaptionStyle*> > PRESETS;

        /** The preferred foreground color for video captions. */
        Int32 mForegroundColor;

        /** The preferred background color for video captions. */
        Int32 mBackgroundColor;

        /**
         * The preferred edge type for video captions, one of:
         * <ul>
         * <li>{@link #EDGE_TYPE_UNSPECIFIED}
         * <li>{@link #EDGE_TYPE_NONE}
         * <li>{@link #EDGE_TYPE_OUTLINE}
         * <li>{@link #EDGE_TYPE_DROP_SHADOW}
         * <li>{@link #EDGE_TYPE_RAISED}
         * <li>{@link #EDGE_TYPE_DEPRESSED}
         * </ul>
         */
        Int32 mEdgeType;

        /**
         * The preferred edge color for video captions, if using an edge type
         * other than {@link #EDGE_TYPE_NONE}.
         */
        Int32 mEdgeColor;

        /** The preferred window color for video captions. */
        Int32 mWindowColor;

        /**
         * @hide
         */
        String mRawTypeface;

    private:
        /** Packed value for a color of 'none' and a cached opacity of 100%. */
        static const Int32 COLOR_NONE_OPAQUE;

        /** Packed value for an unspecified color and opacity. */
        static const Int32 COLOR_UNSPECIFIED;

        static const AutoPtr<ICaptioningManagerCaptionStyle> WHITE_ON_BLACK;
        static const AutoPtr<ICaptioningManagerCaptionStyle> BLACK_ON_WHITE;
        static const AutoPtr<ICaptioningManagerCaptionStyle> YELLOW_ON_BLACK;
        static const AutoPtr<ICaptioningManagerCaptionStyle> YELLOW_ON_BLUE;
        static const AutoPtr<ICaptioningManagerCaptionStyle> DEFAULT_CUSTOM;
        static const AutoPtr<ICaptioningManagerCaptionStyle> UNSPECIFIED;
        Boolean mHasForegroundColor;
        Boolean mHasBackgroundColor;
        Boolean mHasEdgeType;
        Boolean mHasEdgeColor;
        Boolean mHasWindowColor;

        /** Lazily-created typeface based on the raw typeface string. */
        AutoPtr<ITypeface> mParsedTypeface;
    };

    class CaptioningChangeListener
        : public Object
        , public ICaptioningManagerCaptioningChangeListener
    {
    public:
        CAR_INTERFACE_DECL();

        /**
         * Called when the captioning enabled state changes.
         *
         * @param enabled the user's new preferred captioning enabled state
         */
        CARAPI OnEnabledChanged(
            /* [in] */ Boolean enabled);

        /**
         * Called when the captioning user style changes.
         *
         * @param userStyle the user's new preferred style
         * @see CaptioningManager#getUserStyle()
         */
        CARAPI OnUserStyleChanged(
            /* [in] */ ICaptioningManagerCaptionStyle* userStyle);

        /**
         * Called when the captioning locale changes.
         *
         * @param locale the preferred captioning locale, or {@code null} if not specified
         * @see CaptioningManager#getLocale()
         */
        CARAPI OnLocaleChanged(
            /* [in] */ ILocale* locale);

        /**
         * Called when the captioning font scaling factor changes.
         *
         * @param fontScale the preferred font scaling factor
         * @see CaptioningManager#getFontScale()
         */
        CARAPI OnFontScaleChanged(
            /* [in] */ Float fontScale);
    };

private:
    class MyContentObserver
        : public ContentObserver
    {
    public:
        MyContentObserver(
            /* [in ] */ CaptioningManager* host);

        ~MyContentObserver();

        CARAPI OnChange(
            /* [in] */ Boolean selfChange,
            /* [in] */ IUri* uri);

    private:
        CaptioningManager* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in ] */ CaptioningManager* host);

        ~MyRunnable();

        CARAPI Run();

    private:
        CaptioningManager* mHost;
    };

public:
    CAR_INTERFACE_DECL();

    CaptioningManager();

    ~CaptioningManager();

    /**
     * Creates a new captioning manager for the specified context.
     *
     * @hide
     */
    CARAPI constructor(
        /* [in] */ IContext* context);

    /**
     * @return the user's preferred captioning enabled state
     */
    CARAPI IsEnabled(
        /* [out] */ Boolean* result);

    /**
     * @return the raw locale string for the user's preferred captioning
     *         language
     * @hide
     */
    CARAPI GetRawLocale(
        /* [out] */ String* locale);

    /**
     * @return the locale for the user's preferred captioning language, or null
     *         if not specified
     */
    CARAPI GetLocale(
        /* [out] */ ILocale** locale);

    /**
     * @return the user's preferred font scaling factor for video captions, or 1 if not
     *         specified
     */
    CARAPI GetFontScale(
        /* [out] */ Float* scale);

    /**
     * @return the raw preset number, or the first preset if not specified
     * @hide
     */
    CARAPI GetRawUserStyle(
        /* [out] */ Int32* style);

    /**
     * @return the user's preferred visual properties for captions as a
     *         {@link CaptionStyle}, or the default style if not specified
     */
    CARAPI GetUserStyle(
        /* [out] */ ICaptioningManagerCaptionStyle** style);

    /**
     * Adds a listener for changes in the user's preferred captioning enabled
     * state and visual properties.
     *
     * @param listener the listener to add
     */
    CARAPI AddCaptioningChangeListener(
        /* [in] */ ICaptioningManagerCaptioningChangeListener* listener);

    /**
     * Removes a listener previously added using
     * {@link #addCaptioningChangeListener}.
     *
     * @param listener the listener to remove
     */
    CARAPI RemoveCaptioningChangeListener(
        /* [in] */ ICaptioningManagerCaptioningChangeListener* listener);

private:
    CARAPI_(void) RegisterObserver(
        /* [in] */ const String& key);

    CARAPI_(void) NotifyEnabledChanged();

    CARAPI_(void) NotifyUserStyleChanged();

    CARAPI_(void) NotifyLocaleChanged();

    CARAPI_(void) NotifyFontScaleChanged();

private:
    /** Default captioning enabled value. */
    static const Int32 DEFAULT_ENABLED;

    /** Default style preset as an index into {@link CaptionStyle#PRESETS}. */
    static const Int32 DEFAULT_PRESET;

    /** Default scaling value for caption fonts. */
    static const Float DEFAULT_FONT_SCALE;

    AutoPtr<IArrayList> mListeners;
    AutoPtr<IHandler> mHandler;
    AutoPtr<IContentResolver> mContentResolver;
    AutoPtr<IContentObserver> mContentObserver;
    AutoPtr<IRunnable> mStyleChangedRunnable;
};

} // Accessibility
} // View
} // Droid
} // Elastos

#endif //__ELASTOS_DROID_VIEW_ACCESSIBILITY_CAPTIONINGMANAGER_H__
