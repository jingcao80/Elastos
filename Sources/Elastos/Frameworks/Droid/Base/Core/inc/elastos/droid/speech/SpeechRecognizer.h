#ifndef __ELASTOS_DROID_SPEECH_SPEECHRECOGNIZER_H__
#define __ELASTOS_DROID_SPEECH_SPEECHRECOGNIZER_H__

#include "elastos/droid/ext/frameworkdef.h"
#include "elastos/core/Object.h"
#include <elastos/utility/etl/List.h>
#include "_Elastos.Droid.Speech.h"
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Os.h"

using Elastos::Utility::Etl::List;
using Elastos::Droid::Os::IBundle;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Content::IIntent;
using Elastos::Droid::Content::IServiceConnection;
using Elastos::Droid::Content::IComponentName;
using Elastos::Droid::Content::IContext;
using Elastos::Droid::Os::IMessage;
using Elastos::Droid::Os::IHandler;

namespace Elastos {
namespace Droid {
namespace Speech {

/**
 * This class provides access to the speech recognition service. This service allows access to the
 * speech recognizer. Do not instantiate this class directly, instead, call
 * {@link SpeechRecognizer#createSpeechRecognizer(Context)}. This class's methods must be
 * invoked only from the main application thread.
 *
 * <p>The implementation of this API is likely to stream audio to remote servers to perform speech
 * recognition. As such this API is not intended to be used for continuous recognition, which would
 * consume a significant amount of battery and bandwidth.
 *
 * <p>Please note that the application must have {@link android.Manifest.permission#RECORD_AUDIO}
 * permission to use this class.
 */
//public class
class SpeechRecognizer
    : public Object
    , public ISpeechRecognizer
{
private:
    class SpeechRecognizerHandler
        : public Object
    {
    public:
        SpeechRecognizerHandler();

        SpeechRecognizerHandler(
            /* [in] */ SpeechRecognizer* sr);

        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);
    private:
        SpeechRecognizer* mHost;
    };
    friend class SpeechRecognizerHandler;

private:
    /**
     * Basic ServiceConnection that records the mService variable. Additionally, on creation it
     * invokes the {@link IRecognitionService#startListening(Intent, IRecognitionListener)}.
     */
    //private class
    class SpeechRecognizerConnection
        : public Object
        , public IServiceConnection
    {
    public:
        CAR_INTERFACE_DECL();

        SpeechRecognizerConnection();

        virtual ~SpeechRecognizerConnection();

        CARAPI constructor();

    public:
        //public void
        CARAPI OnServiceConnected(
            /* [in] */ IComponentName* name,
            /* [in] */ /*IIBinder*/IBinder* service
            );

        //public void
        CARAPI OnServiceDisconnected(
            /* [in] */ IComponentName* name);
    public:
        SpeechRecognizerConnection(
            /* [in] */ SpeechRecognizer* sr);
    private:
        SpeechRecognizer* mHost;
    };
    friend class SpeechRecognizerConnection;

private:
    /**
     * Internal wrapper of IRecognitionListener which will propagate the results to
     * RecognitionListener
     */
    //private class
    class SpeechRecognizerInternalListener
        : public Object
        , public IIRecognitionListener
    {
    private:
        class SpeechRecognizerInternalListenerHandler
            : public Object
        {
        public:
            SpeechRecognizerInternalListenerHandler();

            SpeechRecognizerInternalListenerHandler(
                /* [in] */ SpeechRecognizerInternalListener* sil);

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            AutoPtr<SpeechRecognizerInternalListener> mHost;
        };

    public:
        friend class SpeechRecognizerInternalListenerHandler;
        friend class SpeechRecognizer;

    public:
        CAR_INTERFACE_DECL();

        SpeechRecognizerInternalListener();

        ~SpeechRecognizerInternalListener();

        CARAPI constructor();

        //public void
        CARAPI OnBeginningOfSpeech();

        //public void
        CARAPI OnBufferReceived(
            /* [in] */ ArrayOf<Byte>* buffer);

        //public void
        CARAPI OnEndOfSpeech();

        //public void
        CARAPI OnError(
            /* [in] */ Int32 error);

        //public void
        CARAPI OnReadyForSpeech(
            /* [in] */ IBundle* noiseParams);

        //public void
        CARAPI OnResults(
            /* [in] */ IBundle* results);

        //public void
        CARAPI OnPartialResults(
            /* [in] */ IBundle* results);

        //public void
        CARAPI OnRmsChanged(
            /* [in] */ Float rmsdB);

        //public void
        CARAPI OnEvent(
            /* [in] */ Int32 eventType,
            /* [in] */ IBundle* bParams);
    private:
        //private
        AutoPtr<IRecognitionListener> mInternalListener;

        //private final static
        const static Int32 MSG_BEGINNING_OF_SPEECH;     // = 1;
        //private final static
        const static Int32 MSG_BUFFER_RECEIVED;         // = 2;
        //private final static
        const static Int32 MSG_END_OF_SPEECH;           // = 3;
        //private final static
        const static Int32 MSG_ERROR;                   // = 4;
        //private final static
        const static Int32 MSG_READY_FOR_SPEECH;        // = 5;
        //private final static
        const static Int32 MSG_RESULTS;                 // = 6;
        //private final static
        const static Int32 MSG_PARTIAL_RESULTS;         // = 7;
        //private final static
        const static Int32 MSG_RMS_CHANGED;             // = 8;
        //private final static
        const static Int32 MSG_ON_EVENT;                // = 9;

    private:
        //private final Handler
        AutoPtr<IHandler> mInternalHandler;             // = new SpeechRecognizerInternalListenerHandler();
    };

