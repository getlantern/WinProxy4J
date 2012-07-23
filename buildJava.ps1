#javac ./src/main/java/org/lastbamboo/jni/JLibTorrent.java
cmd /c "mvn.bat install -Dmaven.test.skip=true"

if (!$?) {
    Write-Output "Could not build Java file. Is javac on your path? Exiting." 
    exit 1
}
