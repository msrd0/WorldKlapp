#include "klapprequesthandler.h"

#include <QDebug>
#include <QDir>

#include <mysql++/connection.h>
#include <mysql++/query.h>
using namespace mysqlpp;

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define PREFIX TO_STRING(USE_HTTP_PREFIX)

KlappRequestHandler::KlappRequestHandler (const QString &configFile, const QString &sharedPath)
{
	QSettings *settings = new QSettings(configFile, QSettings::IniFormat);
	QSettings *s = new QSettings(settings);
	s->beginGroup("html");
	s->setValue("path", QDir::current().absoluteFilePath(sharedPath + "/html"));
	s->setValue("suffix", ".html");
	s->setValue("encoding", "UTF-8");
	html = new TemplateCache(s);
	s = new QSettings(settings);
	s->beginGroup("static");
	s->setValue("path", QDir::current().absoluteFilePath(sharedPath));
	s->setValue("encoding", "UTF-8");
	statik = new StaticFileController(s);
	conn = new Connection(settings->value("db/name").toByteArray().data(), settings->value("db/host").toByteArray().data(), settings->value("db/user").toByteArray().data(), settings->value("db/password").toByteArray().data());
}

KlappRequestHandler::~KlappRequestHandler ()
{
	delete html;
	delete statik;
}

void KlappRequestHandler::service (HttpRequest &request, HttpResponse &response)
{
	QByteArray path = request.getPath();
	printf("%s %s %s (%s)\n", request.getIP().data(), request.getMethod().data(), path.data(), request.getHeader("User-Agent").data());
	if (path == PREFIX)
	{
		response.redirect(PREFIX "index");
		return;
	}
	if (!path.startsWith(PREFIX))
	{
	  path = path.mid(1);
	}
	else
	  path = path.mid(QByteArray(PREFIX).size());
	
	if (path.startsWith("static/"))
	{
		statik->service(request, response);
		return;
	}
	
	Template base = html->getTemplate("base");
	Q_ASSERT(!base.isEmpty());
	Template t = html->getTemplate(path);
	if (path == "base")
		t = html->getTemplate("403");
	else if (t.isEmpty())
		t = html->getTemplate("404");
	Q_ASSERT(!t.isEmpty());

	Query q = conn->query("SELECT * FROM competitors;");
	StoreQueryResult r = q.store();
	if (r)
	  {
	    t.loop("team", r.num_rows());
	    printf("competitors table contains %d rows\n", r.num_rows());
	  }
	else
	  {
	    t.loop("team", 0);
	    printf("Error while querying competitors table\n");
	  }
	
	base.setVariable("body", t);
	base.setVariable("prefix", PREFIX);
	base.setVariable("path", PREFIX + path);
	response.write(base.toUtf8(), true);
}
