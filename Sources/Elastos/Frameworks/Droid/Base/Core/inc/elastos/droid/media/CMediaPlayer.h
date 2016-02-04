
#ifndef __ELASTOS_DROID_MEDIA_CMEDIAPLAYER_H__
#define __ELASTOS_DROID_MEDIA_CMEDIAPLAYER_H__

#include <Elastos.CoreLibrary.Core.h>
#include <Elastos.CoreLibrary.IO.h>
#include <Elastos.CoreLibrary.Libcore.h>
#include <Elastos.CoreLibrary.Net.h>
#include <Elastos.CoreLibrary.Utility.h>
#include "Elastos.Droid.Content.h"
#include "Elastos.Droid.Internal.h"
#include "Elastos.Droid.Os.h"
#include "Elastos.Droid.View.h"
#include "_Elastos_Droid_Media_CMediaPlayer.h"
#include "elastos/droid/ext/frameworkext.h"
#include "elastos/droid/os/Handler.h"
#include "elastos/droid/os/Runnable.h"

using Elastos::Droid::Content::IContext;
using Elastos::Droid::Graphics::IBitmap;
using Elastos::Droid::Internal::App::IIAppOpsService;
using Elastos::Droid::Net::IUri;
using Elastos::Droid::Os::Handler;
using Elastos::Droid::Os::IBinder;
using Elastos::Droid::Os::IHandlerThread;
using Elastos::Droid::Os::IPowerManagerWakeLock;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::View::ISurface;
using Elastos::Droid::View::ISurfaceHolder;
using Elastos::IO::IFileDescriptor;
using Elastos::IO::IInputStream;
using Elastos::Net::IInetSocketAddress;
using Elastos::Utility::IMap;
using Elastos::Utility::ISet;
using Elastos::Utility::IVector;

