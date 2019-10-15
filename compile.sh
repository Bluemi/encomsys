#!/bin/bash

# create include directory in ./build/
function mk_includes() {
	include_dir="./build/include/"
	src_dir="./src/"
	visu_dir="visualizer/"

	cd "$src_dir"

	header_files=$(find -path "./*.hpp" -not -path "./glad/*" -not -path "./images/*")

	cd ..

	directories=

	for i in $header_files
	do
		dir_name=$(dirname $i)
		printf -v directories "$directories\n$dir_name"
	done

	directories=$(echo "$directories" | sort | uniq)

	for i in $directories
	do
		mkdir -p "$include_dir$visu_dir$i"
	done

	for file in $header_files
	do
		cp "$src_dir$file" "$include_dir$visu_dir$file"
	done
}

build_directory="build"

mkdir -p $build_directory
cd $build_directory
cmake -DCMAKE_BUILD_TYPE=Release -g "Unix Makefiles" ..
make -j4

cd ..

echo ""
echo "-- creating includes"
mk_includes