    friend class SpeechRecognizerInternalListener;

public:
    CAR_INTERFACE_DECL();

    SpeechRecognizer();

    virtual ~SpeechRecognizer();

    CARAPI constructor();

    /**
     * The right way to create a {@code SpeechRecognizer} is by using
     * {@link #createSpeechRecognizer} static factory method
     */
    CARAPI constructor(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* serviceComponent);

    /**
     * Checks whether a speech recognition service is available on the system. If this method
     * returns {@code false}, {@link SpeechRecognizer#createSpeechRecognizer(Context)} will
     * fail.
     *
     * @param context with which {@code SpeechRecognizer} will be created
     * @return {@code true} if recognition is available, {@code false} otherwise
     */
    //public
    static CARAPI_(Boolean) IsRecognitionAvailable(
        /* [in] */ IContext* context);

    /**
     * Factory method to create a new {@code SpeechRecognizer}. Please note that
     * {@link #setRecognitionListener(RecognitionListener)} should be called before dispatching any
     * command to the created {@code SpeechRecognizer}, otherwise no notifications will be
     * received.
     *
     * @param context in which to create {@code SpeechRecognizer}
     * @return a new {@code SpeechRecognizer}
     */
    //public
    static CARAPI_(AutoPtr<ISpeechRecognizer>) CreateSpeechRecognizer(
        /* [in] */ IContext* context);

    /**
     * Factory method to create a new {@code SpeechRecognizer}. Please note that
     * {@link #setRecognitionListener(RecognitionListener)} should be called before dispatching any
     * command to the created {@code SpeechRecognizer}, otherwise no notifications will be
     * received.
     *
     * Use this version of the method to specify a specific service to direct this
     * {@link SpeechRecognizer} to. Normally you would not use this; use
     * {@link #createSpeechRecognizer(Context)} instead to use the system default recognition
     * service.
     *
     * @param context in which to create {@code SpeechRecognizer}
     * @param serviceComponent the {@link ComponentName} of a specific service to direct this
     *        {@code SpeechRecognizer} to
     * @return a new {@code SpeechRecognizer}
     */
    //public
    static CARAPI_(AutoPtr<ISpeechRecognizer>) CreateSpeechRecognizer(
        /* [in] */ IContext* context,
        /* [in] */ IComponentName* serviceComponent);

    /**
     * Sets the listener that will receive all the callbacks. The previous unfinished commands will
     * be executed with the old listener, while any following command will be executed with the new
     * listener.
     *
     * @param listener listener that will receive all the callbacks from the created
     *        {@link SpeechRecognizer}, this must not be null.
     */
    //public
    CARAPI SetRecognitionListener(
        /* [in] */ IRecognitionListener* listener);

