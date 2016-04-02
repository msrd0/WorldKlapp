#!/bin/bash -e

git checkout master
if [ "${CC: -3}" == "gcc" ]; then
	for file in httpd/share/static/*.{js,css}; do
		if [[ $file != *.min.* ]]; then
			target="${file%.*}.min.${file##*.}"
			rm -f "$target" || true
			echo "==> minifying $file to $target"
			yui-compressor --type ${file##*.} -o "$target" "$file"
			if [ "$(du "$target" | awk '{print $1}')" == 0 ]; then
				echo " -> fail"
				cp "$file" "$target"
			fi
			git add "$target"
		fi
	done
	for file in httpd/share/static/*.svg; do
		if [[ $file != *.min.* ]]; then
			target="${file%.*}.min.${file##*.}"
			rm -f "$target" || true
			echo "==> minifying $file to $target"
			svgo -o "$target" -i "$file"
			if [ "$(du "$target" | awk '{print $1}')" == 0 ]; then
				echo " -> fail"
				cp "$file" "$target"
			fi
			git add "$target"
		fi
	done
	git commit -m "[skip ci] update minified css & js & svg"
	git push
fi
