#include <iostream>
#include "pushermanager.h"
#include "opencv2/opencv.hpp"
#include "ffconfig.h"
#include "streamchannel.h"
#include "CWHttpServer.h"
#include "CWWebSocketServer.h"
using namespace std;
using namespace cv;

static bool _bExit = false;

BOOL __stdcall ConsoleCtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_BREAK_EVENT:
        _bExit = true;
        printf("ctrl+c pressed!\n");
        return TRUE;
    default:
        return FALSE;
    }
}

void waitForTerminationRequest()
{
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
}
int main()
{
    CWHttpServer srv;
    std::string root = Poco::Path::current() + "/web";

    srv.registerURI("/websocket", new Poco::Instantiator<WebSocketRequestHandler,Poco::Net::HTTPRequestHandler>());

    srv.start(root, 8000);

    PusherManager::PusherPara para;
    para._redirect = false;
    para._bitrate  = 1000000;
    para._gop = 5;
    PusherManager::get().addStream("12345","url",para);
    VideoCapture capture;
    capture.open(0);
    int h   = capture.get(CV_CAP_PROP_FRAME_HEIGHT);
    int w   = capture.get(CV_CAP_PROP_FRAME_WIDTH);
    int fps = capture.get(CV_CAP_PROP_FPS);
    printf("h5live demo start!\n");
    printf("captrue w=%d,h=%d fps=%d\n",w,h,fps);
    _bExit = false;
    while(!_bExit)
    {
        Mat frame;

        if(capture.read(frame))
        {
            VideoBuf vb;
            vb.buffer = frame.data;
            vb.w = frame.cols;
            vb.h = frame.rows;
            vb.size = vb.w*vb.h*3;
            strcpy(vb.id,"12345");
            vb.format = SS_PT_BGR;
            PusherManager::get().pushFrame(&vb);
        }
        else
        {
            cw_error("grab frame failed!\n");
        }
    }
    return 0;
}
