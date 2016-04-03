#!/bin/bash

if [ "${CC: -3}" == "gcc" ]; then
	cd httpd
	sed -i 's/name=.*$/name='"$DBNAME"'/'             httpd.ini
	sed -i 's/user=.*$/user='"$DBUSER"'/'             httpd.ini
	sed -i 's/host=.*$/host='"$DBHOST"'/'             httpd.ini
	sed -i 's/password=.*$/password='"$DBPASSWORD"'/' httpd.ini
	LD_LIBRARY_PATH=.. ../klapp-httpd &
	pid=$!
	sleep 10 # wait for the server to start
	cd ..
	wget -O .travis-preview.html http://localhost:8080/
	kill -9 $pid
	
	test -r .git/refs/heads/preview && git checkout preview || git checkout --orphan preview
	cp .travis-preview.html index.html
	git add index.html
	git commit index.html -m "update preview" && git push origin preview || echo "failed to push"
fi