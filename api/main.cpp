#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QRegularExpression>
#include <QTextCodec>
#include <QTime>

#include <microhttpd.h>

#define MAXTEAMS 55
#define DRIVERPERTEAM 4

QByteArray teamsJson;

class driver
{
public:
	int laps = 0;
	int nr = 0;
	double avg = 0;
	QString name = "dummy";
	
	virtual QJsonObject toJson()
	{
		QJsonObject obj;
		obj.insert("laps", laps);
		obj.insert("avg", avg);
		obj.insert("nr", nr);
		obj.insert("name", name);
		return obj;
	}
};
class team : public driver
{
public:
	int rank = std::numeric_limits<int>::max();
	int currdriver = -1;
	driver drivers[DRIVERPERTEAM];
	
	virtual QJsonObject toJson()
	{
		QJsonObject obj = driver::toJson();
		obj.insert("rank", rank);
		obj.insert("currdriver", currdriver);
		std::sort(drivers, drivers+4, [](const driver &d1, const driver &d2){ return d1.nr<d2.nr; });
		QJsonArray d;
		for (int i = 0; i < DRIVERPERTEAM; i++)
			d.push_back(drivers[i].toJson());
		obj.insert("drivers", d);
		return obj;
	}
};

static QJsonArray parse_file(QFile *in)
{
	static QRegularExpression regex(
				"(?P<teamrank>\\d+)\t"
				"(?P<teamnr>\\d+)\t"
				"(?P<teamname>[^\t]+)\t"
				"(?P<teamfirstname>[^\t]*)\t"
				"(?P<teamlaps>\\d+)\t"
				"(?P<drivernr>\\d+)\t"
				"(?P<driverlastname>[^\t]+)\t"
				"(?P<driverfirstname>[^\t]*)\t"
				"(?P<laptime>[\\d:\\.]+)"
			);
	if (!regex.isValid())
	{
		fprintf(stderr, "regex error: %s\n", qPrintable(regex.errorString()));
		return QJsonArray();
	}
	
	// the input is iso-8859-15
	QTextCodec *codec = QTextCodec::codecForName("ISO-8859-15");
	
	team teams[MAXTEAMS];
	QByteArray line;
	for (uint ll = 1; !(line = in->readLine()).isEmpty(); ll++)
	{
		QString lineu = codec->toUnicode(line.trimmed());
		QRegularExpressionMatch match = regex.match(lineu);
		if (!match.hasMatch())
		{
			fprintf(stderr, "%s:%u: line doesn't match\n", qPrintable(in->fileName()), ll);
			continue;
		}
		
		uint rank = match.captured("teamrank").toUInt();
		teams[rank].rank = rank;
		teams[rank].laps++;
		teams[rank].nr = match.captured("teamnr").toInt();
		teams[rank].name = match.captured("teamname");
		long time = QTime::fromString(match.captured("laptime") + "0", "hh:mm:ss.zzz").msecsSinceStartOfDay();
		double speed = 450.0 / time * 1000.0 * 3.6;
		teams[rank].avg = (teams[rank].avg * (teams[rank].laps - 1) + speed) / teams[rank].laps;
		
		uint drivernr = match.captured("drivernr").toUInt();
		driver *drivers = teams[rank].drivers;
		for (int i = 0; i < DRIVERPERTEAM; i++)
		{
			if (drivers[i].nr == drivernr || drivers[i].nr == 0)
			{
				drivers[i].nr = drivernr;
				drivers[i].laps++;
				drivers[i].name = match.captured("driverfirstname") + " " + match.captured("driverlastname");
				drivers[i].avg = (drivers[i].avg * (drivers[i].laps - 1) + speed) / drivers[i].laps;
				teams[rank].currdriver = drivernr;
				break;
			}
		}
	}
	printf("finished parsing %s\n", qPrintable(in->fileName()));
	
	QJsonArray a;
	for (int i = 0; i < MAXTEAMS; i++)
	{
		if (teams[i].name == "dummy")
			printf("ignoring team %d\n", i);
		else
		{
			printf("adding team %d: %s\n", i, qPrintable(QJsonDocument(teams[i].toJson()).toJson(QJsonDocument::Compact)));
			a.push_back(teams[i].toJson());
		}
	}
	return a;
}

static int handle_request(void *cls,
						  MHD_Connection *connection,
						  const char *url,
						  const char *method,
						  const char *version,
						  const char *upload_data,
						  size_t *upload_data_size,
						  void **ptr)
{
	static int dummy;
	if (&dummy != *ptr)
	{
		*ptr = &dummy;
		return MHD_YES;
	}
	printf("received request for %s\n", url);
	
	QByteArray path = url;
	
	if (path == "/teams.json")
	{
		MHD_Response *response = MHD_create_response_from_buffer(teamsJson.size(), (void*)teamsJson.data(), MHD_RESPMEM_PERSISTENT);
		MHD_add_response_header(response, "Content-Type", "text/json");
		int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
		MHD_destroy_response(response);
		return ret;
	}
	
	else if (path.startsWith("/upload"))
	{
		QFile in(path.mid(7));
		QByteArray json;
		uint status;
		if (in.open(QIODevice::ReadOnly))
		{
			json = QJsonDocument(parse_file(&in)).toJson(QJsonDocument::Compact);
			status = MHD_HTTP_OK;
		}
		else
		{
			QJsonObject obj;
			obj.insert("error", in.errorString());
			json = QJsonDocument(obj).toJson(QJsonDocument::Compact);
			status = MHD_HTTP_NOT_FOUND;
		}
		// TODO sync !!! (mutex ...)
		teamsJson = json;
		
		MHD_Response *response = MHD_create_response_from_buffer(json.size(), (void*)json.data(), MHD_RESPMEM_PERSISTENT);
		MHD_add_response_header(response, "Content-Type", "text/json");
		int ret = MHD_queue_response(connection, status, response);
		MHD_destroy_response(response);
		return ret;
	}
	
	MHD_Response *response = MHD_create_response_from_buffer(strlen(url), (void*)url, MHD_RESPMEM_PERSISTENT);
	int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
	MHD_destroy_response(response);
	
	return ret;
}

int main(int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("klapp-api");
	
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption portOption(QStringList() << "p" << "port", "Specify a port to use", "port", "8000");
	parser.addOption(portOption);
	parser.process(app);
	
	teamsJson = QJsonDocument(QJsonObject()).toJson(QJsonDocument::Compact);
	
	MHD_Daemon *d = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, parser.value(portOption).toShort(), NULL, NULL, &handle_request, NULL, MHD_OPTION_END);
	if (!d)
	{
		fprintf(stderr, "Failed to start daemon!\n");
		return 1;
	}
	
	return app.exec();
}
