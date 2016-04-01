#include <iostream>
using namespace std;

#include <QBuffer>
#include <QCoreApplication>
#include <QEventLoop>
#include <QFile>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

#define e(x) ((x).replace("'", "''"))

QNetworkAccessManager mgr;
inline QByteArray downloadImage (const QString &url, int size = 64)
{
	QNetworkRequest req(url);
	printf("GET %s\n", req.url().toDisplayString(QUrl::FullyDecoded).toLocal8Bit().data());
	QEventLoop loop;
	QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
	QNetworkReply *reply = mgr.get(req);
	loop.exec();
	printf("GOT %s\n", req.url().toDisplayString(QUrl::FullyDecoded).toLocal8Bit().data());
	QImage img;
	img.loadFromData(reply->readAll());
	img = img.scaled(size, size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	QByteArray imgData;
	QBuffer buf(&imgData);
	buf.open(QIODevice::WriteOnly);
	img.save(&buf, "PNG");
	return imgData;
}

int main (int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	if (argc != 6)
	{
		cerr << "Usage: " << argv[0] << " <csv> <host> <user> <password> <database>" << endl;
		return 1;
	}
	
	QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
	db.setHostName(argv[2]);
	db.setUserName(argv[3]);
	db.setPassword(argv[4]);
	db.setDatabaseName(argv[5]);
	if (!db.open())
	{
		fprintf(stderr, "Failed to connect to database (%s)\n", qPrintable(db.lastError().text()));
		return 1;
	}
	
	// maybe create competitors table
	QSqlQuery q(db);
	if (!q.exec("CREATE TABLE IF NOT EXISTS `competitors` (\n"
			            "  `uid` int(11) NOT NULL AUTO_INCREMENT,\n"
			            "  `Lastname` varchar(50) NOT NULL,\n"
			            "  `Firstname` varchar(50) NOT NULL,\n"
			            "  `Address1` varchar(50) NOT NULL,\n"
			            "  `Address2` varchar(50) NOT NULL,\n"
			            "  `ZipCode` int(11) NOT NULL DEFAULT '0',\n"
			            "  `Town` tinytext,\n"
			            "  `State` tinytext,\n"
			            "  `Country` text,\n"
			            "  `Phone` tinytext,\n"
			            "  `Sex` varchar(2) DEFAULT NULL,\n"
			            "  `Number` tinytext,\n"
			            "  `License` tinytext,\n"
			            "  `Licensetype` tinytext,\n"
			            "  `BirthYear` int(11) NOT NULL DEFAULT '0',\n"
			            "  `Category` tinytext,\n"
			            "  `CategoryName` text,\n"
			            "  `CategoryAbbrev` tinytext,\n"
			            "  `Nation` text,\n"
			            "  `Team` text,\n"
			            "  `TeamCode` tinytext,\n"
			            "  `Competition` text,\n"
			            "  `CompetitionType` text,\n"
			            "  `CompetitionTown` text,\n"
			            "  `CompetitionZipCode` text,\n"
			            "  `CompetitionDate` text,\n"
			            "  `Race` text,\n"
			            "  `Distance` text,\n"
			            "  `Time` tinytext,\n"
			            "  `RunnerSeconds` tinytext,\n"
			            "  `RoundedTime` tinytext,\n"
			            "  `RoundedSeconds` tinytext,\n"
			            "  `RoundedHours` tinytext,\n"
			            "  `Ranking` tinytext,\n"
			            "  `CategoryRanking` tinytext,\n"
			            "  `Organization` tinytext,\n"
			            "  `Paid` text,\n"
			            "  `Invited` tinytext,\n"
			            "  `MedProvided` varchar(3) NOT NULL,\n"
			            "  `MarkedStarted` tinytext,\n"
			            "  `Stopped` tinytext,\n"
			            "  `Disqualified` tinytext,\n"
			            "  `Qualified` tinytext,\n"
			            "  `SendResults` tinytext,\n"
			            "  `Handicap` tinytext,\n"
			            "  `RunnerID` int(11) NOT NULL DEFAULT '0',\n"
			            "  `Sponsor` tinytext,\n"
			            "  `Awards` tinytext,\n"
			            "  `EMail` varchar(128) NOT NULL,\n"
			            "  `U4ProChipID` varchar(10) NOT NULL,\n"
			            "  PRIMARY KEY (`uid`)\n"
			            ") ENGINE=MyISAM  DEFAULT CHARSET=utf8 AUTO_INCREMENT=15 ;"))
	{
		fprintf(stderr, "Failed to create the 'competitors' table (%s).\n", qPrintable(q.lastError().text()));
		return 1;
	}
	
	// clean content of the competitors table
	q = QSqlQuery(db);
	if (!q.exec("DELETE FROM competitors;"))
	{
		cerr << "Failed to clean the existing table!" << endl;
		return 1;
	}
	
	// insert the competitors
	QFile in(argv[1]);
	if (!in.open(QIODevice::ReadOnly))
	{
		fprintf(stderr, "Failed to open csv file\n");
		return 1;
	}
	QByteArray line = in.readLine().trimmed();
	QList<QByteArray> s = line.split(',');
	for (uint linenum = 2; !in.atEnd(); linenum++)
	{
		line = in.readLine().trimmed();
		if (line == "")
			continue;
		// Start-Nr,Fahrer-Kurzname,Team-Nr ,Team-Kurzname,Team-Name,Fahrer-Nr,Fahrer-Name,Team_id,Team-URL,Fahrer-URL,ProChip
		// 0       ,1              ,2       ,3            ,4        ,5        ,6          ,7      ,8       ,9         ,10
		// Number  ,Lastname       ,TeamCode,             ,Team     ,Address1 ,Firstname  ,       ,        ,          ,U4ProChipID
		QList<QByteArray> s0 = line.split(',');
		if (s0.size() != s.size())
		{
			cerr << "Error in line " << linenum << ": Column count differs from title!" << endl;
			continue;
		}
		QByteArray query = "INSERT INTO competitors (Lastname, Firstname, Address1, Address2, Number, Team, TeamCode, MedProvided, EMail, U4ProChipID) VALUES ("
				"'" + e(s0[1]) + "', '" + e(s0[6]) + "', '" + e(s0[5]) + "', 'Cares', '" + e(s0[0]) + "', '" + e(s0[4]) + "', '" + e(s0[2]) + "'"
				", '?', 'who@cares', '" + e(s0[10]) + "');";
		if (!q.exec(query))
		{
			cerr << "Failed to insert competitor of line " << linenum << ": " << q.lastError().text().toStdString() << endl;
			continue;
		}

	}
	
	return 0;
}
