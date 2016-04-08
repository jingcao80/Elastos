/*
 * Copyright 2008, The Android Open Source Project
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "HelloPlugin.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

extern NPNetscapeFuncs*         browser;
extern ANPLogInterfaceV0        gLogI;
extern ANPCanvasInterfaceV0     gCanvasI;
extern ANPPaintInterfaceV0      gPaintI;
extern ANPAudioTrackInterfaceV0 gSoundI;
extern ANPTypefaceInterfaceV0   gTypefaceI;

static void inval(NPP instance) {
        browser->invalidaterect(instance, NULL);
}

///////////////////////////////////////////////////////////////////////////////

HelloPlugin::HelloPlugin(NPP inst) : SubPlugin(inst) {

    gLogI.log(kDebug_ANPLogType, "HelloPlugin begin to create ...");

    memset(&m_trackRect, 0, sizeof(m_trackRect));
    memset(&m_helloRect,  0, sizeof(m_helloRect));
    memset(&m_RRect,  0, sizeof(m_RRect));
    memset(&m_GRect,  0, sizeof(m_BRect));
    memset(&m_BRect,  0, sizeof(m_BRect));

    m_paintRect = gPaintI.newPaint();
    gPaintI.setFlags(m_paintRect, gPaintI.getFlags(m_paintRect) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintRect, 0xFFA8A8A8);

    //m_paintActiveRect = gPaintI.newPaint();
    //gPaintI.setFlags(m_paintActiveRect, gPaintI.getFlags(m_paintActiveRect) | kAntiAlias_ANPPaintFlag);
    //gPaintI.setColor(m_paintActiveRect, 0xFF545454);

    m_paintTrack = gPaintI.newPaint();
    gPaintI.setFlags(m_paintTrack, gPaintI.getFlags(m_paintTrack) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintTrack, 0xFF54FF9F);

    m_paintR = gPaintI.newPaint();
    gPaintI.setFlags(m_paintR, gPaintI.getFlags(m_paintR) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintR, 0xFFFF0000);
    m_paintG = gPaintI.newPaint();
    gPaintI.setFlags(m_paintG, gPaintI.getFlags(m_paintG) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintG, 0xFF00FF00);
    m_paintB = gPaintI.newPaint();
    gPaintI.setFlags(m_paintB, gPaintI.getFlags(m_paintB) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintB, 0xFF0000FF);

    m_paintText = gPaintI.newPaint();
    gPaintI.setFlags(m_paintText, gPaintI.getFlags(m_paintText) | kAntiAlias_ANPPaintFlag);
    gPaintI.setColor(m_paintText, 0xFF7D26CD);
    gPaintI.setTextSize(m_paintText, 18);

    m_activeTouch = false;
    //register for touch events
    ANPEventFlags flags = kTouch_ANPEventFlag;
    NPError err = browser->setvalue(inst, kAcceptEvents_ANPSetValue, &flags);
    if (err != NPERR_NO_ERROR) {
        gLogI.log(kError_ANPLogType, "Error selecting input events.");
    }
}

HelloPlugin::~HelloPlugin() {
    gPaintI.deletePaint(m_paintTrack);
    gPaintI.deletePaint(m_paintText);
    gPaintI.deletePaint(m_paintRect);
    //gPaintI.deletePaint(m_paintActiveRect);
    gPaintI.deletePaint(m_paintR);
    gPaintI.deletePaint(m_paintG);
    gPaintI.deletePaint(m_paintB);
}

bool HelloPlugin::supportsDrawingModel(ANPDrawingModel model) {
    return (model == kBitmap_ANPDrawingModel);
}

void HelloPlugin::drawPlugin(const ANPBitmap& bitmap, const ANPRectI& clip) {
    ANPCanvas* canvas = gCanvasI.newCanvas(&bitmap);

    ANPRectF clipR;
    clipR.left = clip.left;
    clipR.top = clip.top;
    clipR.right = clip.right;
    clipR.bottom = clip.bottom;
    gCanvasI.clipRect(canvas, &clipR);

    draw(canvas);
    gCanvasI.deleteCanvas(canvas);
}

void HelloPlugin::draw(ANPCanvas* canvas) {

    PluginObject *obj = (PluginObject*) this->inst()->pdata;

    gLogI.log(kDebug_ANPLogType, "Draw the picture");

    const int W = obj->window->width;
    const int H = obj->window->height;
    //const float trackHeight = 30;
    const float buttonWidth = (W - 40)/3;
    const float buttonHeight = 30;

    // color the plugin canvas
    gCanvasI.drawColor(canvas, 0xFFFF6347);

    // get font metrics
    ANPFontMetrics fontMetrics;
    gPaintI.getFontMetrics(m_paintText, &fontMetrics);

    // draw the track box (1 px from the edge)
    m_trackRect.left = 5;
    m_trackRect.top = 5;
    m_trackRect.right = W - 5;
    m_trackRect.bottom = H - 5;//trackHeight;
    gCanvasI.drawRect(canvas, &m_trackRect, m_paintTrack);

    // draw the play box (under track box)
    m_helloRect.left = m_trackRect.left + 5;
    m_helloRect.top = m_trackRect.top + 5;
    m_helloRect.right = m_trackRect.right - 5;
    m_helloRect.bottom = m_helloRect.top + buttonHeight;
    gCanvasI.drawRect(canvas, &m_helloRect, getPaint(&m_helloRect));
    // draw the play box (in track box)
    const char helloText[] = "Hello world";
    gCanvasI.drawText(canvas, helloText, sizeof(helloText)-1, m_helloRect.left + 15,
                      m_helloRect.top - fontMetrics.fTop, m_paintText);
    //draw R G B button.
    m_RRect.left = m_trackRect.left + 5;
    m_RRect.top = m_helloRect.bottom + 10;
    m_RRect.right = m_RRect.left + buttonWidth;
    m_RRect.bottom = m_RRect.top + buttonHeight;
    gCanvasI.drawRect(canvas, &m_RRect, m_paintR);

    m_GRect.left = m_RRect.right + 10;
    m_GRect.top = m_helloRect.bottom + 10;
    m_GRect.right = m_GRect.left + buttonWidth;
    m_GRect.bottom = m_GRect.top + buttonHeight;
    gCanvasI.drawRect(canvas, &m_GRect, m_paintG);

    m_BRect.left = m_GRect.right + 10;
    m_BRect.top = m_helloRect.bottom + 10;
    m_BRect.right = m_BRect.left + buttonWidth;
    m_BRect.bottom = m_GRect.top + buttonHeight;
    gCanvasI.drawRect(canvas, &m_BRect, m_paintB);
}

ANPPaint* HelloPlugin::getPaint(ANPRectF* input) {
    //return (input == m_activeRect) ? m_paintActiveRect : m_paintRect;
    if (m_activeRect == &m_RRect)
        return m_paintR;
    else if (m_activeRect == &m_GRect)
        return m_paintG;
    else if (m_activeRect == &m_BRect)
        return m_paintB;
    else
        return m_paintRect;
}

int16_t HelloPlugin::handleEvent(const ANPEvent* evt) {
    NPP instance = this->inst();

    gLogI.log(kError_ANPLogType, "AudioPlugin::handleEvent:%d, action:%d", evt->eventType, evt->data.touch.action);
    switch (evt->eventType) {
        case kDraw_ANPEventType:
            switch (evt->data.draw.model) {
                case kBitmap_ANPDrawingModel:
                    drawPlugin(evt->data.draw.data.bitmap, evt->data.draw.clip);
                    return 1;
                default:
                    break;   // unknown drawing model
            }

        case kTouch_ANPEventType:
            {
            int x = evt->data.touch.x;
            int y = evt->data.touch.y;
            gLogI.log(kError_ANPLogType, "kTouch_ANPEventType:x:%d, y:%d", x, y);
            if (kDown_ANPTouchAction == evt->data.touch.action) {

                m_activeTouchRect = validTouch(x,y);
                if(m_activeTouchRect) {
                    m_activeTouch = true;
                    return 1;
                }

            } else if (kUp_ANPTouchAction == evt->data.touch.action && m_activeTouch) {
                handleTouch(x, y);
                m_activeTouch = false;
                return 1;
            } else if (kCancel_ANPTouchAction == evt->data.touch.action) {
                m_activeTouch = false;
            }
            break;
        }

        case kMouse_ANPEventType:
            {
            int x = evt->data.mouse.x;
            int y = evt->data.mouse.y;
            gLogI.log(kError_ANPLogType, "kMouse_ANPEventType:x:%d, y:%d", x, y);
            if (kDown_ANPTouchAction == evt->data.touch.action) {

                m_activeTouchRect = validTouch(x,y);
                if(m_activeTouchRect) {
                    m_activeTouch = true;
                    return 1;
                }

            } else if (kUp_ANPTouchAction == evt->data.touch.action && m_activeTouch) {
                handleTouch(x, y);
                m_activeTouch = false;
                return 1;
            } else if (kCancel_ANPTouchAction == evt->data.touch.action) {
                m_activeTouch = false;
            }
            break;
        }
        case kMultiTouch_ANPEventType:
        {
            int pointerCount = evt->data.multiTouch.pointerCount;
            for (int id = 0; id < pointerCount; ++id)
            {
                int x = evt->data.multiTouch.touchPoint[id].x;
                int y = evt->data.multiTouch.touchPoint[id].y;
                gLogI.log(kError_ANPLogType, "kMultiTouch_ANPEventType:id:%d,x:%d, y:%d", id, x, y);
                if (kDown_ANPTouchAction == evt->data.multiTouch.action) {

                    m_activeTouchRect = validTouch(x,y);
                    if(m_activeTouchRect) {
                        m_activeTouch = true;
                        return 1;
                    }

                } else if (kUp_ANPTouchAction == evt->data.multiTouch.action && m_activeTouch) {
                    handleTouch(x, y);
                    m_activeTouch = false;
                    return 1;
                } else if (kCancel_ANPTouchAction == evt->data.multiTouch.action) {
                    m_activeTouch = false;
                }
            }
            break;
        }
        case kLifecycle_ANPEventType:
        {
            //redraw after lost focus
            if (kLoseFocus_ANPLifecycleAction == evt->data.lifecycle.action) {
                inval(instance);
            }
        }
        default:
            break;
    }
    return 0;   // unknown or unhandled event
}

void HelloPlugin::invalActiveRect() {

}

ANPRectF* HelloPlugin::validTouch(int x, int y) {

    if (x > m_helloRect.left && x < m_helloRect.right && y > m_helloRect.top && y < m_helloRect.bottom)
        return &m_helloRect;
    else if (x > m_RRect.left && x < m_RRect.right && y > m_RRect.top && y < m_RRect.bottom)
        return &m_RRect;
    else if (x > m_GRect.left && x < m_GRect.right && y > m_GRect.top && y < m_BRect.bottom)
        return &m_GRect;
    else if (x > m_BRect.left && x < m_BRect.right && y > m_GRect.top && y < m_BRect.bottom)
        return &m_BRect;
    else
        return NULL;
}

void HelloPlugin::handleTouch(int x, int y) {
    NPP instance = this->inst();

    // check to make sure the currentRect matches the activeRect
    ANPRectF* currentRect = validTouch(x,y);
    if (m_activeTouchRect != currentRect)
        return;

    if (currentRect == &m_helloRect) {
        gLogI.log(kDebug_ANPLogType, "----touch hello button ");
    }
    else if (currentRect == &m_RRect) {
        gLogI.log(kDebug_ANPLogType, "----touch red button ");
    }
    else if (currentRect == &m_GRect) {
        gLogI.log(kDebug_ANPLogType, "----touch green button ");
    }
    else if (currentRect == &m_BRect) {
        gLogI.log(kDebug_ANPLogType, "----touch blue button ");
    }
    else {
        gLogI.log(kDebug_ANPLogType, "---- touch x:%d, y:%d ", x, y);
        return;
    }

    // set the currentRect to be the activeRect
    m_activeRect = currentRect;
    inval(instance);
}
