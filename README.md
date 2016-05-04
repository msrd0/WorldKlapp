# WorldKlapp [![Build Status](https://img.shields.io/travis/msrd0/WorldKlapp/master.svg)](https://travis-ci.org/msrd0/WorldKlapp) [![License](https://img.shields.io/badge/license-GPL3-blue.svg)](https://github.com/msrd0/WorldKlapp/blob/master/LICENSE)

LiveTicker für den [WorldKlapp](http://world-klapp.de).

## Usage

This project is build with CMake. To compile, download the whole repository
```bash
git clone --depth=1 https://github.com/msrd0/WorldKlapp.git
cd WorldKlapp
```
Then, build the project with CMake:
```bash
mkdir build
cd build
cmake -DCMAKE_RELEASE_TYPE=Release ..
make
```

To use the program, you need an nginx server. Add something like this to the configuration file `/etc/nginx/nginx.conf`:
```nginx
http {

  gzip  on;
  gzip_types text/plain text/css text/javascript application/javascript text/html;
  
  server {
    
    location /klapp/ {
      root /YOUR/CLONE/LOCATION/html/;
    }
    location /klapp/api/ {
      proxy_pass http://localhost:8000/;
    }
  }
}
```
Afterwards, restart nginx and start `./api/klapp-api`. Please note that you will need to upload a file before you get a non-empty web page.
