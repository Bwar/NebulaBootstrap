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

deploy_without_download="no"
if [ $# -gt 0 ]
then
    if [ "$1" = "--without-download" ]
    then
        deploy_without_download="yes"
    fi
fi

replace_config="no"
build_dir_num=`ls -l ${DEPLOY_PATH}/build | wc -l`
if [ $build_dir_num -gt 3 -a "$deploy_without_download" = "no" ]            # deploy local
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
    if [ "$deploy_without_download" = "no" ]
    then
        mkdir NebulaDepend lib_build
    fi

    # install protobuf
    cd ${BUILD_PATH}/lib_build
    if [ "$deploy_without_download" = "no" ]
    then
        wget https://github.com/google/protobuf/archive/v3.6.0.zip
        unzip v3.6.0.zip
        rm v3.6.0.zip >> /dev/null 2>&1
    fi
    cd protobuf-3.6.0
    chmod u+x autogen.sh
    chmod u+x configure 
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install

    # install libev
    cd ${BUILD_PATH}/lib_build
    if [ "$deploy_without_download" = "no" ]
    then
        wget https://github.com/kindy/libev/archive/master.zip
        unzip master.zip
        rm master.zip
        mv libev-master libev
    fi
    cd libev/src
    chmod u+x autogen.sh
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install

    # install hiredis
    cd ${BUILD_PATH}/lib_build
    if [ "$deploy_without_download" = "no" ]
    then
        wget https://github.com/redis/hiredis/archive/v0.13.0.zip
        unzip v0.13.0.zip
        rm v0.13.0.zip
        mv hiredis-0.13.0 hiredis
    fi
    cd hiredis
    make
    mkdir -p ../../NebulaDepend/include/hiredis
    cp -r adapters *.h ../../NebulaDepend/include/hiredis/
    cp libhiredis.so ../../NebulaDepend/lib/

    # install openssl
    ssl_num=`whereis ssl | awk '{print NF}'`
    if [ $ssl_num -gt 1 ]
    then
        echo "ssl had been installed."
    else
        cd ${BUILD_PATH}/lib_build
        if [ "$deploy_without_download" = "no" ]
        then
            wget https://github.com/openssl/openssl/archive/OpenSSL_1_1_0.zip
            unzip OpenSSL_1_1_0.zip
            rm OpenSSL_1_1_0.zip >> /dev/null 2>&1
        fi
        cd openssl-OpenSSL_1_1_0
        ./config --prefix=${BUILD_PATH}/NebulaDepend
        make
        make install
    fi

    # install crypto++
    cd ${BUILD_PATH}/lib_build
    if [ "$deploy_without_download" = "no" ]
    then
        wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_6_0_0.tar.gz
        tar -zxvf CRYPTOPP_6_0_0.tar.gz
        rm CRYPTOPP_6_0_0.tar.gz
    fi
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
    if [ "$deploy_without_download" = "no" ]
    then
        wget https://github.com/Bwar/Nebula/archive/master.zip
        unzip master.zip
        rm master.zip
        mv Nebula-master Nebula
        mkdir Nebula/include
        mkdir Nebula/lib
    fi
    cd Nebula/proto
    ${BUILD_PATH}/NebulaDepend/bin/protoc *.proto --cpp_out=../src/pb

    cd ${BUILD_PATH}
    for server in $NEBULA_BOOTSTRAP
    do
        if [ "$deploy_without_download" = "no" ]
        then
            wget https://github.com/Bwar/${server}/archive/master.zip 
            unzip master.zip
            rm master.zip
            mv ${server}-master ${server}
        fi
    done
fi


cd ${DEPLOY_PATH}
echo "yes" | ./shutdown.sh
rm log/* >> /dev/null 2>&1

cd ${BUILD_PATH}/Nebula/src
sed -i 's/gcc-6/gcc/g' Makefile
sed -i 's/g++-6/g++/g' Makefile
make clean; make
cp libnebula.so ${DEPLOY_PATH}/lib/


for server in $NEBULA_BOOTSTRAP
do
    cd ${BUILD_PATH}/${server}/src/
    sed -i 's/gcc-6/gcc/g' Makefile
    sed -i 's/g++-6/g++/g' Makefile
    make clean; make
    cp ${server} ${DEPLOY_PATH}/bin/
    if [[ "$replace_config" == "yes" ]]
    then
        cd ${BUILD_PATH}/${server}/conf/
        cp *.json ${DEPLOY_PATH}/conf/
    fi
done


cd ${BUILD_PATH}/NebulaDynamic/Hello/src/
sed -i 's/gcc-6/gcc/g' Makefile
sed -i 's/g++-6/g++/g' Makefile
make clean; make
cp *.so ${DEPLOY_PATH}/plugins/logic/


cd ${DEPLOY_PATH}/
if [[ "$replace_config" == "yes" ]]
then
    ./configure.sh
fi
./startup.sh 


