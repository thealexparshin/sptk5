/***************************************************************************
 *                          SIMPLY POWERFUL TOOLKIT (SPTK)
 *                          CAsyncSockets.wsock.cpp  -  description
 *                             -------------------
 *    begin                : July 19 2003
 *    copyright            : (C) 2003-2011 by Alexey Parshin. All rights reserved.
 *    email                : alexeyp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * Neither the name of the <ORGANIZATION> nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ***************************************************************************/

#include <sptk4/CDateTime.h>
#include <sptk4/CThread.h>
#include <sptk4/net/CAsyncSockets.h>
#include <string.h>

using namespace std;
using namespace sptk;

#if HAVE_WSAASYNCSELECT

ATOM CAsyncSockets::m_workerWindowClass = 0;

#include <stdio.h>
#include <conio.h>

#define WM_SOCKET (WM_USER + 1)

void CAsyncSockets::processMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   if (uMsg == WM_SOCKET) {
      short events = 0;
      if (WSAGETSELECTERROR(lParam)) {
         events |= SOCKET_HANGUP;
      } else {
         switch (WSAGETSELECTEVENT(lParam)) {
            case FD_ACCEPT:
            case FD_READ:
               events |= SOCKET_READ;
                break;

            case FD_WRITE:
                events |= SOCKET_WRITE;
                break;

            case FD_CLOSE:
               events |= SOCKET_HANGUP;
               break;
         }
      }
      if (events) {
          //cout << CDateTime::Now().timeString(true,true) << " Event: " << events << endl;
          callback(wParam, events);
      }
   }
}

LRESULT CALLBACK CAsyncSockets::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CAsyncSockets::makeWorkerWindow() throw (exception)
{
    static const CHAR *ProviderClass = "AsyncSelect";
    if (m_workerWindowClass == 0) {
        WNDCLASS wndclass;
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = (WNDPROC)windowProc;
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = NULL;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = ProviderClass;

        m_workerWindowClass = RegisterClass(&wndclass);
        if (m_workerWindowClass == 0)
            throw CSystemException("RegisterClass() failed");
    }

    // Create a window
    m_workerWindow = CreateWindow(
        ProviderClass,
        "",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        NULL,
        NULL);
    if (m_workerWindow == NULL)
        throw CSystemException("CreateWindow() failed");

    m_workerThreadId = CThread::currentThreadId();
}


void CAsyncSockets::initSockets() throw (exception)
{
    makeWorkerWindow();
}

void CAsyncSockets::watchSocket(SOCKET socket, void* userData) throw (exception)
{
    Argument& socketArgument = m_socketArguments.insert(socket);
    socketArgument.m_data = userData;

    uint32_t socketEvents = 0;
    if (m_eventMask & SOCKET_READ)
        socketEvents |= FD_ACCEPT|FD_CONNECT|FD_READ;

    if (m_eventMask & SOCKET_WRITE)
        socketEvents |= FD_WRITE;

    if (m_eventMask & SOCKET_HANGUP)
        socketEvents |= FD_CLOSE;

    if (m_eventMask & SOCKET_OOB)
        socketEvents |= FD_OOB;

    // Minimizing socket triggering delays
    int flag = 1;
    setsockopt( socket, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );

    if (WSAAsyncSelect(socket, m_workerWindow, WM_SOCKET, FD_ACCEPT|FD_READ|FD_CLOSE) != 0)
        SYSTEM_EXCEPTION("Error in WSAAsyncSelect");
}

void CAsyncSockets::ignoreSocket(SOCKET socket) throw (exception)
{
    if (WSAAsyncSelect(socket, m_workerWindow, WM_SOCKET, 0))
        SYSTEM_EXCEPTION("Error in WSAAsyncSelect");
}

void CAsyncSockets::checkSockets(uint32_t timeoutMS) throw (exception)
{
    int rc;
    MSG msg;

    uint64_t threadId = CThread::currentThreadId();
    if (threadId != m_workerThreadId)
        throw CException("CAsyncSockets must be initialized from the same thread as calls to loop() or timedLoop()");

    enum TimerID {
        TM_TIMEOUT = 1,
        TM_STOP_CHECK
    };

    // If timeoutMS is defined, send WM_TIMER after timeoutMS milliseconds
    UINT_PTR timeoutTimerId = 0;
    if (timeoutMS > 0)
        timeoutTimerId = SetTimer(m_workerWindow, TM_TIMEOUT, timeoutMS, NULL);

    // Send WM_TIMER every 100 milliseconds to check for sockets stop flag
    UINT_PTR terminateTimerId = SetTimer(m_workerWindow, TM_STOP_CHECK, 100, NULL);

    while (rc = GetMessage(&msg, m_workerWindow, 0, 0)) {
        if (rc == -1)
            SYSTEM_EXCEPTION("GetMessage() failed");
        if (msg.message == WM_TIMER) {
            if (msg.wParam == TM_TIMEOUT)
                break;
            if (msg.wParam == TM_STOP_CHECK) {
                CODE_GUARD(m_socketsCS);
                if (m_stopped)
                    break;
            }
        }
        processMessage(msg.message, msg.wParam, msg.lParam);
    }

    // Kill created timers
    if (timeoutTimerId)
        KillTimer(m_workerWindow, timeoutTimerId);
    KillTimer(m_workerWindow, terminateTimerId);
}

bool CAsyncSockets::cleanupSocket(SOCKET socket, Argument& argument, void* window)
{
    HWND& workerWindow = *(HWND*) window;
    WSAAsyncSelect(socket, workerWindow, WM_SOCKET, 0);
    return true;
}

void CAsyncSockets::cleanupSockets(bool destroy)
{
    m_socketArguments.foreach(cleanupSocket, &m_workerWindow);
    m_socketArguments.clear();

    if (destroy) {
        DestroyWindow(m_workerWindow);
        m_workerWindow = NULL;
        m_initted = false;
    }
}

#endif
