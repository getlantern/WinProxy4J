

.\buildJava.ps1

if (!$?) {
    Write-Output "Could not build Java. Is javac on your path? Exiting." 
    exit 1
}

echo "Building Java header file"
#javah -verbose -classpath ./target/classes -force -d src/main/cpp org.lantern.win.WinProxy
javah -verbose -classpath ./target/classes -force -o src/main/cpp/WinProxy.h org.lantern.win.WinProxy
if (!$?) {
    Write-Output "Could not build cpp header file. Exiting" 
    exit 1
}

#devenv jlibtorrent.vcproj/jlibtorrent.sln /rebuild Release
#if (!$?) {
#    Write-Output "Could not build JNI project for WinProxy. Exiting" 
    exit 1
#}

#cp ./jlibtorrent.vcproj/Release/jnltorrent.dll ../../lib/jnltorrent.dll

#if (!$?) {
#    Write-Output "Could not copy dll. Exiting" 
#    exit 1
#}