#!/bin/bash

DEPLOY_PATH="."
cd $DEPLOY_PATH
DEPLOY_PATH=`pwd`
BUILD_PATH="${DEPLOY_PATH}/build"
NEBULA_BOOTSTRAP="NebulaBeacon NebulaInterface NebulaLogic"
chmod u+x *.sh

mkdir -p ${DEPLOY_PATH}/lib >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/bin >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/conf >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/log >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/data >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/plugins/logic >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/conf/ssl >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/temp >> /dev/null 2>&1
mkdir -p ${DEPLOY_PATH}/build >> /dev/null 2>&1

replace_config="no"
bin_file_num=`ls -l ${DEPLOY_PATH}/bin | wc -l`
conf_file_num=`ls -l ${DEPLOY_PATH}/conf | wc -l`
if [ $bin_file_num -ge 3 -a $conf_file_num -ge 3 ]            # deploy local
then
    echo "please input the build path[\"enter\" when using default build path]:"
    read build_path
    if [ ! -z "$build_path" ]
    then
        BUILD_PATH="$build_path"
    fi

    echo "do you want to replace all the config files with the original config files in build path? [yes | no]"
    read replace_config
else                # deploy remote
    cd ${BUILD_PATH}
    mkdir NebulaDepend lib_build

    # install protobuf
    cd ${BUILD_PATH}/lib_build
    wget https://github.com/google/protobuf/archive/v3.6.0.zip
    unzip v3.6.0.zip
    rm v3.6.0.zip >> /dev/null 2>&1
    cd protobuf-3.6.0
    chmod u+x autogen.sh
    chmod u+x configure 
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install

    # install libev
    cd ${BUILD_PATH}/lib_build
    git clone https://github.com/kindy/libev.git libev
    cd libev/src
    chmod u+x autogen.sh
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install

    # install hiredis
    cd ${BUILD_PATH}/lib_build
    git clone https://github.com/redis/hiredis.git hiredis
    cd hiredis
    make
    mkdir -p ../../NebulaDepend/include/hiredis
    cp -r adapters *.h ../../NebulaDepend/include/hiredis/
    cp libhiredis.so ../../NebulaDepend/lib/

    # install openssl
    cd ${BUILD_PATH}/lib_build
    wget https://github.com/openssl/openssl/archive/OpenSSL_1_1_0.zip
    unzip OpenSSL_1_1_0.zip
    rm OpenSSL_1_1_0.zip >> /dev/null 2>&1
    cd openssl-OpenSSL_1_1_0
    ./config --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install

    # install crypto++
    cd ${BUILD_PATH}/lib_build
    wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_6_0_0.tar.gz
    tar -zxvf CRYPTOPP_6_0_0.tar.gz
    rm CRYPTOPP_6_0_0.tar.gz
    cd cryptopp-CRYPTOPP_6_0_0
    make libcryptopp.so
    mkdir -p ../../NebulaDepend/include/cryptopp
    cp *.h ../../NebulaDepend/include/cryptopp/
    cp libcryptopp.so ../../NebulaDepend/lib/

    # copy libs to deploy path
    cd ${BUILD_PATH}/NebulaDepend/lib
    tar -zcvf neb_depend.tar.gz lib* pkgconfig engines-1.1
    mv neb_depend.tar.gz ${DEPLOY_PATH}/lib/
    cd ${DEPLOY_PATH}/lib
    tar -zxvf neb_depend.tar.gz
    rm neb_depend.tar.gz

    # now download Nebula and NebulaBootstrap
    cd ${BUILD_PATH}
    git clone https://github.com/Bwar/Nebula.git ${BUILD_PATH}/Nebula
    mkdir Nebula/include
    mkdir Nebula/lib
    cd Nebula/proto
    ${BUILD_PATH}/NebulaDepend/bin/protoc *.proto --cpp_out=../src/pb

    cd ${BUILD_PATH}
    for server in $NEBULA_BOOTSTRAP
    do
        git clone https://github.com/Bwar/${server}.git ${BUILD_PATH}/${server}
    done
fi


cd ${DEPLOY_PATH}
echo "yes" | ./shutdown.sh
rm log/* >> /dev/null 2>&1

cd ${BUILD_PATH}/Nebula/src
make clean; make
cp libnebula.so ${DEPLOY_PATH}/lib/


for server in $NEBULA_BOOTSTRAP
do
    cd ${BUILD_PATH}/${server}/src/
    make clean; make
    cp ${server} ${DEPLOY_PATH}/bin/
    if [[ "$replace_config" == "yes" ]]
    then
        cd ${BUILD_PATH}/${server}/conf/
        cp *.json ${DEPLOY_PATH}/conf/
    fi
done


cd ${BUILD_PATH}/NebulaDynamic/Hello/src/
make clean; make
cp *.so ${DEPLOY_PATH}/plugins/logic/


cd ${DEPLOY_PATH}/
if [[ "$replace_config" == "yes" ]]
then
    ./configure.sh
fi
./startup.sh 


