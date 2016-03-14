#include "klapprequesthandler.h"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>
#include <httplistener.h>

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)
#define PREFIX TO_STRING(USE_PREFIX)

int main (int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	QCoreApplication::setApplicationName("Klapp HTTP Server Daemon");
	QCoreApplication::setApplicationVersion("0.1.0");
	
	QCommandLineParser parser;
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("config", "The configuration file.", "[<config>]");
	parser.process(app);
	QStringList args = parser.positionalArguments();
	
	QString configFile;
	if (args.size() > 0)
		configFile = args[0];
	else if (QFileInfo("/etc/klapp/httpd.ini").exists())
		configFile = "/etc/klapp/httpd.ini";
	else if (QFileInfo(PREFIX "/httpd.ini").exists())
		configFile = PREFIX "/httpd.ini";
	else
		configFile = "httpd.ini";
	printf("Using config file %s\n", qPrintable(configFile));
	
	QString sharedPath;
	if (QFileInfo(PREFIX "/share/klapp/httpd").exists())
		sharedPath = PREFIX "/share/klapp/httpd";
	else
		sharedPath = "share/";
	printf("Using shared path %s\n", qPrintable(sharedPath));
	
	KlappRequestHandler handler(configFile, sharedPath);
	HttpListener listener(new QSettings(configFile, QSettings::IniFormat), &handler, &app);
	
	return app.exec();
}
