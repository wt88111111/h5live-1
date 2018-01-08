#include "CWRequestHandler.h"
#include "Poco/Net/HTTPRequest.h"
#include "Poco/File.h"
#include "CWLogger.h"


using Poco::Net::HTTPRequest;
extern const std::string& getMime(std::string& suffix);
CWRequestHandler::CWRequestHandler(std::string &path):
    m_website_path(path)
{
	
	m_index_path=m_website_path+"/index.html";
	m_error_path=m_website_path+"/error.html";
	
	//m_error_path=m_website_path+"/login.html";
}


CWRequestHandler::~CWRequestHandler(void)
{
}

int CWRequestHandler::responseFile(HTTPServerResponse& response, std::string sendfilepath, std::string contentType)
{
	Poco::File file(sendfilepath);
	if(!file.exists())
	{
		cw_error("%s not exist send error.html\r\n",sendfilepath.c_str());
		response.sendFile(m_error_path,"text/html");	//请求的网页不存在，直接返回错误的页面
		return -1;
	}

	try
	{
		response.sendFile(sendfilepath,contentType);
	}

	catch (Poco::Exception* e)
	{
		cw_error("**********err %s %s\r\n",sendfilepath.c_str(),e->displayText().c_str());
	}
	
	return 0;
}

void CWRequestHandler::handleRequest(HTTPServerRequest& request, HTTPServerResponse& response)
{

	std::string url= request.getURI();
	std::string suffix = "";

	
	
	int pos = url.find_first_of("?");
    int dot = url.find_last_of(".");
	
	if(dot != url.npos )
	{
		if(pos != url.npos)
		{
            cw_info("http: URL with param\r\n");
			url.erase(pos);
		}
		suffix = url.substr(dot+1);
		//有资源文件
	}

	if("/" == url)
	{
        //app.logger().information("http: load magepage" + m_index_path);
		response.sendFile(m_index_path,"text/html");
		return;
	}
	else 
	{
		std::string mime = getMime(suffix);
        std::string sendfilepath = m_website_path + url;

        responseFile(response,sendfilepath,mime);
	}

}