namespace Elastos {
namespace Droid {
namespace Media {

/**
 * MediaPlayer class can be used to control playback
 * of audio/video files and streams. An example on how to use the methods in
 * this class can be found in {@link android.widget.VideoView}.
 * Please see <a href="{@docRoot}guide/topics/media/index.html">Audio and Video</a>
 * for additional help using MediaPlayer.
 *
 * <p>Topics covered here are:
 * <ol>
 * <li><a href="#StateDiagram">State Diagram</a>
 * <li><a href="#Valid_and_Invalid_States">Valid and Invalid States</a>
 * <li><a href="#Permissions">Permissions</a>
 * </ol>
 *
 * <a name="StateDiagram"></a>
 * <h3>State Diagram</h3>
 *
 * <p>Playback control of audio/video files and streams is managed as a state
 * machine. The following diagram shows the life cycle and the states of a
 * MediaPlayer object driven by the supported playback control operations.
 * The ovals represent the states a MediaPlayer object may reside
 * in. The arcs represent the playback control operations that drive the object
 * state transition. There are two types of arcs. The arcs with a single arrow
 * head represent synchronous method calls, while those with
 * a double arrow head represent asynchronous method calls.</p>
 *
 * <p><img src="../../../images/mediaplayer_state_diagram.gif"
 *         alt="MediaPlayer State diagram"
 *         border="0" /></p>
 *
 * <p>From this state diagram, one can see that a MediaPlayer object has the
 *    following states:</p>
 * <ul>
 *     <li>When a MediaPlayer object is just created using <code>new</code> or
 *         after {@link #reset()} is called, it is in the <em>Idle</em> state; and after
 *         {@link #release()} is called, it is in the <em>End</em> state. Between these
 *         two states is the life cycle of the MediaPlayer object.
 *         <ul>
 *         <li>There is a subtle but important difference between a newly constructed
 *         MediaPlayer object and the MediaPlayer object after {@link #reset()}
 *         is called. It is a programming error to invoke methods such
 *         as {@link #getCurrentPosition()},
 *         {@link #getDuration()}, {@link #getVideoHeight()},
 *         {@link #getVideoWidth()}, {@link #setAudioStreamType(int)},
 *         {@link #setLooping(boolean)},
 *         {@link #setVolume(float, float)}, {@link #pause()}, {@link #start()},
 *         {@link #stop()}, {@link #seekTo(int)}, {@link #prepare()} or
 *         {@link #prepareAsync()} in the <em>Idle</em> state for both cases. If any of these
 *         methods is called right after a MediaPlayer object is constructed,
 *         the user supplied callback method OnErrorListener.onError() won't be
 *         called by the internal player engine and the object state remains
 *         unchanged; but if these methods are called right after {@link #reset()},
 *         the user supplied callback method OnErrorListener.onError() will be
 *         invoked by the internal player engine and the object will be
 *         transfered to the <em>Error</em> state. </li>
 *         <li>It is also recommended that once
 *         a MediaPlayer object is no longer being used, call {@link #release()} immediately
 *         so that resources used by the internal player engine associated with the
 *         MediaPlayer object can be released immediately. Resource may include
 *         singleton resources such as hardware acceleration components and
 *         failure to call {@link #release()} may cause subsequent instances of
 *         MediaPlayer objects to fallback to software implementations or fail
 *         altogether. Once the MediaPlayer
 *         object is in the <em>End</em> state, it can no longer be used and
 *         there is no way to bring it back to any other state. </li>
 *         <li>Furthermore,
 *         the MediaPlayer objects created using <code>new</code> is in the
 *         <em>Idle</em> state, while those created with one
 *         of the overloaded convenient <code>create</code> methods are <em>NOT</em>
 *         in the <em>Idle</em> state. In fact, the objects are in the <em>Prepared</em>
 *         state if the creation using <code>create</code> method is successful.
 *         </li>
 *         </ul>
 *         </li>
 *     <li>In general, some playback control operation may fail due to various
 *         reasons, such as unsupported audio/video format, poorly interleaved
 *         audio/video, resolution too high, streaming timeout, and the like.
 *         Thus, error reporting and recovery is an important concern under
 *         these circumstances. Sometimes, due to programming errors, invoking a playback
 *         control operation in an invalid state may also occur. Under all these
 *         error conditions, the internal player engine invokes a user supplied
 *         OnErrorListener.onError() method if an OnErrorListener has been
 *         registered beforehand via
 *         {@link #setOnErrorListener(android.media.MediaPlayer.OnErrorListener)}.
 *         <ul>
 *         <li>It is important to note that once an error occurs, the
 *         MediaPlayer object enters the <em>Error</em> state (except as noted
 *         above), even if an error listener has not been registered by the application.</li>
 *         <li>In order to reuse a MediaPlayer object that is in the <em>
 *         Error</em> state and recover from the error,
 *         {@link #reset()} can be called to restore the object to its <em>Idle</em>
 *         state.</li>
 *         <li>It is good programming practice to have your application
 *         register a OnErrorListener to look out for error notifications from
 *         the internal player engine.</li>
 *         <li>IllegalStateException is
 *         thrown to prevent programming errors such as calling {@link #prepare()},
 *         {@link #prepareAsync()}, or one of the overloaded <code>setDataSource
 *         </code> methods in an invalid state. </li>
 *         </ul>
 *         </li>
 *     <li>Calling
 *         {@link #setDataSource(FileDescriptor)}, or
 *         {@link #setDataSource(String)}, or
 *         {@link #setDataSource(Context, Uri)}, or
 *         {@link #setDataSource(FileDescriptor, Int64, Int64)} transfers a
 *         MediaPlayer object in the <em>Idle</em> state to the
 *         <em>Initialized</em> state.
 *         <ul>
 *         <li>An IllegalStateException is thrown if
 *         setDataSource() is called in any other state.</li>
 *         <li>It is good programming
 *         practice to always look out for <code>IllegalArgumentException</code>
 *         and <code>IOException</code> that may be thrown from the overloaded
 *         <code>setDataSource</code> methods.</li>
 *         </ul>
 *         </li>
 *     <li>A MediaPlayer object must first enter the <em>Prepared</em> state
 *         before playback can be started.
 *         <ul>
 *         <li>There are two ways (synchronous vs.
 *         asynchronous) that the <em>Prepared</em> state can be reached:
 *         either a call to {@link #prepare()} (synchronous) which
 *         transfers the object to the <em>Prepared</em> state once the method call
 *         returns, or a call to {@link #prepareAsync()} (asynchronous) which
 *         first transfers the object to the <em>Preparing</em> state after the
 *         call returns (which occurs almost right way) while the internal
 *         player engine continues working on the rest of preparation work
 *         until the preparation work completes. When the preparation completes or when {@link #prepare()} call returns,
 *         the internal player engine then calls a user supplied callback method,
 *         onPrepared() of the OnPreparedListener interface, if an
 *         OnPreparedListener is registered beforehand via {@link
 *         #setOnPreparedListener(android.media.MediaPlayer.OnPreparedListener)}.</li>
 *         <li>It is important to note that
 *         the <em>Preparing</em> state is a transient state, and the behavior
 *         of calling any method with side effect while a MediaPlayer object is
 *         in the <em>Preparing</em> state is undefined.</li>
 *         <li>An IllegalStateException is
 *         thrown if {@link #prepare()} or {@link #prepareAsync()} is called in
 *         any other state.</li>
 *         <li>While in the <em>Prepared</em> state, properties
 *         such as audio/sound volume, screenOnWhilePlaying, looping can be
 *         adjusted by invoking the corresponding set methods.</li>
 *         </ul>
 *         </li>
 *     <li>To start the playback, {@link #start()} must be called. After
 *         {@link #start()} returns successfully, the MediaPlayer object is in the
 *         <em>Started</em> state. {@link #isPlaying()} can be called to test
 *         whether the MediaPlayer object is in the <em>Started</em> state.
 *         <ul>
 *         <li>While in the <em>Started</em> state, the internal player engine calls
 *         a user supplied OnBufferingUpdateListener.onBufferingUpdate() callback
 *         method if a OnBufferingUpdateListener has been registered beforehand
 *         via {@link #setOnBufferingUpdateListener(OnBufferingUpdateListener)}.
 *         This callback allows applications to keep track of the buffering status
 *         while streaming audio/video.</li>
 *         <li>Calling {@link #start()} has not effect
 *         on a MediaPlayer object that is already in the <em>Started</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>Playback can be paused and stopped, and the current playback position
 *         can be adjusted. Playback can be paused via {@link #pause()}. When the call to
 *         {@link #pause()} returns, the MediaPlayer object enters the
 *         <em>Paused</em> state. Note that the transition from the <em>Started</em>
 *         state to the <em>Paused</em> state and vice versa happens
 *         asynchronously in the player engine. It may take some time before
 *         the state is updated in calls to {@link #isPlaying()}, and it can be
 *         a number of seconds in the case of streamed content.
 *         <ul>
 *         <li>Calling {@link #start()} to resume playback for a paused
 *         MediaPlayer object, and the resumed playback
 *         position is the same as where it was paused. When the call to
 *         {@link #start()} returns, the paused MediaPlayer object goes back to
 *         the <em>Started</em> state.</li>
 *         <li>Calling {@link #pause()} has no effect on
 *         a MediaPlayer object that is already in the <em>Paused</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>Calling  {@link #stop()} stops playback and causes a
 *         MediaPlayer in the <em>Started</em>, <em>Paused</em>, <em>Prepared
 *         </em> or <em>PlaybackCompleted</em> state to enter the
 *         <em>Stopped</em> state.
 *         <ul>
 *         <li>Once in the <em>Stopped</em> state, playback cannot be started
 *         until {@link #prepare()} or {@link #prepareAsync()} are called to set
 *         the MediaPlayer object to the <em>Prepared</em> state again.</li>
 *         <li>Calling {@link #stop()} has no effect on a MediaPlayer
 *         object that is already in the <em>Stopped</em> state.</li>
 *         </ul>
 *         </li>
 *     <li>The playback position can be adjusted with a call to
 *         {@link #seekTo(int)}.
 *         <ul>
 *         <li>Although the asynchronuous {@link #seekTo(int)}
 *         call returns right way, the actual seek operation may take a while to
 *         finish, especially for audio/video being streamed. When the actual
 *         seek operation completes, the internal player engine calls a user
 *         supplied OnSeekComplete.onSeekComplete() if an OnSeekCompleteListener
 *         has been registered beforehand via
 *         {@link #setOnSeekCompleteListener(OnSeekCompleteListener)}.</li>
 *         <li>Please
 *         note that {@link #seekTo(int)} can also be called in the other states,
 *         such as <em>Prepared</em>, <em>Paused</em> and <em>PlaybackCompleted
 *         </em> state.</li>
 *         <li>Furthermore, the actual current playback position
 *         can be retrieved with a call to {@link #getCurrentPosition()}, which
 *         is helpful for applications such as a Music player that need to keep
 *         track of the playback progress.</li>
 *         </ul>
 *         </li>
 *     <li>When the playback reaches the end of stream, the playback completes.
 *         <ul>
 *         <li>If the looping mode was being set to <var>true</var>with
 *         {@link #setLooping(boolean)}, the MediaPlayer object shall remain in
 *         the <em>Started</em> state.</li>
 *         <li>If the looping mode was set to <var>false
 *         </var>, the player engine calls a user supplied callback method,
 *         OnCompletion.onCompletion(), if a OnCompletionListener is registered
 *         beforehand via {@link #setOnCompletionListener(OnCompletionListener)}.
 *         The invoke of the callback signals that the object is now in the <em>
 *         PlaybackCompleted</em> state.</li>
 *         <li>While in the <em>PlaybackCompleted</em>
 *         state, calling {@link #start()} can restart the playback from the
 *         beginning of the audio/video source.</li>
 * </ul>
 *
 *
 * <a name="Valid_and_Invalid_States"></a>
 * <h3>Valid and invalid states</h3>
 *
 * <table border="0" cellspacing="0" cellpadding="0">
 * <tr><td>Method Name </p></td>
 *     <td>Valid Sates </p></td>
 *     <td>Invalid States </p></td>
 *     <td>Comments </p></td></tr>
 * <tr><td>attachAuxEffect </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted} </p></td>
 *     <td>{Idle, Error} </p></td>
 *     <td>This method must be called after setDataSource.
 *     Calling it does not change the object state. </p></td></tr>
 * <tr><td>getAudioSessionId </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>getCurrentPosition </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted} </p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state. </p></td></tr>
 * <tr><td>getDuration </p></td>
 *     <td>{Prepared, Started, Paused, Stopped, PlaybackCompleted} </p></td>
 *     <td>{Idle, Initialized, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state. </p></td></tr>
 * <tr><td>getVideoHeight </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change the
 *         state. Calling this method in an invalid state transfers the object
 *         to the <em>Error</em> state.  </p></td></tr>
 * <tr><td>getVideoWidth </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>isPlaying </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>pause </p></td>
 *     <td>{Started, Paused}</p></td>
 *     <td>{Idle, Initialized, Prepared, Stopped, PlaybackCompleted, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Paused</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>prepare </p></td>
 *     <td>{Initialized, Stopped} </p></td>
 *     <td>{Idle, Prepared, Started, Paused, PlaybackCompleted, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Prepared</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>prepareAsync </p></td>
 *     <td>{Initialized, Stopped} </p></td>
 *     <td>{Idle, Prepared, Started, Paused, PlaybackCompleted, Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Preparing</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>release </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>After {@link #release()}, the object is no longer available. </p></td></tr>
 * <tr><td>reset </p></td>
 *     <td>{Idle, Initialized, Prepared, Started, Paused, Stopped,
 *         PlaybackCompleted, Error}</p></td>
 *     <td>{}</p></td>
 *     <td>After {@link #reset()}, the object is like being just created.</p></td></tr>
 * <tr><td>seekTo </p></td>
 *     <td>{Prepared, Started, Paused, PlaybackCompleted} </p></td>
 *     <td>{Idle, Initialized, Stopped, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an invalid state transfers the
 *         object to the <em>Error</em> state. </p></td></tr>
 * <tr><td>setAudioSessionId </p></td>
 *     <td>{Idle} </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted,
 *          Error} </p></td>
 *     <td>This method must be called in idle state as the audio session ID must be known before
 *         calling setDataSource. Calling it does not change the object state. </p></td></tr>
 * <tr><td>setAudioStreamType </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method does not change the state. In order for the
 *         target audio stream type to become effective, this method must be called before
 *         prepare() or prepareAsync().</p></td></tr>
 * <tr><td>setAuxEffectSendLevel </p></td>
 *     <td>any</p></td>
 *     <td>{} </p></td>
 *     <td>Calling this method does not change the object state. </p></td></tr>
 * <tr><td>setDataSource </p></td>
 *     <td>{Idle} </p></td>
 *     <td>{Initialized, Prepared, Started, Paused, Stopped, PlaybackCompleted,
 *          Error} </p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Initialized</em> state. Calling this method in an
 *         invalid state throws an IllegalStateException.</p></td></tr>
 * <tr><td>setDisplay </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setLooping </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *         PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method in a valid state does not change
 *         the state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>isLooping </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnBufferingUpdateListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnCompletionListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnErrorListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnPreparedListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setOnSeekCompleteListener </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state. </p></td></tr>
 * <tr><td>setScreenOnWhilePlaying</></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state.  </p></td></tr>
 * <tr><td>setVolume </p></td>
 *     <td>{Idle, Initialized, Stopped, Prepared, Started, Paused,
 *          PlaybackCompleted}</p></td>
 *     <td>{Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.
 * <tr><td>setWakeMode </p></td>
 *     <td>any </p></td>
 *     <td>{} </p></td>
 *     <td>This method can be called in any state and calling it does not change
 *         the object state.</p></td></tr>
 * <tr><td>start </p></td>
 *     <td>{Prepared, Started, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Stopped, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Started</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 * <tr><td>stop </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method in a valid state transfers the
 *         object to the <em>Stopped</em> state. Calling this method in an
 *         invalid state transfers the object to the <em>Error</em> state.</p></td></tr>
 *
  * <tr><td>getTrackInfo </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>addTimedTextSource </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>selectTrack </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * <tr><td>deselectTrack </p></td>
 *     <td>{Prepared, Started, Stopped, Paused, PlaybackCompleted}</p></td>
 *     <td>{Idle, Initialized, Error}</p></td>
 *     <td>Successful invoke of this method does not change the state.</p></td></tr>
 * </table>
 *
 * <p>This class requires the {@link android.Manifest.permission#INTERNET} permission
 * when used with network-based content.
 *
 * <a name="Callbacks"></a>
 * <h3>Callbacks</h3>
 * <p>Applications may want to register for informational and error
 * events in order to be informed of some internal state update and
 * possible runtime errors during playback or streaming. Registration for
 * these events is done by properly setting the appropriate listeners (via calls
 * to
 * {@link #setOnPreparedListener(OnPreparedListener)}setOnPreparedListener,
 * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}setOnVideoSizeChangedListener,
 * {@link #setOnSeekCompleteListener(OnSeekCompleteListener)}setOnSeekCompleteListener,
 * {@link #setOnCompletionListener(OnCompletionListener)}setOnCompletionListener,
 * {@link #setOnBufferingUpdateListener(OnBufferingUpdateListener)}setOnBufferingUpdateListener,
 * {@link #setOnInfoListener(OnInfoListener)}setOnInfoListener,
 * {@link #setOnErrorListener(OnErrorListener)}setOnErrorListener, etc).
 * In order to receive the respective callback
 * associated with these listeners, applications are required to create
 * MediaPlayer objects on a thread with its own Looper running (main UI
 * thread by default has a Looper running).
 *
 * <a name="Permissions"></a>
 * <h3>Permissions</h3>
 * <p>One may need to declare a corresponding WAKE_LOCK permission {@link
 * android.R.styleable#AndroidManifestUsesPermission &lt;uses-permission&gt;}
 * element.
 *
 */
CarClass(CMediaPlayer)
    , public Object
    , public IMediaPlayer
    , public ISubtitleControllerListener
{
public:
    class TimeProvider
        : public Object
        , public IMediaPlayerOnSeekCompleteListener
        , public IMediaTimeProvider
    {
    private:
        class EventHandler
            : public Handler
        {
        public:
            EventHandler(
                /* [in] */ TimeProvider* host,
                /* [in] */ ILooper* looper)
                    : Handler(looper)
                    , mHost(host)
                {}

            CARAPI HandleMessage(
                /* [in] */ IMessage* msg);

        private:
            TimeProvider* mHost;
        };

    public:
        CAR_INTERFACE_DECL()

        TimeProvider(
            /* [in] */ CMediaPlayer* mp);

        ~TimeProvider();

        CARAPI Close();

        CARAPI OnPaused(
            /* [in] */ Boolean paused);

        CARAPI OnStopped();

        CARAPI OnSeekComplete(
            /* [in] */ IMediaPlayer* mp);

        CARAPI OnNewPlayer();

        CARAPI NotifyAt(
            /* [in] */ Int64 timeUs,
            /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener);

        CARAPI ScheduleUpdate(
            /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener);

        CARAPI CancelNotifications(
            /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener);

        CARAPI GetCurrentTimeUs(
            /* [in] */ Boolean refreshTime,
            /* [in] */ Boolean monotonic,
            /* [out] */ Int64* result);

    private:
        CARAPI_(void) ScheduleNotification(
            /* [in] */ Int32 type,
            /* [in] */ Int64 delayUs);

        /*synchronized*/
        CARAPI_(void) NotifySeek();

        /*synchronized*/
        CARAPI_(void) NotifyStop();

        CARAPI_(Int32) RegisterListener(
            /* [in] */ IMediaTimeProviderOnMediaTimeListener* listener);

        /*synchronized*/
        CARAPI_(void) NotifyTimedEvent(
            /* [in] */ Boolean refreshTime);

        CARAPI_(Int64) GetEstimatedTime(
            /* [in] */ Int64 nanoTime,
            /* [in] */ Boolean monotonic);

    public:
        /** @hide */
        Boolean DEBUG;

    private:
        static const String TAG;
        static const Int64 MAX_NS_WITHOUT_POSITION_CHECK;
        static const Int64 MAX_EARLY_CALLBACK_US;
        static const Int64 TIME_ADJUSTMENT_RATE;
        Int64 mLastTimeUs;
        AutoPtr<CMediaPlayer> mPlayer;
        Boolean mPaused;
        Boolean mStopped;
        Int64 mLastReportedTime;
        Int64 mTimeAdjustment;
        // since we are expecting only a handful listeners per stream, there is
        // no need for log(N) search performance
        AutoPtr<ArrayOf<IMediaTimeProviderOnMediaTimeListener*> > mListeners;
        AutoPtr<ArrayOf<Int64> > mTimes;
        Int64 mLastNanoTime;
        AutoPtr<IHandler> mEventHandler;
        Boolean mRefresh;
        Boolean mPausing;
        Boolean mSeeking;
        static const Int32 NOTIFY;
        static const Int32 NOTIFY_TIME;
        static const Int32 REFRESH_AND_NOTIFY_TIME;
        static const Int32 NOTIFY_STOP;
        static const Int32 NOTIFY_SEEK;
        AutoPtr<IHandlerThread> mHandlerThread;
    };

private:
    class EventHandler
        : public Handler
    {
    public:
        friend class CMediaPlayer;

        EventHandler(
            /* [in] */ CMediaPlayer* host,
            /* [in] */ ILooper* looper);

        //@Override
        CARAPI HandleMessage(
            /* [in] */ IMessage* msg);

    private:
        CMediaPlayer* mMediaPlayer;
    };

    class MediaPlayerOnSubtitleDataListener
        : public Object
        , public IMediaPlayerOnSubtitleDataListener
    {
    public:
        CAR_INTERFACE_DECL()

        MediaPlayerOnSubtitleDataListener(
            /* [in] */ CMediaPlayer* host)
            : mHost(host)
        {}

        CARAPI OnSubtitleData(
            /* [in] */ IMediaPlayer* mp,
            /* [in] */ ISubtitleData* data);

    private:
        CMediaPlayer* mHost;
    };

    class HandlerRunnable
        : public Runnable
    {
    public:
        HandlerRunnable(
            /* [in] */ CMediaPlayer* host,
            /* [in] */ IHandlerThread* thread,
            /* [in] */ IInputStream* is,
            /* [in] */ IMediaFormat* format)
            : mHost(host)
            , mThread(thread)
            , mIs(is)
            , mFormat(format)
        {}

        CARAPI Run();

    private:
        CARAPI_(Int32) AddTrack();

    private:
        CMediaPlayer* mHost;
        AutoPtr<IHandlerThread> mThread;
        AutoPtr<IInputStream> mIs;
        AutoPtr<IMediaFormat> mFormat;
    };

    class MyAnchor
        : public Object
        , public ISubtitleControllerAnchor
    {
    public:
        CAR_INTERFACE_DECL()

        MyAnchor(
            /* [in] */ CMediaPlayer* host)
            : mHost(host)
        {}

        CARAPI SetSubtitleWidget(
            /* [in] */ ISubtitleTrackRenderingWidget* subtitleWidget);

        CARAPI GetSubtitleLooper(
            /* [out] */ ILooper** result);

    private:
        CMediaPlayer* mHost;
    };

    class MyRunnable
        : public Runnable
    {
    public:
        MyRunnable(
            /* [in] */ CMediaPlayer* host,
            /* [in] */ IHandlerThread* thread,
            /* [in] */ IFileDescriptor* fd,
            /* [in] */ Int64 offset,
            /* [in] */ Int64 length,
            /* [in] */ ISubtitleTrack* track)
            : mHost(host)
            , mThread(thread)
            , mFd(fd)
            , mOffset(offset)
            , mLength(length)
            , mTrack(track)
        {}

        CARAPI Run();

    private:
        CARAPI_(Int32) AddTrack();

    private:
        CMediaPlayer* mHost;
        AutoPtr<IHandlerThread> mThread;
        AutoPtr<IFileDescriptor> mFd;
        Int64 mOffset;
        Int64 mLength;
        AutoPtr<ISubtitleTrack> mTrack;
    };

public:
    CMediaPlayer();

    virtual ~CMediaPlayer();

    CAR_INTERFACE_DECL()

    CAR_OBJECT_DECL()

    /**
     * Default constructor. Consider using one of the create() methods for
     * synchronously instantiating a MediaPlayer from a Uri or resource.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances may
     * result in an exception.</p>
     */
    CARAPI constructor();

    /**
     * Create a request parcel which can be routed to the  native  media
     * player using {@link #invoke(Parcel, Parcel)}. The Parcel
     * returned has the proper InterfaceToken set. The caller should
     * not overwrite that token, i.e it can only append data to the
     * Parcel.
     *
     * @return A parcel suitable to hold a request for the  native
     * player.
     * {@hide}
     */
    CARAPI NewRequest(
        /* [out] */ IParcel** parcel);

    /**
     * Invoke a generic method on the  native  player using opaque
     * parcels for the request and reply. Both payloads' format is a
     * convention between the java caller and the native player.
     * Must be called after setDataSource to make sure a native player
     * exists.
     *
     * @param request Parcel with the data for the extension. The
     * caller must use {@link #newRequest()} to get one.
     *
     * @param reply Output parcel with the data returned by the
     * native player.
     *
     * @return The status code see utils/Errors.h
     * {@hide}
     */
    CARAPI Invoke(
        /* [in] */ IParcel* request,
        /* [in] */ IParcel* reply);

    /**
     * Sets the {@link SurfaceHolder} to use for displaying the video
     * portion of the media.
     *
     * Either a surface holder or surface must be set if a display or video sink
     * is needed.  Not calling this method or {@link #setSurface(Surface)}
     * when playing back a video will result in only the audio track being played.
     * A null surface holder or surface will result in only the audio track being
     * played.
     *
     * @param sh the SurfaceHolder to use for video display
     */
    CARAPI SetDisplay(
        /* [in] */ ISurfaceHolder* sh);

    /**
     * Sets the {@link Surface} to be used as the sink for the video portion of
     * the media. This is similar to {@link #setDisplay(SurfaceHolder)}, but
     * does not support {@link #setScreenOnWhilePlaying(boolean)}.  Setting a
     * Surface will un-set any Surface or SurfaceHolder that was previously set.
     * A null surface will result in only the audio track being played.
     *
     * If the Surface sends frames to a {@link SurfaceTexture}, the timestamps
     * returned from {@link SurfaceTexture#getTimestamp()} will have an
     * unspecified zero point.  These timestamps cannot be directly compared
     * between different media sources, different instances of the same media
     * source, or multiple runs of the same program.  The timestamp is normally
     * monotonically increasing and is unaffected by time-of-day adjustments,
     * but it is reset when the position is set.
     *
     * @param surface The {@link Surface} to be used for the video portion of
     * the media.
     */
    CARAPI SetSurface(
        /* [in] */ ISurface* surface);

    /**
     * Sets video scaling mode. To make the target video scaling mode
     * effective during playback, this method must be called after
     * data source is set. If not called, the default video
     * scaling mode is {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT}.
     *
     * <p> The supported video scaling modes are:
     * <ul>
     * <li> {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT}
     * <li> {@link #VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING}
     * </ul>
     *
     * @param mode target video scaling mode. Most be one of the supported
     * video scaling modes; otherwise, IllegalArgumentException will be thrown.
     *
     * @see MediaPlayer#VIDEO_SCALING_MODE_SCALE_TO_FIT
     * @see MediaPlayer#VIDEO_SCALING_MODE_SCALE_TO_FIT_WITH_CROPPING
     */
    CARAPI SetVideoScalingMode(
        /* [in] */ Int32 mode);

    /**
     * Convenience method to create a MediaPlayer for a given Uri.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @return a MediaPlayer object, or NULL if creation failed
     */
    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [out] */ IMediaPlayer** player);

