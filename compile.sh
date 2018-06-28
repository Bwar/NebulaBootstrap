#!/bin/bash

DEPLOY_PATH="."
cd $DEPLOY_PATH
DEPLOY_PATH=`pwd`
CODE_PATH="${DEPLOY_PATH}/temp/compile"

cd ${DEPLOY_PATH}
echo "yes" | ./stop.sh
rm log/*

cd ${CODE_PATH}/Nebula/src
make clean; make
cp libnebula.so ${DEPLOY_PATH}/nebula/lib/

cd ${CODE_PATH}/NebulaBeacon/src/
make clean; make
cp Beacon ${DEPLOY_PATH}/nebula/bin/

cd ${CODE_PATH}/NebulaInterface/src
make clean; make
cp Interface ${DEPLOY_PATH}/nebula/bin/

cd ${CODE_PATH}/NebulaLogic/src/
make clean; make
cp Logic ${DEPLOY_PATH}/nebula/bin/

cd ${CODE_PATH}/NebulaLogger/src/
make clean; make
cp Logger ${DEPLOY_PATH}/nebula/bin/

cd ${CODE_PATH}/NebulaDynamic/Hello/src/
make clean; make
cp *.so ${DEPLOY_PATH}/nebula/plugins/logic/

cd ${DEPLOY_PATH}/nebula/
./start.sh Beacon; sleep 1; ./start.sh Logic; sleep 1; ./start.sh Interface; sleep 1; ./start.sh Logger

cd $DEPLOY_PATH

