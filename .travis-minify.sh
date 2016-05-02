#!/bin/bash -e

git checkout "${TRAVIS_BRANCH:master}"
if [ "${CC: -3}" == "gcc" ]; then
	for file in html/*.{js,css}; do
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
	for file in html/*.svg; do
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
	for file in html/*.min.*; do
	    target="${file%.min.*}.gz"
	    rm -f "$target" || true
	    echo "==> compressing $file to $target"
	    cat "$file" | gzip >"$target"
	    git add "$target"
	done
	git commit -m "[skip ci] update minified css & js & svg" && git push || true
fi
git checkout "${TRAVIS_COMMIT:master}" || true
