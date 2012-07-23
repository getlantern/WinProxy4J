
javac ./src/main/java/org/lastbamboo/jni/JLibTorrent.java

javah -verbose -classpath ./src/main/java/ -force -d build/headers/ org.lastbamboo.jni.JLibTorrent

devenv jlibtorrent.vcproj/jlibtorrent.sln jlibtorrent.vcproj/jlibtorrent.vcproj /build Debug

devenv jlibtorrent.vcproj/jlibtorrent.sln jlibtorrent.vcproj/jlibtorrent.vcproj /build Release
