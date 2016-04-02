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
	printf("Conneting to database %s at %s (%s:%s)\n",
	       qPrintable(settings->value("db/name").toString()),
	       qPrintable(settings->value("db/host").toString()),
	       qPrintable(settings->value("db/user").toString()),
	       qPrintable(settings->value("db/password").toString()));
	db.setHostName(settings->value("db/host").toString());
	db.setUserName(settings->value("db/user").toString());
	db.setPassword(settings->value("db/password").toString());
	db.setDatabaseName(settings->value("db/name").toString());
	if (!db.open())
	  fprintf(stderr, "Failed to connect to database: %s\n", qPrintable(db.lastError().text()));
}

KlappRequestHandler::~KlappRequestHandler ()
{
	// wait for running requests
	dbMutex.lock();
	delete html;
	delete statik;
}

struct Driver
{
	int id;
	QString name;
    QString chip = "AB-1234";
    int laps = 100;
    int avg = 10, last = 9;

    Driver() : id(-1), name("") {}
    Driver(int id, const QString &name) : id(id), name(name) {}
};
struct Team
{
	int id, rank;
	QString name;
	Driver drivers[4];
	int driverCount = 0;
	int curr = 1;
    int laps = 400;
    int avg = 10, last = 9;
	
	bool operator< (const Team &other) const { return (rank < other.rank); }
};

void KlappRequestHandler::service (HttpRequest &request, HttpResponse &response)
{
	QByteArray path = request.getPath();
	printf("%s %s %s (%s)\n", request.getIP().data(), request.getMethod().data(), path.data(), request.getHeader("User-Agent").data());
	if (path == PREFIX || path == "/")
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
	
	if (path == "index")
	{
		dbMutex.lock();
		QSqlQuery q(db);
		if (q.exec("SELECT * FROM competitors;") && q.first())
		{
			QHash<QString, Team> teams;
			do
			{
				Team &t = teams[q.value("Team").toString()];
				t.id = q.value("TeamCode").toInt();
				t.rank = t.id;
				t.name = q.value("Team").toString();
				t.drivers[t.driverCount] = Driver{q.value("Address1").toInt(), q.value("Firstname").toString()};
				t.driverCount++;
			}
			while (q.next());
			
			auto teamlist = teams.values();
			std::sort(teamlist.begin(), teamlist.end());
			t.loop("team", teams.size());
			int i = 0;
			for (Team &team : teamlist)
			{
				t.setVariable("team" + QString::number(i) + ".id", QString::number(team.id));
				t.setVariable("team" + QString::number(i) + ".rank", QString::number(team.rank));
				t.setVariable("team" + QString::number(i) + ".name", team.name);
				t.setVariable("team" + QString::number(i) + ".curr", QString::number(team.curr));
				t.setVariable("team" + QString::number(i) + ".laps", QString::number(team.laps));
				t.setVariable("team" + QString::number(i) + ".avg", QString::number(team.avg));
				t.setVariable("team" + QString::number(i) + ".last", QString::number(team.last));
				t.loop("team" + QString::number(i) + ".driver", team.driverCount);
				for (int j = 0; j < team.driverCount; j++)
				{
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".id", QString::number(team.drivers[j].id));
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".name", team.drivers[j].name);
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".laps", QString::number(team.drivers[j].laps));
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".avg", QString::number(team.drivers[j].avg));
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".last", QString::number(team.drivers[j].last));
					t.setVariable("team" + QString::number(i) + ".driver" + QString::number(j) + ".chip", team.drivers[j].chip);
					t.setCondition("team" + QString::number(i) + ".driver" + QString::number(j) + ".curr", team.drivers[j].id == team.curr);
				}
				i++;
			}
		}
		else
		{
			t.loop("team", 0);
			printf("Error while querying competitors table (%s)\n", qPrintable(q.lastError().text()));
		}
		dbMutex.unlock();
	}
	
	base.setVariable("body", t);
	base.setVariable("prefix", PREFIX);
	base.setVariable("path", PREFIX + path);
	response.write(base.toUtf8(), true);
}
