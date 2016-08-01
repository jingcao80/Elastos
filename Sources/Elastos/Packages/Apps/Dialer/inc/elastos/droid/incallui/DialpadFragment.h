
#ifndef __ELASTOS_DROID_INCALLUI_DIALPADFRAGMENT_H__
#define __ELASTOS_DROID_INCALLUI_DIALPADFRAGMENT_H__

#include "Elastos.Droid.View.h"
#include "Elastos.Droid.Widget.h"
#include "elastos/droid/incallui/BaseFragment.h"
#include "elastos/droid/incallui/DialpadPresenter.h"
#include "elastos/droid/os/Runnable.h"
#include "elastos/droid/text/method/DialerKeyListener.h"
#include <elastos/utility/etl/HashMap.h>

using Elastos::Droid::Os::IHandler;
using Elastos::Droid::Os::Runnable;
using Elastos::Droid::Phone::Common::Dialpad::IDialpadView;
using Elastos::Droid::Text::Method::DialerKeyListener;
using Elastos::Droid::View::IView;
using Elastos::Droid::View::IViewOnTouchListener;
using Elastos::Droid::View::IViewOnKeyListener;
using Elastos::Droid::View::IViewOnHoverListener;
using Elastos::Droid::View::IViewOnClickListener;
using Elastos::Droid::View::IMotionEvent;
using Elastos::Droid::Widget::IEditText;
using Elastos::Utility::Etl::HashMap;

