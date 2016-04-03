#!/bin/bash

if [ "${CC: -3}" == "gcc" ]; then
	cd httpd
	sed -i 's/name=.*$/name='"$DBNAME"'/'             httpd.ini
	sed -i 's/user=.*$/user='"$DBUSER"'/'             httpd.ini
	sed -i 's/host=.*$/host='"$DBHOST"'/'             httpd.ini
	sed -i 's/port=.*$/port='"$DBPORT"'/'             httpd.ini
	sed -i 's/password=.*$/password='"$DBPASSWORD"'/' httpd.ini
	LD_LIBRARY_PATH=.. ../klapp-httpd &
	pid=$!
	sleep 60 # wait for the server to start
	cd ..
	wget -O .travis-preview.html http://localhost:$DBPORT/
	kill -9 $pid
	
	test -r .git/refs/heads/preview && git checkout preview || git checkout --orphan preview
	git pull origin preview || echo "failed to pull"
	cp .travis-preview.html index.html
	sed -i 's/\/msrd0\/WorldKlapp\/master\/httpd\/share\//https:\/\/raw.githubusercontent.com\/msrd0\/WorldKlapp\/master\/httpd\/share\//'
	git add index.html
	git commit index.html -m "update preview" && git push origin preview || echo "failed to push"
fi
