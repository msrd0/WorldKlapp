#include "klapprequesthandler.h"

#include <QDebug>
#include <QDir>
#include <QHash>

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

struct Team
{
  int id;
  QString name;
  QString drivers[4];
  int driverCount = 0;
};

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

	response.setHeader("Content-Type", "text/html; charset=utf-8");
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
	    QHash<QString, Team> teams;
	    for (int i = 0; i < r.num_rows(); i++)
	      {
		Team &t = teams[r[i]["Team-Name"].c_str()];;
		t.id = QString(r[i]["Team-Nr"].c_str()).toInt();
		t.name = r[i]["Team-Name"].c_str();
		t.drivers[t.driverCount] = r[i]["Fahrer-Name"].c_str();
		t.driverCount++;
	      }
	    t.loop("team", teams.size());
	    int i = 0;
	    for (Team &team : teams.values())
	      {
		t.setVariable("team" + QString::number(i) + ".rank", QString::number(team.id));
		t.setVariable("team" + QString::number(i) + ".name", team.name);
		for (int j = 0; j < 4; j++)
		  t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".name", team.drivers[j]);
		i++;
	      }
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
