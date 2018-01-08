#include "streamchannel.h"
#include "Poco/Delegate.h"
#include "CWWebSocketServer.h"
StreamChannel::StreamChannel():
    _activity(this, &StreamChannel::run)
{

}

int StreamChannel::open(int port)
{

    WebSocketSvrImpl::instance().onConnect += Poco::delegate(this, &StreamChannel::onWebSocketConnected);


    _activity.start();

    return true;
}

int StreamChannel::close()
{
    return 0;
}

int StreamChannel::push(Poco::UInt8 *data, int size,Poco::Int64 pts)
{
    _muxer.push_h264(data, size,pts);

    return 0;
}

int StreamChannel::flush()
{
    return 0;
}

void StreamChannel::run()
{

     while (!_activity.isStopped())
     {
//        Poco::Thread::sleep(1000);
        FlvTag tag;
        if(_muxer.pop_tag(tag,5000))
        {
            //printf("send size=%d\n",tag.data.size());

            WebSocketSvrImpl::instance().sendFrame("12345",(const char*)tag.data.data(),tag.data.size());
        }
        else
        {
            printf("timeout\n");
        }

     }

}
bool StreamChannel::sendFrame(Poco::Net::WebSocket* socket, const char* buffer, int sz)
{
    try
    {

        int sendLen = 0;
        do
        {
            int len = socket->sendFrame((char*)buffer + sendLen, sz - sendLen,Poco::Net::WebSocket::FRAME_BINARY);
            if (len < 0)
            {
                return false;
            }

            sendLen += len;
            if (sendLen >= sz)
                break;
        } while (1);
    }
    catch (...)
    {
        return false;
    }
    return true;
}
void StreamChannel::onWebSocketConnected(const void* pSender,Poco::Net::WebSocket*& socket)
{
    printf("websocket connected\n");
    std::vector<unsigned char> data;
    if(_muxer.get_header(data))
    {

        sendFrame(socket,(const char*)data.data(),data.size());
    }
    if(_muxer.get_ppssps(data))
    {
        sendFrame(socket,(const char*)data.data(),data.size());
    }
    std::vector<FlvTag> gops;
    if(_muxer.get_gop(gops))
    {
        for(size_t i = 0; i < gops.size(); i++)
        {
           sendFrame(socket,(const char*)gops[i].data.data(),gops[i].data.size());
        }

    }
}
