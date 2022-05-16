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

#if [ ! -d "$BUILD_DIR" ]
#then
#    mkdir $BUILD_DIR
#fi

if [  -d "$BUILD_DIR" ]
then
    rm -r $BUILD_DIR
fi
mkdir $BUILD_DIR
cd $BUILD_DIR

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

#$QMAKE_FILE ../Montezuma_plugin
$QMAKE_FILE ${SCRIPT_DIR}/Montezuma_plugin
make

#QMAKE_FILE ../Montezuma_qml
$QMAKE_FILE ${SCRIPT_DIR}/Montezuma_qml
make

./Montezuma_qml