    /**
     * Convenience method to create a MediaPlayer for a given Uri.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @param holder the SurfaceHolder to use for displaying the video
     * @return a MediaPlayer object, or NULL if creation failed
     */
    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ ISurfaceHolder* holder,
        /* [out] */ IMediaPlayer ** player);

    /**
     * Same factory method as {@link #create(Context, Uri, SurfaceHolder)} but that lets you specify
     * the audio attributes and session ID to be used by the new MediaPlayer instance.
     * @param context the Context to use
     * @param uri the Uri from which to get the datasource
     * @param holder the SurfaceHolder to use for displaying the video, may be null.
     * @param audioAttributes the {@link AudioAttributes} to be used by the media player.
     * @param audioSessionId the audio session ID to be used by the media player,
     *     see {@link AudioManager#generateAudioSessionId()} to obtain a new session.
     * @return a MediaPlayer object, or null if creation failed
     */
    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ ISurfaceHolder* holder,
        /* [in] */ IAudioAttributes* audioAttributes,
        /* [in] */ Int32 audioSessionId,
        /* [out] */ IMediaPlayer** result);

    /**
     * Convenience method to create a MediaPlayer for a given resource id.
     * On success, {@link #prepare()} will already have been called and must not be called again.
     * <p>When done with the MediaPlayer, you should call  {@link #release()},
     * to free the resources. If not released, too many MediaPlayer instances will
     * result in an exception.</p>
     *
     * @param context the Context to use
     * @param resid the raw resource id (<var>R.raw.&lt;something></var>) for
     *              the resource to use as the datasource
     * @return a MediaPlayer object, or NULL if creation failed
     */
    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid,
        /* [out] */ IMediaPlayer** player);

    /**
     * Same factory method as {@link #create(Context, int)} but that lets you specify the audio
     * attributes and session ID to be used by the new MediaPlayer instance.
     * @param context the Context to use
     * @param resid the raw resource id (<var>R.raw.&lt;something></var>) for
     *              the resource to use as the datasource
     * @param audioAttributes the {@link AudioAttributes} to be used by the media player.
     * @param audioSessionId the audio session ID to be used by the media player,
     *     see {@link AudioManager#generateAudioSessionId()} to obtain a new session.
     * @return a MediaPlayer object, or null if creation failed
     */
    static CARAPI Create(
        /* [in] */ IContext* context,
        /* [in] */ Int32 resid,
        /* [in] */ IAudioAttributes* audioAttributes,
        /* [in] */ Int32 audioSessionId,
        /* [out] */ IMediaPlayer** result);

    /**
     * Sets the data source as a content Uri.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri the Content URI of the data you want to play
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI SetDataSource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri);

    /**
     * Sets the data source as a content Uri.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri the Content URI of the data you want to play
     * @param headers the headers to be sent together with the request for the data
     * @throws IllegalStateException if it is called in an invalid state
     * @hide pending API council
     */
    CARAPI SetDataSource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ IMap* headers);

    /**
     * Sets the data source (file-path or http/rtsp URL) to use.
     *
     * @param path the path of the file, or the http/rtsp URL of the stream you want to play
     * @throws IllegalStateException if it is called in an invalid state
     *
     * <p>When <code>path</code> refers to a local file, the file may actually be opened by a
     * process other than the calling application.  This implies that the pathname
     * should be an absolute path (as any other process runs with unspecified current working
     * directory), and that the pathname should reference a world-readable file.
     * As an alternative, the application could first open the file for reading,
     * and then use the file descriptor form {@link #setDataSource(FileDescriptor)}.
     */
    CARAPI SetDataSource(
        /* [in] */ const String& path);

    /**
     * Sets the data source (file-path or http/rtsp URL) to use.
     *
     * @param path the path of the file, or the http/rtsp URL of the stream you want to play
     * @param headers the headers associated with the http request for the stream you want to play
     * @throws IllegalStateException if it is called in an invalid state
     * @hide pending API council
     */
    CARAPI SetDataSource(
        /* [in] */ const String& path,
        /* [in] */ IMap* headers);

    /**
     * Sets the data source (FileDescriptor) to use. It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd);

    /**
     * Sets the data source (FileDescriptor) to use.  The FileDescriptor must be
     * seekable (N.B. a LocalSocket is not seekable). It is the caller's responsibility
     * to close the file descriptor. It is safe to do so as soon as this call returns.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param offset the offset into the file where the data to be played starts, in bytes
     * @param length the length in bytes of the data to be played
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI SetDataSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    /**
     * Prepares the player for playback, synchronously.
     *
     * After setting the datasource and the display surface, you need to either
     * call prepare() or prepareAsync(). For files, it is OK to call prepare(),
     * which blocks until MediaPlayer is ready for playback.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI Prepare();

    /**
     * Prepares the player for playback, asynchronously.
     *
     * After setting the datasource and the display surface, you need to either
     * call prepare() or prepareAsync(). For streams, you should call prepareAsync(),
     * which returns immediately, rather than blocking until enough data has been
     * buffered.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI PrepareAsync();

    /**
     * Starts or resumes playback. If playback had previously been paused,
     * playback will continue from where it was paused. If playback had
     * been stopped, or never started before, playback will start at the
     * beginning.
     *
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI Start();

    /**
     * Stops playback after playback has been stopped or paused.
     *
     * @throws IllegalStateException if the internal player engine has not been
     * initialized.
     */
    CARAPI Stop();

    /**
     * Pauses playback. Call start() to resume.
     *
     * @throws IllegalStateException if the internal player engine has not been
     * initialized.
     */
    CARAPI Pause();

    /**
     * Set the low-level power management behavior for this MediaPlayer.  This
     * can be used when the MediaPlayer is not playing through a SurfaceHolder
     * set with {@link #setDisplay(SurfaceHolder)} and thus can use the
     * high-level {@link #setScreenOnWhilePlaying(Boolean)} feature.
     *
     * <p>This function has the MediaPlayer access the low-level power manager
     * service to control the device's power usage while playing is occurring.
     * The parameter is a combination of {@link android.os.PowerManager} wake flags.
     * Use of this method requires {@link android.Manifest.permission#WAKE_LOCK}
     * permission.
     * By default, no attempt is made to keep the device awake during playback.
     *
     * @param context the Context to use
     * @param mode    the power/wake mode to set
     * @see android.os.PowerManager
     */
    CARAPI SetWakeMode(
        /* [in] */ IContext* context,
        /* [in] */ Int32 mode);

    /**
     * Control whether we should use the attached SurfaceHolder to keep the
     * screen on while video playback is occurring.  This is the preferred
     * method over {@link #setWakeMode} where possible, since it doesn't
     * require that the application have permission for low-level wake lock
     * access.
     *
     * @param screenOn Supply TRUE to keep the screen on, FALSE to allow it
     * to turn off.
     */
    CARAPI SetScreenOnWhilePlaying(
        /* [in] */ Boolean screenOn);

    /**
     * Returns the width of the video.
     *
     * @return the width of the video, or 0 if there is no video,
     * no display surface was set, or the width has not been determined
     * yet. The OnVideoSizeChangedListener can be registered via
     * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
     * to provide a notification when the width is available.
     */
    CARAPI GetVideoWidth(
        /* [out] */ Int32* width);

    /**
     * Returns the height of the video.
     *
     * @return the height of the video, or 0 if there is no video,
     * no display surface was set, or the height has not been determined
     * yet. The OnVideoSizeChangedListener can be registered via
     * {@link #setOnVideoSizeChangedListener(OnVideoSizeChangedListener)}
     * to provide a notification when the height is available.
     */
    CARAPI GetVideoHeight(
        /* [out] */ Int32* height);

    /**
     * Checks whether the MediaPlayer is playing.
     *
     * @return true if currently playing, false otherwise
     */
    CARAPI IsPlaying(
        /* [out] */ Boolean* isPlaying);

    /**
     * Seeks to specified time position.
     *
     * @param msec the offset in milliseconds from the start to seek to
     * @throws IllegalStateException if the internal player engine has not been
     * initialized
     */
    CARAPI SeekTo(
        /* [in] */ Int32 msec);

    /**
     * Gets the current playback position.
     *
     * @return the current position in milliseconds
     */
    CARAPI GetCurrentPosition(
        /* [out] */ Int32* position);

    /**
     * Gets the duration of the file.
     *
     * @return the duration in milliseconds
     */
    CARAPI GetDuration(
        /* [out] */ Int32* duration);

    /**
     * Gets the media metadata.
     *
     * @param update_only controls whether the full set of available
     * metadata is returned or just the set that changed since the
     * last call. See {@see #METADATA_UPDATE_ONLY} and {@see
     * #METADATA_ALL}.
     *
     * @param apply_filter if TRUE only metadata that matches the
     * filter is returned. See {@see #APPLY_METADATA_FILTER} and {@see
     * #BYPASS_METADATA_FILTER}.
     *
     * @return The metadata, possibly empty. NULL if an error occured.
     // FIXME: unhide.
     * {@hide}
     */
    CARAPI GetMetadata(
        /* [in] */ Boolean update_only,
        /* [in] */ Boolean apply_filter,
        /* [out] */ IMetadata** metadata);

    /**
     * Set a filter for the metadata update notification and update
     * retrieval. The caller provides 2 set of metadata keys, allowed
     * and blocked. The blocked set always takes precedence over the
     * allowed one.
     * Metadata.MATCH_ALL and Metadata.MATCH_NONE are 2 sets available as
     * shorthands to allow/block all or no metadata.
     *
     * By default, there is no filter set.
     *
     * @param allow Is the set of metadata the client is interested
     *              in receiving new notifications for.
     * @param block Is the set of metadata the client is not interested
     *              in receiving new notifications for.
     * @return The call status code.
     *
     // FIXME: unhide.
     * {@hide}
     */
    CARAPI SetMetadataFilter(
        /* [in] */ ISet* allow,
        /* [in] */ ISet* block,
        /* [out] */ Int32* result);

    /**
     * Set the MediaPlayer to start when this MediaPlayer finishes playback
     * (i.e. reaches the end of the stream).
     * The media framework will attempt to transition from this player to
     * the next as seamlessly as possible. The next player can be set at
     * any time before completion. The next player must be prepared by the
     * app, and the application should not call start() on it.
     * The next MediaPlayer must be different from 'this'. An exception
     * will be thrown if next == this.
     * The application may call setNextMediaPlayer(null) to indicate no
     * next player should be started at the end of playback.
     * If the current player is looping, it will keep looping and the next
     * player will not be started.
     *
     * @param next the player to start after this one completes playback.
     *
     */
    CARAPI SetNextMediaPlayer(
        /* [in] */ IMediaPlayer* next);

    /**
     * Releases resources associated with this MediaPlayer object.
     * It is considered good practice to call this method when you're
     * done using the MediaPlayer. In particular, whenever an Activity
     * of an application is paused (its onPause() method is called),
     * or stopped (its onStop() method is called), this method should be
     * invoked to release the MediaPlayer object, unless the application
     * has a special need to keep the object around. In addition to
     * unnecessary resources (such as memory and instances of codecs)
     * being held, failure to call this method immediately if a
     * MediaPlayer object is no longer needed may also lead to
     * continuous battery consumption for mobile devices, and playback
     * failure for other applications if no multiple instances of the
     * same codec are supported on a device. Even if multiple instances
     * of the same codec are supported, some performance degradation
     * may be expected when unnecessary multiple instances are used
     * at the same time.
    */
    CARAPI ReleaseResources();

    /**
     * Resets the MediaPlayer to its uninitialized state. After calling
     * this method, you will have to initialize it again by setting the
     * data source and calling prepare().
     */
    CARAPI Reset();

    /**
     * Sets the audio stream type for this MediaPlayer. See {@link AudioManager}
     * for a list of stream types. Must call this method before prepare() or
     * prepareAsync() in order for the target stream type to become effective
     * thereafter.
     *
     * @param streamtype the audio stream type
     * @see android.media.AudioManager
     */
    CARAPI SetAudioStreamType(
        /* [in] */ Int32 streamtype);

    /**
     * Sets the audio attributes for this MediaPlayer.
     * See {@link AudioAttributes} for how to build and configure an instance of this class.
     * You must call this method before {@link #prepare()} or {@link #prepareAsync()} in order
     * for the audio attributes to become effective thereafter.
     * @param attributes a non-null set of audio attributes
     */
    CARAPI SetAudioAttributes(
        /* [in] */ IAudioAttributes* attributes);

    /**
     * Sets the player to be looping or non-looping.
     *
     * @param looping whether to loop or not
     */
    CARAPI SetLooping(
        /* [in] */ Boolean looping);

    /**
     * Checks whether the MediaPlayer is looping or non-looping.
     *
     * @return true if the MediaPlayer is currently looping, false otherwise
     */
    CARAPI IsLooping(
        /* [out] */ Boolean* isLooping);

    /**
     * Sets the volume on this player.
     * This API is recommended for balancing the output of audio streams
     * within an application. Unless you are writing an application to
     * control user settings, this API should be used in preference to
     * {@link AudioManager#setStreamVolume(int, int, int)} which sets the volume of ALL streams of
     * a particular type. Note that the passed volume values are raw scalars.
     * UI controls should be scaled logarithmically.
     *
     * @param leftVolume left volume scalar
     * @param rightVolume right volume scalar
     */
    CARAPI SetVolume(
        /* [in] */ Float leftVolume,
        /* [in] */ Float rightVolume);

    /**
     * Similar, excepts sets volume of all channels to same value.
     * @hide
     */
    CARAPI SetVolume(
        /* [in] */ Float volume);

    /**
     * Sets the audio session ID.
     *
     * @param sessionId the audio session ID.
     * The audio session ID is a system wide unique identifier for the audio stream played by
     * this MediaPlayer instance.
     * The primary use of the audio session ID  is to associate audio effects to a particular
     * instance of MediaPlayer: if an audio session ID is provided when creating an audio effect,
     * this effect will be applied only to the audio content of media players within the same
     * audio session and not to the output mix.
     * When created, a MediaPlayer instance automatically generates its own audio session ID.
     * However, it is possible to force this player to be part of an already existing audio session
     * by calling this method.
     * This method must be called before one of the overloaded <code> setDataSource </code> methods.
     * @throws IllegalStateException if it is called in an invalid state
     */
    CARAPI SetAudioSessionId(
        /* [in] */ Int32 sessionId);

    /**
     * Returns the audio session ID.
     *
     * @return the audio session ID. {@see #setAudioSessionId(int)}
     * Note that the audio session ID is 0 only if a problem occured when the MediaPlayer was contructed.
     */
    CARAPI GetAudioSessionId(
        /* [out] */ Int32* sessionID);

    /**
     * Attaches an auxiliary effect to the player. A typical auxiliary effect is a reverberation
     * effect which can be applied on any sound source that directs a certain amount of its
     * energy to this effect. This amount is defined by setAuxEffectSendLevel().
     * {@see #setAuxEffectSendLevel(float)}.
     * <p>After creating an auxiliary effect (e.g.
     * {@link android.media.audiofx.EnvironmentalReverb}), retrieve its ID with
     * {@link android.media.audiofx.AudioEffect#getId()} and use it when calling this method
     * to attach the player to the effect.
     * <p>To detach the effect from the player, call this method with a null effect id.
     * <p>This method must be called after one of the overloaded <code> setDataSource </code>
     * methods.
     * @param effectId system wide unique id of the effect to attach
     */
    CARAPI AttachAuxEffect(
        /* [in] */ Int32 effectId);

    /**
     * Sets the send level of the player to the attached auxiliary effect
     * {@see #attachAuxEffect(int)}. The level value range is 0 to 1.0.
     * <p>By default the send level is 0, so even if an effect is attached to the player
     * this method must be called for the effect to be applied.
     * <p>Note that the passed level value is a raw scalar. UI controls should be scaled
     * logarithmically: the gain applied by audio framework ranges from -72dB to 0dB,
     * so an appropriate conversion from linear UI input x to level is:
     * x == 0 -> level = 0
     * 0 < x <= R -> level = 10^(72*(x-R)/20/R)
     * @param level send level scalar
     */
    CARAPI SetAuxEffectSendLevel(
        /* [in] */ Float level);

    /**
     * Returns an array of track information.
     *
     * @return Array of track info. The total number of tracks is the array length.
     * Must be called again if an external timed text source has been added after any of the
     * addTimedTextSource methods are called.
     * @throws IllegalStateException if it is called in an invalid state.
     */
    CARAPI GetTrackInfo(
        /* [out, callee] */ ArrayOf<IMediaPlayerTrackInfo*>** result);

    /** @hide */
    CARAPI SetSubtitleAnchor(
            /* [in] */ ISubtitleController* controller,
            /* [in] */ ISubtitleControllerAnchor* anchor);

    CARAPI OnSubtitleTrackSelected(
        /* [in] */ ISubtitleTrack* track);

    /** @hide */
    CARAPI AddSubtitleSource(
        /* [in] */ IInputStream* is,
        /* [in] */ IMediaFormat* format);

    /* TODO: Limit the total number of external timed text source to a reasonable number.
     */
    /**
     * Adds an external timed text source file.
     *
     * Currently supported format is SubRip with the file extension .srt, case insensitive.
     * Note that a single external timed text source may contain multiple tracks in it.
     * One can find the total number of available tracks using {@link #getTrackInfo()} to see what
     * additional tracks become available after this method call.
     *
     * @param path The file path of external timed text source file.
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IOException if the file cannot be accessed or is corrupted.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    CARAPI AddTimedTextSource(
        /* [in] */ const String& path,
        /* [in] */ const String& mimeType);

    /**
     * Adds an external timed text source file (Uri).
     *
     * Currently supported format is SubRip with the file extension .srt, case insensitive.
     * Note that a single external timed text source may contain multiple tracks in it.
     * One can find the total number of available tracks using {@link #getTrackInfo()} to see what
     * additional tracks become available after this method call.
     *
     * @param context the Context to use when resolving the Uri
     * @param uri the Content URI of the data you want to play
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IOException if the file cannot be accessed or is corrupted.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    CARAPI AddTimedTextSource(
        /* [in] */ IContext* context,
        /* [in] */ IUri* uri,
        /* [in] */ const String& mimeType);

    /**
     * Adds an external timed text source file (FileDescriptor).
     *
     * It is the caller's responsibility to close the file descriptor.
     * It is safe to do so as soon as this call returns.
     *
     * Currently supported format is SubRip. Note that a single external timed text source may
     * contain multiple tracks in it. One can find the total number of available tracks
     * using {@link #getTrackInfo()} to see what additional tracks become available
     * after this method call.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    CARAPI AddTimedTextSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ const String& mimeType);

    /**
     * Adds an external timed text file (FileDescriptor).
     *
     * It is the caller's responsibility to close the file descriptor.
     * It is safe to do so as soon as this call returns.
     *
     * Currently supported format is SubRip. Note that a single external timed text source may
     * contain multiple tracks in it. One can find the total number of available tracks
     * using {@link #getTrackInfo()} to see what additional tracks become available
     * after this method call.
     *
     * @param fd the FileDescriptor for the file you want to play
     * @param offset the offset into the file where the data to be played starts, in bytes
     * @param length the length in bytes of the data to be played
     * @param mimeType The mime type of the file. Must be one of the mime types listed above.
     * @throws IllegalArgumentException if the mimeType is not supported.
     * @throws IllegalStateException if called in an invalid state.
     */
    CARAPI AddTimedTextSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length,
        /* [in] */ const String& mimeType);

    /**
     * Returns the index of the audio, video, or subtitle track currently selected for playback,
     * The return value is an index into the array returned by {@link #getTrackInfo()}, and can
     * be used in calls to {@link #selectTrack(int)} or {@link #deselectTrack(int)}.
     *
     * @param trackType should be one of {@link TrackInfo#MEDIA_TRACK_TYPE_VIDEO},
     * {@link TrackInfo#MEDIA_TRACK_TYPE_AUDIO}, or
     * {@link TrackInfo#MEDIA_TRACK_TYPE_SUBTITLE}
     * @return index of the audio, video, or subtitle track currently selected for playback;
     * a negative integer is returned when there is no selected track for {@code trackType} or
     * when {@code trackType} is not one of audio, video, or subtitle.
     * @throws IllegalStateException if called after {@link #release()}
     *
     * @see {@link #getTrackInfo()}
     * @see {@link #selectTrack(int)}
     * @see {@link #deselectTrack(int)}
     */
    CARAPI GetSelectedTrack(
        /* [in] */ Int32 trackType,
        /* [out] */ Int32* result);

    /**
     * Selects a track.
     * <p>
     * If a MediaPlayer is in invalid state, it throws an IllegalStateException exception.
     * If a MediaPlayer is in <em>Started</em> state, the selected track is presented immediately.
     * If a MediaPlayer is not in Started state, it just marks the track to be played.
     * </p>
     * <p>
     * In any valid state, if it is called multiple times on the same type of track (ie. Video,
     * Audio, Timed Text), the most recent one will be chosen.
     * </p>
     * <p>
     * The first audio and video tracks are selected by default if available, even though
     * this method is not called. However, no timed text track will be selected until
     * this function is called.
     * </p>
     * <p>
     * Currently, only timed text tracks or audio tracks can be selected via this method.
     * In addition, the support for selecting an audio track at runtime is pretty limited
     * in that an audio track can only be selected in the <em>Prepared</em> state.
     * </p>
     * @param index the index of the track to be selected. The valid range of the index
     * is 0..total number of track - 1. The total number of tracks as well as the type of
     * each individual track can be found by calling {@link #getTrackInfo()} method.
     * @throws IllegalStateException if called in an invalid state.
     *
     * @see android.media.MediaPlayer#getTrackInfo
     */
    CARAPI SelectTrack(
        /* [in] */ Int32 index);

    /**
     * Deselect a track.
     * <p>
     * Currently, the track must be a timed text track and no audio or video tracks can be
     * deselected. If the timed text track identified by index has not been
     * selected before, it throws an exception.
     * </p>
     * @param index the index of the track to be deselected. The valid range of the index
     * is 0..total number of tracks - 1. The total number of tracks as well as the type of
     * each individual track can be found by calling {@link #getTrackInfo()} method.
     * @throws IllegalStateException if called in an invalid state.
     *
     * @see android.media.MediaPlayer#getTrackInfo
     */
    CARAPI DeselectTrack(
        /* [in] */ Int32 index);

    /**
     * @param reply Parcel with audio/video duration info for battery
                    tracking usage
     * @return The status code.
     * {@hide}
     */
    CARAPI NativePullBatteryData(
        /* [in] */ IParcel* reply,
        /* [out] */ Int32* result);

    /**
     * Sets the target UDP re-transmit endpoint for the low level player.
     * Generally, the address portion of the endpoint is an IP multicast
     * address, although a unicast address would be equally valid.  When a valid
     * retransmit endpoint has been set, the media player will not decode and
     * render the media presentation locally.  Instead, the player will attempt
     * to re-multiplex its media data using the Android@Home RTP profile and
     * re-transmit to the target endpoint.  Receiver devices (which may be
     * either the same as the transmitting device or different devices) may
     * instantiate, prepare, and start a receiver player using a setDataSource
     * URL of the form...
     *
     * aahRX://&lt;multicastIP&gt;:&lt;port&gt;
     *
     * to receive, decode and render the re-transmitted content.
     *
     * setRetransmitEndpoint may only be called before setDataSource has been
     * called; while the player is in the Idle state.
     *
     * @param endpoint the address and UDP port of the re-transmission target or
     * null if no re-transmission is to be performed.
     * @throws IllegalStateException if it is called in an invalid state
     * @throws IllegalArgumentException if the retransmit endpoint is supplied,
     * but invalid.
     *
     * {@hide} pending API council
     */
    CARAPI SetRetransmitEndpoint(
        /* [in] */ IInetSocketAddress* endpoint);

    /** @hide */
    CARAPI GetMediaTimeProvider(
        /* [out] */ IMediaTimeProvider** result);

    /**
     * Register a callback to be invoked when the media source is ready
     * for playback.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnPreparedListener(
        /* [in] */ IMediaPlayerOnPreparedListener* listener);

    /**
     * Register a callback to be invoked when the end of a media source
     * has been reached during playback.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnCompletionListener(
        /* [in] */ IMediaPlayerOnCompletionListener* listener);

    /**
     * Register a callback to be invoked when the status of a network
     * stream's buffer has changed.
     *
     * @param listener the callback that will be run.
     */
    CARAPI SetOnBufferingUpdateListener(
        /* [in] */ IMediaPlayerOnBufferingUpdateListener* listener);

    /**
     * Register a callback to be invoked when a seek operation has been
     * completed.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnSeekCompleteListener(
        /* [in] */ IMediaPlayerOnSeekCompleteListener* listener);

    /**
     * Register a callback to be invoked when the video size is
     * known or updated.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnVideoSizeChangedListener(
        /* [in] */ IMediaPlayerOnVideoSizeChangedListener* listener);

    /**
     * Register a callback to be invoked when a timed text is available
     * for display.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnTimedTextListener(
        /* [in] */ IMediaPlayerOnTimedTextListener* listener);

    /**
     * Register a callback to be invoked when a track has data available.
     *
     * @param listener the callback that will be run
     *
     * @hide
     */
    CARAPI SetOnSubtitleDataListener(
        /* [in] */ IMediaPlayerOnSubtitleDataListener* listener);

    /**
     * Register a callback to be invoked when an info/warning is available.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnInfoListener(
        /* [in] */ IMediaPlayerOnInfoListener* listener);

    /**
     * Register a callback to be invoked when an error has happened
     * during an asynchronous operation.
     *
     * @param listener the callback that will be run
     */
    CARAPI SetOnErrorListener(
        /* [in] */ IMediaPlayerOnErrorListener* listener);

    /**
     * Called from  native  code when an interesting event happens.  This method
     * just uses the EventHandler system to post the event back to the main app thread.
     * We use a weak reference to the original MediaPlayer object so that the native
     * code is safe from the object disappearing from underneath it.  (This is
     * the cookie passed to native_setup().)
     */
    static CARAPI PostEventFromNative(
        /* [in] */ IWeakReference* mediaplayer_ref,
        /* [in] */ Int32 what,
        /* [in] */ Int32 arg1,
        /* [in] */ Int32 arg2,
        /* [in] */ IInterface* obj);