namespace Elastos {
namespace Droid {
namespace InCallUI {

class DialpadFragment
    : public BaseFragment
    , public IUi
    , public IDialpadUi
    , public IViewOnTouchListener
    , public IViewOnKeyListener
    , public IViewOnHoverListener
    , public IViewOnClickListener
{
private:
    /**
     * Our own key listener, specialized for dealing with DTMF codes.
     *   1. Ignore the backspace since it is irrelevant.
     *   2. Allow ONLY valid DTMF characters to generate a tone and be
     *      sent as a DTMF code.
     *   3. All other remaining characters are handled by the superclass.
     *
     * This code is purely here to handle events from the hardware keyboard
     * while the DTMF dialpad is up.
     */
    class DTMFKeyListener : public DialerKeyListener
    {
    public:
        /**
         * Overriden to return correct DTMF-dialable characters.
         */
        //@Override
        CARAPI_(AutoPtr< ArrayOf<Char32> >) GetAcceptedChars();

        /** special key listener ignores backspace. */
        // @Override
        CARAPI Backspace(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* ret);

        /**
         * Overriden so that with each valid button press, we start sending
         * a dtmf code and play a local dtmf tone.
         */
        //@Override
        CARAPI OnKeyDown(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* ret);

        /**
         * Overriden so that with each valid button up, we stop sending
         * a dtmf code and the dtmf tone.
         */
        //override
        CARAPI OnKeyUp(
            /* [in] */ IView* view,
            /* [in] */ IEditable* content,
            /* [in] */ Int32 keyCode,
            /* [in] */ IKeyEvent* event,
            /* [out] */ Boolean* ret);

        /**
         * Handle individual keydown events when we DO NOT have an Editable handy.
         */
        CARAPI_(Boolean) OnKeyDown(
            /* [in] */ IKeyEvent* event);

        /**
         * Handle individual keyup events.
         *
         * @param event is the event we are trying to stop.  If this is null,
         * then we just force-stop the last tone without checking if the event
         * is an acceptable dialer event.
         */
        CARAPI_(Boolean) OnKeyUp(
            /* [in] */ IKeyEvent* event);

        /**
         * Check to see if the keyEvent is dialable.
         */
        CARAPI_(Boolean) IsKeyEventAcceptable (
            /* [in] */ IKeyEvent* event);

    private:
        DTMFKeyListener(
            /* [in] */ DialpadFragment* host);

        /**
         * Return true if the keyCode is an accepted modifier key for the
         * dialer (ALT or SHIFT).
         */
        CARAPI_(Boolean) IsAcceptableModifierKey(
            /* [in] */ Int32 keyCode);

        using DialerKeyListener::Lookup;

        /**
         * Find the Dialer Key mapped to this event.
         *
         * @return The char value of the input event, otherwise
         * 0 if no matching character was found.
         */
        CARAPI_(Char32) Lookup(
            /* [in] */ IKeyEvent* event);

    public:
        /**
         * Overrides the characters used in {@link DialerKeyListener#CHARACTERS}
         * These are the valid dtmf characters.
         */
        static const AutoPtr<ArrayOf<Char32> > DTMF_CHARACTERS;

    private:
        DialpadFragment* mHost;
        friend class DialpadFragment;
    };

    class StopDtmfRunnable : public Runnable
    {
    public:
        StopDtmfRunnable(
            /* [in] */ DialpadPresenter* presenter)
            : mPresenter(presenter)
        {}

        CARAPI Run();

    private:
        AutoPtr<DialpadPresenter> mPresenter;
    };

    class StaticInitializer
    {
    public:
        StaticInitializer();
    };

public:
    CAR_INTERFACE_DECL()

    // @Override
    CARAPI OnClick(
        /* [in] */ IView* v);

    // @Override
    CARAPI OnHover(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* comsumed);

    // @Override
    CARAPI OnKey(
        /* [in] */ IView* v,
        /* [in] */ Int32 keyCode,
        /* [in] */ IKeyEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI OnTouch(
        /* [in] */ IView* v,
        /* [in] */ IMotionEvent* event,
        /* [out] */ Boolean* result);

    // @Override
    CARAPI_(AutoPtr<IPresenter>) CreatePresenter();

    // @Override
    CARAPI_(AutoPtr<IUi>) GetUi();

    // @Override
    CARAPI OnCreate(
        /* [in] */ IBundle* savedInstanceState);

    // @Override
    CARAPI OnCreateView(
        /* [in] */ ILayoutInflater* inflater,
        /* [in] */ IViewGroup* container,
        /* [in] */ IBundle* savedInstanceState,
        /* [out] */ IView** view);

    // @Override
    CARAPI OnDestroyView();

    /**
     * Getter for Dialpad text.
     *
     * @return String containing current Dialpad EditText text.
     */
    CARAPI_(String) GetDtmfText();

    /**
     * Sets the Dialpad text field with some text.
     *
     * @param text Text to set Dialpad EditText to.
     */
    CARAPI_(void) SetDtmfText(
        /* [in] */ const String& text);

    // @Override
    CARAPI SetVisible(
        /* [in] */ Boolean on);

    /**
     * Starts the slide up animation for the Dialpad keys when the Dialpad is revealed.
     */
    CARAPI_(void) AnimateShowDialpad();

    // @Override
    CARAPI AppendDigitsToField(
        /* [in] */ Char32 digit);

    /**
     * Called externally (from InCallScreen) to play a DTMF Tone.
     */
    CARAPI_(Boolean) OnDialerKeyDown(
        /* [in] */ IKeyEvent* event);

    /**
     * Called externally (from InCallScreen) to cancel the last DTMF Tone played.
     */
    CARAPI_(Boolean) OnDialerKeyUp(
        /* [in] */ IKeyEvent* event);

private:
    CARAPI_(void) ConfigureKeypadListeners(
        /* [in] */ IView* fragmentView);

private:
    static const Int32 ACCESSIBILITY_DTMF_STOP_DELAY_MILLIS = 50;

    AutoPtr<IEditText> mDtmfDialerField;

    /** Hash Map to map a view id to a character*/
    static HashMap<Int32, Char32> sDisplayMap;
    static StaticInitializer sInitializer;

    static const AutoPtr<IHandler> sHandler;

    // KeyListener used with the "dialpad digits" EditText widget.
    AutoPtr<DTMFKeyListener> mDialerKeyListener;

    AutoPtr<IDialpadView> mDialpadView;

};

} // namespace InCallUI
} // namespace Droid
} // namespace Elastos

DEFINE_OBJECT_HASH_FUNC_FOR(Elastos::Core::IInteger32)

#endif // __ELASTOS_DROID_INCALLUI_DIALPADFRAGMENT_H__
