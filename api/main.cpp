#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

#include <microhttpd.h>

QByteArray teamsJson;

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
	
	else if (path == "/update")
	{
		if (*upload_data_size == 0)
		{
			MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
			MHD_destroy_response(response);
			return ret;
		}
		else
		{
			QByteArray data(upload_data, *upload_data_size);
			qDebug() << data;
			printf("received upload with %d newlines\n", data.count('\n'));
			MHD_Response *response = MHD_create_response_from_buffer(0, NULL, MHD_RESPMEM_PERSISTENT);
			int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
			MHD_destroy_response(response);
			return ret;
		}
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
