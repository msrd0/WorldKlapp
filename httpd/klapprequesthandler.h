#ifndef WORLDKLAPP_KLAPPREQUESTHANDLER_H
#define WORLDKLAPP_KLAPPREQUESTHANDLER_H

#include <QMutex>
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
	QMutex dbMutex;
	TemplateCache *html;
	StaticFileController *statik;
};

#endif //WORLDKLAPP_KLAPPREQUESTHANDLER_H
