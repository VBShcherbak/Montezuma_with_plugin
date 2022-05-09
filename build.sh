QMAKE_FILE="qmake"
#/home/vbshcherbak/Qt/5.15.2/gcc_64/bin/qmake
while [ -n "$1" ]
do
case "$1" in
-qmake) QMAKE_FILE="$2" ;;
--help) echo "Usage: ./build.sh [option]"
        echo "By default \"qmake\" in PATH variable is used"
        echo "-qmake    path to qmake file"
        echo "          Example: ./build.sh -qmake /path/to/dir/qmake" 
        echo "--help    display help and exit"
        exit 0;;
esac
shift
done

BUILD_DIR=build

if [ ! -d "$BUILD_DIR" ]
then
    mkdir $BUILD_DIR
fi

cd $BUILD_DIR

$QMAKE_FILE ../Montezuma_plugin
make

$QMAKE_FILE ../Montezuma_qml
make

./Montezuma_qml