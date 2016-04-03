# WorldKlapp [![Build Status](https://img.shields.io/travis/msrd0/WorldKlapp/master.svg)](https://travis-ci.org/msrd0/WorldKlapp) [![License](https://img.shields.io/badge/license-GPL3-blue.svg)](https://github.com/msrd0/WorldKlapp/blob/master/LICENSE)

LiveTicker für den [WorldKlapp](http://world-klapp.de).

## Usage

This project is build with CMake. To compile, download the whole repository
```
git clone https://github.com/msrd0/WorldKlapp.git
cd WorldKlapp
git submodule init
git submodule update
```
Then, build the project with CMake:
```
mkdir build
cd build
cmake -DCMAKE_RELEASE_TYPE=Release ..
make
```
Now, edit the [httpd/httpd.ini](https://github.com/msrd0/WorldKlapp/blob/master/httpd/httpd.ini) file
to fit your needs. If your database is not filled, use ./klapp-competitors to fill it. Afterwards, you
can simply start ./klapp-httpd
