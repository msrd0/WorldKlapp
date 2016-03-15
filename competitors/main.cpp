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

int main (int argc, char **argv)
{
	QCoreApplication app(argc, argv);
	if (argc != 6)
	{
		cerr << "Usage: " << argv[0] << " <csv> <host> <user> <password> <database>" << endl;
		return 1;
	}

	QNetworkAccessManager mgr;

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
	
	// clean content of the competitors table
	QSqlQuery q(db);
	if (!q.exec("DROP TABLE IF EXISTS competitors;"))
	{
		cerr << "Failed to delete the existing table!" << endl;
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
	QString query = "CREATE TABLE competitors (uid BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY";
	for (auto a : s)
		query += ", `" + a + "` TEXT NOT NULL DEFAULT ''";
	query += ", __team_img BLOB) DEFAULT CHARSET=utf8;";
	q = QSqlQuery(db);
	if (!q.exec(query))
	{
		fprintf(stderr, "Failed to create table (%s)\n", qPrintable(q.lastError().text()));
		return 1;
	}
	uint uid = 0;
	for (uint linenum = 2; !in.atEnd(); linenum++)
	{
		line = in.readLine().trimmed();
		if (line == "")
			continue;
		QList<QByteArray> s0 = line.split(',');
		if (s0.size() != s.size())
		{
			cerr << "Error in line " << linenum << ": Column count differs from title!" << endl;
			continue;
		}
		query = "INSERT INTO competitors VALUES (" + QString::number(++uid);
		for (auto a : s0)
		{
			a.replace('\'', "''");
			query += ", '" + a + "'"; // TODO XSS
		}
		
		QNetworkRequest req(QString(s0[8].data()));
		printf("GET %s\n", req.url().toDisplayString(QUrl::FullyDecoded).toLocal8Bit().data());
		QEventLoop loop;
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
		QNetworkReply *reply = mgr.get(req);
		loop.exec();
		printf("GOT %s\n", req.url().toDisplayString(QUrl::FullyDecoded).toLocal8Bit().data());
		QImage img;
		img.loadFromData(reply->readAll());
		img = img.scaled(64,64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		QByteArray imgData;
		QBuffer buf(&imgData);
		buf.open(QIODevice::WriteOnly);
		img.save(&buf, "PNG");
		query += ", x'" + imgData.toHex().toUpper() + "');";
		
		if (!q.exec(query))
		{
			cerr << "Failed to insert competitor of line " << linenum << endl;
			continue;
		}

	}
	
	return 0;
}