private:
    CARAPI DecVideoSurfaceRef();

    CARAPI SetVideoSurface(
        /* [in] */ ISurface* mSurface,
        /* [in] */ Boolean mediaPlayerMustBeAlive);

    /*
     * Update the MediaPlayer SurfaceTexture.
     * Call after setting a new display surface.
     */
    CARAPI NativeSetVideoSurface(
        /* [in] */ ISurface* surface);

    CARAPI SetDataSource(
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values);

    CARAPI NativeSetDataSource(
        /* [in] */ IBinder* httpServiceBinder,
        /* [in] */ const String& path,
        /* [in] */ ArrayOf<String>* keys,
        /* [in] */ ArrayOf<String>* values);

    CARAPI NativeSetDataSource(
        /* [in] */ IFileDescriptor* fd,
        /* [in] */ Int64 offset,
        /* [in] */ Int64 length);

    CARAPI NativePrepare();

    CARAPI NativeStart();

    CARAPI_(Boolean) IsRestricted();

    CARAPI_(Int32) GetAudioStreamType();

    CARAPI_(Int32) NativeGetAudioStreamType();

    CARAPI NativeStop();

    CARAPI NativePause();

    CARAPI StayAwake(
        /* [in] */ Boolean awake);

    CARAPI UpdateSurfaceScreenOn();

    CARAPI NativeRelease();

    CARAPI NativeReset();

    CARAPI NativeSetAudioStreamType(
        /* [in] */ Int32 streamtype);

    /**
     * Sets the parameter indicated by key.
     * @param key key indicates the parameter to be set.
     * @param value value of the parameter to be set.
     * @return true if the parameter is set successfully, false otherwise
     * {@hide}
     */
    CARAPI NativeSetParameter(
        /* [in] */ Int32 key,
        /* [in] */ IParcel* value);

    CARAPI NativeSetVolume(
        /* [in] */ Float leftVolume,
        /* [in] */ Float rightVolume);

    CARAPI NativeSetAuxEffectSendLevel(
        /* [in] */ Float level);

    /*
     * Gets the value of the parameter indicated by key.
     * @param key key indicates the parameter to get.
     * @param reply value of the parameter to get.
     */
    CARAPI GetParameter(
        /* [in] */ Int32 key,
        /* [in] */ IParcel* reply);

    /**
     * @param request Parcel destinated to the media player. The
     *                Interface token must be set to the IMediaPlayer
     *                one to be routed correctly through the system.
     * @param reply[out] Parcel that will contain the reply.
     * @return The status code.
     */
    CARAPI NativeInvoke(
        /* [in] */ IParcel* request,
        /* [in] */ IParcel* reply,
        /* [out] */ Int32* result);

    /**
     * @param update_only If TRUE fetch only the set of metadata that have
     *                    changed since the last invocation of getMetadata.
     *                    The set is built using the unfiltered
     *                    notifications the native player sent to the
     *                    MediaPlayerService during that period of
     *                    time. If FALSE, all the metadatas are considered.
     * @param apply_filter  If TRUE, once the metadata set has been built based on
     *                     the value update_only, the current filter is applied.
     * @param reply[out] On return contains the serialized
     *                   metadata. Valid only if the call was successful.
     * @return The status code.
     */
    CARAPI NativeGetMetadata(
        /* [in] */ Boolean update_only,
        /* [in] */ Boolean apply_filter,
        /* [in] */ IParcel* reply,
        /* [out] */ Boolean* succeeded);

    /**
     * @param request Parcel with the 2 serialized lists of allowed
     *                metadata types followed by the one to be
     *                dropped. Each list starts with an integer
     *                indicating the number of metadata type elements.
     * @return The status code.
     */
    CARAPI NativeSetMetadataFilter(
        /* [in] */ IParcel* request);

    CARAPI NativeSetup();

    CARAPI NativeFinalize();

    CARAPI GetInbandTrackInfo(
        /* [out, callee] */ ArrayOf<IMediaPlayerTrackInfo*>** result);

    /*
     * A helper function to check if the mime type is supported by media framework.
     */
    static CARAPI_(Boolean) AvailableMimeTypeForExternalSource(
        /* [in] */ const String& mimeType);

    CARAPI_(void) ScanInternalSubtitleTracks();

    CARAPI SelectOrDeselectTrack(
        /* [in] */ Int32 index,
        /* [in] */ Boolean select);

    CARAPI SelectOrDeselectInbandTrack(
        /* [in] */ Int32 index,
        /* [in] */ Boolean select);

    CARAPI NativeSetRetransmitEndpoint(
        /* [in] */ const String& addrString,
        /* [in] */ Int32 port);

    /*
     * Test whether a given video scaling mode is supported.
     */
    CARAPI_(Boolean) IsVideoScalingModeSupported(
        /* [in] */ Int32 mode);

