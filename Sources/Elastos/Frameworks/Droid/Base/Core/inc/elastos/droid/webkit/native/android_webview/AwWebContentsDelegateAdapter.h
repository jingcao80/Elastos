#ifndef __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__
#define __ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/webkit/native/content/browser/ContentViewCore.h"
#include "elastos/droid/webkit/native/android_webview/AwWebContentsDelegate.h"
#include "elastos/droid/webkit/native/android_webview/AwContentsClient.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/AsyncTask.h"

using Elastos::Droid::Content::IContentResolver;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::AsyncTask;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::ILooper;
using Elastos::Droid::View::IKeyEvent;
using Elastos::Droid::View::IView;
//TODO using Elastos::Droid::Webkit::IValueCallback;

using Elastos::Droid::Webkit::Content::Browser::ContentViewCore;

namespace Elastos {
namespace Droid {
namespace Webkit {
namespace AndroidWebview {

/**
 * Adapts the AwWebContentsDelegate interface to the AwContentsClient interface.
 * This class also serves a secondary function of routing certain callbacks from the content layer
 * to specific listener interfaces.
 */
class AwWebContentsDelegateAdapter : public AwWebContentsDelegate
{
public:
    class GetDisplayNameTask : public AsyncTask
    {
    public:
        GetDisplayNameTask(
            /* [in] */ IContentResolver* contentResolver,
            /* [in] */ Int32 processId,
            /* [in] */ Int32 renderId,
            /* [in] */ Int32 modeFlags,
            /* [in] */ ArrayOf<String>* filePaths);

    public:
        const Int32 mProcessId;
        const Int32 mRenderId;
        const Int32 mModeFlags;
        AutoPtr<ArrayOf<String> > mFilePaths;
        AutoPtr<IContentResolver> mContentResolver;

    protected:
        //@Override
        CARAPI DoInBackground(
            /* [in] */ ArrayOf<IInterface*>* params,
            /* [in] */ IInterface** result);

        //@Override
        CARAPI OnPostExecute(
            /* [in] */ IInterface* result);

    private:
        /**
         * @return the display name of a path if it is a content URI and is present in the database
         * or an empty string otherwise.
         */
        CARAPI_(String) ResolveFileName(
            /* [in] */ const String& filePath);
    };

    class InnerHandler
        : public Handler
    {
    public:
        InnerHandler(
            /* [in] */ AwWebContentsDelegateAdapter* owner,
            /* [in] */ ILooper* looper,
            /* [in] */ ContentViewCore* contentViewCore);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        AwWebContentsDelegateAdapter* mOwner;
        ContentViewCore* mContentViewCore;
    };

    class InnerValueCallback
        : public Object
        //TODO , public IValueCallback
    {
    public:
        //TODO CAR_INTERFACE_DECL();
        InnerValueCallback(
            /* [in] */ AwWebContentsDelegateAdapter* owner,
            /* [in] */ Int32 processId,
            /* [in] */ Int32 renderId,
            /* [in] */ Int32 modeFlags,
            /* [in] */ Boolean completed);

        CARAPI OnReceiveValue(
            /* [in] */ IInterface* results);
    private:
        AwWebContentsDelegateAdapter* mOwner;
        Int32 mProcessId;
        Int32 mRenderId;
        Int32 mModeFlags;
        Boolean mCompleted;
    };

public:
    AutoPtr<AwContentsClient> mContentsClient;
    AutoPtr<IView> mContainerView;
    AutoPtr<IContext> mContext;

public:
    AwWebContentsDelegateAdapter(
        /* [in] */ AwContentsClient* contentsClient,
        /* [in] */ IView* containerView,
        /* [in] */ IContext* context);

    CARAPI_(void) SetContainerView(
        /* [in] */ IView* containerView);

    //@Override
    CARAPI OnLoadProgressChanged(
        /* [in] */ Int32 progress);

    //@Override
    CARAPI HandleKeyboardEvent(
        /* [in] */ IKeyEvent* event);

    //@Override
    CARAPI_(Boolean) TakeFocus(
        /* [in] */ Boolean reverse);

    //@Override
    CARAPI_(Boolean) AddMessageToConsole(
        /* [in] */ Int32 level,
        /* [in] */ const String& message,
        /* [in] */ Int32 lineNumber,
        /* [in] */ const String& sourceId);

    //@Override
    CARAPI OnUpdateUrl(
        /* [in] */ const String& url);

    //@Override
    CARAPI OpenNewTab(
        /* [in] */ const String& url,
        /* [in] */ const String& extraHeaders,
        /* [in] */ ArrayOf<Byte>* postData,
        /* [in] */ Int32 disposition,
        /* [in] */ Boolean isRendererInitiated);

    //@Override
    CARAPI CloseContents();

    //@Override
    CARAPI ShowRepostFormWarningDialog(
        /* [in] */ ContentViewCore* contentViewCore);

    //@Override
    CARAPI_(void) RunFileChooser(
        /* [in] */ Int32 processId,
        /* [in] */ Int32 renderId,
        /* [in] */ Int32 modeFlags,
        /* [in] */ const String& acceptTypes,
        /* [in] */ const String& title,
        /* [in] */ const String& defaultFilename,
        /* [in] */ Boolean capture);

    //@Override
    CARAPI_(Boolean) AddNewContents(
        /* [in] */ Boolean isDialog,
        /* [in] */ Boolean isUserGesture);

    //@Override
    CARAPI ActivateContents();

    //@Override
    CARAPI ToggleFullscreenModeForTab(
        /* [in] */ Boolean enterFullscreen);

private:
    static const String TAG;
    static const Int32 MSG_CONTINUE_PENDING_RELOAD;
    static const Int32 MSG_CANCEL_PENDING_RELOAD;


    CARAPI_(Boolean) TryToMoveFocus(
        /* [in] */ Int32 direction);
};

} // namespace AndroidWebview
} // namespace Webkit
} // namespace Droid
} // namespace Elastos

#endif//__ELASTOS_DROID_WEBKIT_ANDROIDWEBVIEW_AWWEBCONTENTSDELEGATEADAPTER_H__
