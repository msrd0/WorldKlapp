#!/bin/bash -e

git checkout travis
if [ "${CC: -3}" == "gcc" ]; then
    for file in httpd/share/static/*.{js,css}; do
	if [[ $file != *.min.* ]]; then
	    target="${file%.*}.min.${file##*.}"
	    echo "minifying $file to $target"
	    yui-compressor --type ${file##*.} -o "$target" "$file"
	    if [ "$(du "$target" | awk '{print $1}')" != 0 ]; then
		git add "$target"
	    fi
	fi
    done
    for file in httpd/share/static/*.svg; do
	if [[ $file != *.min.* ]]; then
	    target="${file%.*}.min.${file##*.}"
	    echo "minifying $file to $target"
	    svgo -o "$target" -i "$file"
	    if [ "$(du "$target" | awk '{print $1}')" != 0 ]; then
		git add "$target"
	    fi
	fi
    done
    git commit -m "[skip ci] update minified css & js & svg"
    git push
fi
