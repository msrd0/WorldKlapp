#include "klapprequesthandler.h"

#include <QDebug>
#include <QDir>
#include <QHash>
#include <QSqlError>
#include <QSqlQuery>

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define PREFIX TO_STRING(USE_HTTP_PREFIX)

KlappRequestHandler::KlappRequestHandler (const QString &configFile, const QString &sharedPath)
		: db(QSqlDatabase::addDatabase("QMYSQL"))
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
	db.setHostName(settings->value("db/host").toString());
	db.setUserName(settings->value("db/user").toString());
	db.setPassword(settings->value("db/password").toString());
	db.setDatabaseName(settings->value("db/name").toString());
	db.open();
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
	if (path.startsWith("img/team/"))
	{
		QByteArray qq = QByteArray("SELECT __team_img FROM competitors WHERE `Team-Nr`=") + path.mid(9);
		QSqlQuery q(db);
		printf("%s\n", qq.data());
		if (q.exec(qq) && q.first())
		{
			response.setHeader("Content-Type", "image/png");
			response.write(q.value("__team_img").toByteArray(), true);
			return;
		}
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
	
	if (path == "index")
	{
		QSqlQuery q(db);
		if (q.exec("SELECT * FROM competitors;") && q.first())
		{
			QHash<QString, Team> teams;
			do
			{
				Team &t = teams[q.value("Team-Name").toString()];
				t.id = q.value("Team-Nr").toInt();
				t.name = q.value("Team-Name").toString();
				t.drivers[t.driverCount] = q.value("Fahrer-Name").toString();
				t.driverCount++;
			}
			while (q.next());
			
			t.loop("team", teams.size());
			int i = 0;
			for (Team &team : teams.values())
			{
				t.setVariable("team" + QString::number(i) + ".id", QString::number(team.id));
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
			printf("Error while querying competitors table (%s)\n", qPrintable(q.lastError().text()));
		}
	}
	
	base.setVariable("body", t);
	base.setVariable("prefix", PREFIX);
	base.setVariable("path", PREFIX + path);
	response.write(base.toUtf8(), true);
}