    /**
     * Starts listening for speech. Please note that
     * {@link #setRecognitionListener(RecognitionListener)} should be called beforehand, otherwise
     * no notifications will be received.
     *
     * @param recognizerIntent contains parameters for the recognition to be performed. The intent
     *        may also contain optional extras, see {@link RecognizerIntent}. If these values are
     *        not set explicitly, default values will be used by the recognizer.
     */
    //public
    CARAPI StartListening(
        /* [in] */ IIntent* recognizerIntent);

    /**
     * Stops listening for speech. Speech captured so far will be recognized as if the user had
     * stopped speaking at this point. Note that in the default case, this does not need to be
     * called, as the speech endpointer will automatically stop the recognizer listening when it
     * determines speech has completed. However, you can manipulate endpointer parameters directly
     * using the intent extras defined in {@link RecognizerIntent}, in which case you may sometimes
     * want to manually call this method to stop listening sooner. Please note that
     * {@link #setRecognitionListener(RecognitionListener)} should be called beforehand, otherwise
     * no notifications will be received.
     */
    //public
    CARAPI StopListening();

    /**
     * Cancels the speech recognition. Please note that
     * {@link #setRecognitionListener(RecognitionListener)} should be called beforehand, otherwise
     * no notifications will be received.
     */
    //public
    CARAPI Cancel();

    /**
     * Destroys the {@code SpeechRecognizer} object.
     */
    //public
    CARAPI Destroy();

protected:
    //private
    static CARAPI_(void) CheckIsCalledFromMainThread();

    //private
    CARAPI_(void) PutMessage(
        /* [in] */ IMessage* msg);

    /** sends the actual message to the service */
    //private
    CARAPI_(void) HandleStartListening(
        /* [in] */ IIntent* recognizerIntent);

    /** sends the actual message to the service */
    //private
    CARAPI_(void) HandleStopMessage();

    /** sends the actual message to the service */
    //private
    CARAPI_(void) HandleCancelMessage();

    //private
    CARAPI_(Boolean) CheckOpenConnection();

    /** changes the listener */
    //private
    CARAPI_(void) HandleChangeListener(
        /* [in] */ IRecognitionListener* listener);

protected:
    /** DEBUG value to enable verbose debug prints */
    //private final static
    static const Boolean DBG;                       // = FALSE;

    /** Log messages identifier */
    //private static final
    static const String TAG;                        // = "SpeechRecognizer";

private:
    /** action codes */
    //private final static
    static const Int32 MSG_START;                   // = 1;

    //private final static
    static const Int32 MSG_STOP;                    // = 2;

    //private final static
    static const Int32 MSG_CANCEL;                  // = 3;

    //private final static
    static const Int32 MSG_CHANGE_LISTENER;         // = 4;

    /** The actual RecognitionService endpoint */
    //private
    AutoPtr<IIRecognitionService> mService;

    /** The connection to the actual service */
    //private
    AutoPtr<SpeechRecognizerConnection> mConnection;

    /** Context with which the manager was created */
    //private final
    AutoPtr<IContext> mContext;

    /** Component to direct service intent to */
    //private final
    AutoPtr<IComponentName> mServiceComponent;

    /** Handler that will execute the main tasks */
    //private
    AutoPtr<IHandler> mHandler;                             // = new SpeechRecognizerHandler();

    /**
     * Temporary queue, saving the messages until the connection will be established, afterwards,
     * only mHandler will receive the messages
     */
    //private final Queue<Message>
    List< AutoPtr<IMessage> > mPendingTasks;                // = new LinkedList<Message>();

    /** The Listener that will receive all the callbacks */
    //private final InternalListener
    AutoPtr<SpeechRecognizerInternalListener> mListener;    // = new SpeechRecognizerInternalListener();
};

} // namespace Speech
} // namepsace Droid
} // namespace Elastos

#endif // __ELASTOS_DROID_SPEECH_SPEECHRECOGNIZER_H__
