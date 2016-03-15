#ifndef WORLDKLAPP_KLAPPREQUESTHANDLER_H
#define WORLDKLAPP_KLAPPREQUESTHANDLER_H

#include <httprequesthandler.h>
#include <staticfilecontroller.h>
#include <templatecache.h>

#include <mysql++/connection.h>

class KlappRequestHandler : public HttpRequestHandler
{
public:
	KlappRequestHandler(const QString &configFile, const QString &sharedPath);
	~KlappRequestHandler ();
	
	virtual void service(HttpRequest &request, HttpResponse &response);
	
private:
	mysqlpp::Connection conn;
	TemplateCache *html;
	StaticFileController *statik;
};

#endif //WORLDKLAPP_KLAPPREQUESTHANDLER_H
