#include <fstream>
#include <iostream>
using namespace std;

#include <mysql++/connection.h>
#include <mysql++/query.h>
using namespace mysqlpp;

#include <boost/algorithm/string.hpp>
using namespace boost;

#include <QEventLoop>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

int main (int argc, char **argv)
{
	if (argc != 6)
	{
		cerr << "Usage: " << argv[0] << " <csv> <host> <user> <password> <database>" << endl;
		return 1;
	}

	QNetworkAccessManager mgr;

	Connection con(false);
	con.connect(argv[5], argv[2], argv[3], argv[4]);
	if (!con.connected())
	{
		cerr << "Failed to connect to the database!" << endl;
		return 1;
	}
	
	// clean content of the competitors table
	Query q(&con);
	if (!q.exec("DROP TABLE IF EXISTS competitors;"))
	{
		cerr << "Failed to delete the existing table!" << endl;
		return 1;
	}
	
	// insert the competitors
	ifstream in(argv[1]);
	char line[8192];
	in.getline(line, 8192);
	vector<string> s;
	split(s, line, is_any_of(",;"));
	string query = "CREATE TABLE competitors (uid BIGINT UNSIGNED AUTO_INCREMENT PRIMARY KEY";
	for (auto a : s)
		query += ", `" + a + "` TEXT NOT NULL DEFAULT ''";
	query += ") DEFAULT CHARSET=utf8;";
	if (!q.exec(query))
	{
		cerr << "Failed to create table!" << endl;
		return 1;
	}
	uint uid = 0;
	for (uint linenum = 2; !in.eof(); linenum++)
	{
		in.getline(line, 8192);
		if (line[0] == 0)
		{
			continue;
		}
		vector<string> s0;
		split(s0, line, is_any_of(",;"));
		if (s0.size() != s.size())
		{
			cerr << "Error in line " << linenum << ": Column count differs from title!" << endl;
			continue;
		}
		query = "INSERT INTO competitors VALUES (" + to_string(++uid);
		for (auto a : s0)
		{
			replace_all(a, "'", "''");
			query += ", '" + a + "'"; // TODO XSS
		}
		query += ");";
		if (!q.exec(query))
		{
			cerr << "Failed to insert competitor of line " << linenum << endl;
			continue;
		}

		QNetworkRequest req(QString(s0[8].data()));
		QEventLoop loop;
		QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
		QNetworkReply *reply = mgr.get(req);
		loop.exec();
		QImage img;
		img.loadFromData(reply->readAll());
		img = img.scaled(64,64, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		img.save(QString("share/img/team") + s[2].data() + ".png", "PNG");
	}
	
	return 0;
}