public:
    Int64 mNativeContext; // accessed by native methods
    Int64 mNativeSurfaceTexture;  // accessed by native methods

    static const String TAG;
    static const Boolean LOGV;

private:
    // Name of the remote interface for the media player. Must be kept
    // in sync with the 2nd parameter of the IMPLEMENT_META_INTERFACE
    // macro invocation in IMediaPlayer.cpp
    static const String IMEDIA_PLAYER;

    Int32 mListenerContext; // accessed by native methods

    AutoPtr<ISurfaceHolder> mSurfaceHolder;
    AutoPtr<IHandler> mEventHandler;

    AutoPtr<IPowerManagerWakeLock> mWakeLock;
    Boolean mScreenOnWhilePlaying;
    Boolean mStayAwake;

    AutoPtr<IIAppOpsService> mAppOps;
    Int32 mStreamType;
    Int32 mUsage;

    /* Do not change these values (starting with INVOKE_ID) without updating
     * their counterparts in include/media/mediaplayer.h!
     */
    static const Int32 INVOKE_ID_GET_TRACK_INFO;
    static const Int32 INVOKE_ID_ADD_EXTERNAL_SOURCE;
    static const Int32 INVOKE_ID_ADD_EXTERNAL_SOURCE_FD;
    static const Int32 INVOKE_ID_SELECT_TRACK;
    static const Int32 INVOKE_ID_DESELECT_TRACK;
    static const Int32 INVOKE_ID_SET_VIDEO_SCALE_MODE;
    static const Int32 INVOKE_ID_GET_SELECTED_TRACK;

    /* Do not change these values without updating their counterparts
     * in include/media/mediaplayer.h!
     */
    static const Int32 MEDIA_NOP;
    static const Int32 MEDIA_PREPARED;
    static const Int32 MEDIA_PLAYBACK_COMPLETE;
    static const Int32 MEDIA_BUFFERING_UPDATE;
    static const Int32 MEDIA_SEEK_COMPLETE;
    static const Int32 MEDIA_SET_VIDEO_SIZE;
    static const Int32 MEDIA_STARTED;
    static const Int32 MEDIA_PAUSED;
    static const Int32 MEDIA_STOPPED;
    static const Int32 MEDIA_SKIPPED;
    static const Int32 MEDIA_TIMED_TEXT;
    static const Int32 MEDIA_ERROR;
    static const Int32 MEDIA_INFO;
    static const Int32 MEDIA_SUBTITLE_DATA;

    AutoPtr<IMediaPlayerOnPreparedListener> mOnPreparedListener;
    AutoPtr<IMediaPlayerOnCompletionListener> mOnCompletionListener;
    AutoPtr<IMediaPlayerOnBufferingUpdateListener> mOnBufferingUpdateListener;
    AutoPtr<IMediaPlayerOnSeekCompleteListener> mOnSeekCompleteListener;
    AutoPtr<IMediaPlayerOnVideoSizeChangedListener> mOnVideoSizeChangedListener;
    AutoPtr<IMediaPlayerOnErrorListener> mOnErrorListener;
    AutoPtr<IMediaPlayerOnInfoListener> mOnInfoListener;
    AutoPtr<IMediaPlayerOnTimedTextListener> mOnTimedTextListener;

    // Keep KEY_PARAMETER_* in sync with include/media/mediaplayer.h
    static const Int32 KEY_PARAMETER_AUDIO_ATTRIBUTES;

    AutoPtr<ISubtitleController> mSubtitleController;

    Object mInbandSubtitleLock;
    AutoPtr<ArrayOf<ISubtitleTrack*> > mInbandSubtitleTracks;
    Int32 mSelectedSubtitleTrackIndex;
    AutoPtr<IVector> mOutOfBandSubtitleTracks;
    AutoPtr<IVector> mOpenSubtitleSources;
    AutoPtr<MediaPlayerOnSubtitleDataListener> mSubtitleDataListener;
    AutoPtr<TimeProvider> mTimeProvider;
    AutoPtr<IMediaPlayerOnSubtitleDataListener> mOnSubtitleDataListener;
};

} // namespace Media
} // namepsace Droid
} // namespace Elastos
#endif // __ELASTOS_DROID_MEDIA_CMEDIAPLAYER_H__
