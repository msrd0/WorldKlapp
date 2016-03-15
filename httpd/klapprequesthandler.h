#ifndef WORLDKLAPP_KLAPPREQUESTHANDLER_H
#define WORLDKLAPP_KLAPPREQUESTHANDLER_H

#include <QSqlDatabase>

#include <httprequesthandler.h>
#include <staticfilecontroller.h>
#include <templatecache.h>

class KlappRequestHandler : public HttpRequestHandler
{
public:
	KlappRequestHandler(const QString &configFile, const QString &sharedPath);
	~KlappRequestHandler ();
	
	virtual void service(HttpRequest &request, HttpResponse &response);
	
private:
	QSqlDatabase db;
	TemplateCache *html;
	StaticFileController *statik;
};

#endif //WORLDKLAPP_KLAPPREQUESTHANDLER_H
