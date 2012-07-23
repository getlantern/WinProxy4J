#!/usr/bin/env bash

function die() {
  echo $*
  exit 1
}

mvn install -Dmaven.test.skip=true || die "Could not build JNI lib"

javah -verbose -classpath ./target/classes -force -d src/main/cpp org.lantern.win.WinProxy || die "Could not build header files"

#xcodebuild -project jlibtorrent.xcodeproj -target JNILib || die "Could not build xcode!!"

#cp ./build/Release/libjnltorrent.jnilib ../../lib || die "Could not copy ./build/Release/libjnltorrent.jnilib??"

echo ""
echo "******** ALL JNI BUILD PROCESSES SUCCEEDED ********"
echo ""
echo ""
