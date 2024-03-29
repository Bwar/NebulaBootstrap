#!/bin/bash

DEPLOY_PATH="."
cd $DEPLOY_PATH
DEPLOY_PATH=`pwd`
BUILD_PATH="${DEPLOY_PATH}/build"
NEBULA_BOOTSTRAP="NebulaBeacon NebulaInterface NebulaLogic"
CPU_NUM=`lscpu | awk '/^CPU\(s\)/{print $2}'`
CPU_NUM=1
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

function Usage()
{
    echo "Usage: $0 [OPTION]..."
    echo "options:"
    echo -e "  -h, --help\t\t\tdisplay this help and exit."
    echo -e "  -v, --version\t\t\tdisplay nebula version and exit."
    echo -e "  -L, --local\t\t\tdeploy from local without download any files from internet."
    echo -e "      --demo\t\tbuild NebulaDemo and NebulaDepend."
    echo -e "      --only-nebula\t\tonly build Nebula and NebulaBootstrap."
    echo -e "      --build-path\t\tset the compilation path."
    echo -e "      --with-ssl\t\tinclude openssl for ssl and crypto. [default: without ssl]"
    echo -e "      --with-custom-ssl\t\tinclude openssl for ssl and crypto, the openssl is a custom installation version. [default: without ssl]"
    echo -e "      --with-ssl-include\tthe openssl include path."
    echo -e "      --with-ssl-lib\t\tthe openssl library path."
    echo "example:"
    echo "  $0 --local --with-ssl --with-ssl-lib /usr/local/lib64 --with-ssl-include /usr/local/include"
    echo ""
}

DEPLOY_ONLY_NEBULA=false
DEPLOY_LOCAL=false
DEPLOY_WITH_SSL=false
DEPLOY_WITH_CUSTOM_SSL=false
SSL_INCLUDE_PATH=""
SSL_LIB_PATH=""
ARGV_DEFINE=`getopt \
    -o hvL \
    --long help,version,local,demo,only-nebula,with-ssl,with-custom-ssl,build-path:,with-ssl-include:,with-ssl-lib: \
    -n 'deploy.bash' \
    -- "$@"`
if [ $? != 0 ]
then
    echo "Terminated!" >&2
    exit 1
fi
eval set -- "$ARGV_DEFINE"

while :
do
    case "$1" in
        -h|--help)
            Usage
            exit 0
            ;;
        -v|--version)
            echo "0.3"
            exit 0
            ;;
        --demo)
            echo "build NebulaDemo and NebulaDepend."
            NEBULA_BOOTSTRAP="NebulaDemo"
            shift
            ;;
        --only-nebula)
            echo "Only build Nebula and NebulaBootstrap."
            DEPLOY_ONLY_NEBULA=true
            shift
            ;;
        -L|--local)
            echo "Deploy from local without download any files from internet."
            DEPLOY_LOCAL=true
            shift
            ;;
        --with-ssl)
            DEPLOY_WITH_SSL=true
            shift
            ;;
        --with-custom-ssl)
            DEPLOY_WITH_CUSTOM_SSL=true
            shift
            ;;
        --build-path)
            BUILD_PATH=$2
            shift 2
            ;;
        --with-ssl-include)
            SSL_INCLUDE_PATH=$2
            DEPLOY_WITH_SSL=true
            shift 2
            ;;
        --with-ssl-lib)
            SSL_LIB_PATH=$2
            DEPLOY_WITH_SSL=true
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "invalid argument!"
            break
            ;;
    esac
done

replace_config="yes"
build_dir_num=`ls -l ${DEPLOY_PATH}/build | wc -l`
if $DEPLOY_ONLY_NEBULA
then
    echo "do you want to replace all the config files with the original config files in build path? [yes | no]"
    read replace_config
else                # deploy remote
    cd ${BUILD_PATH}
    if ! $DEPLOY_LOCAL
    then
        mkdir NebulaDepend lib_build >> /dev/null 2>&1
    fi

    # install protobuf
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -f v3.6.0.zip ]
        then
            echo "protobuf-3.6.0 exist, skip download."
        else
            wget https://github.com/google/protobuf/archive/v3.6.0.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download protobuf!" >&2
                exit 2
            fi
        fi

        unzip v3.6.0.zip
        cd protobuf-3.6.0
        chmod u+x autogen.sh
        ./autogen.sh
        ./configure --prefix=${BUILD_PATH}/NebulaDepend
        make -j$CPU_NUM
        make install
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
        cd ${BUILD_PATH}/lib_build
        rm -rf protobuf-3.6.0 
    fi

    # install libev
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -f libev.zip ]
        then
            echo "libev exist, skip download."
        else
            wget https://github.com/kindy/libev/archive/master.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download libev!" >&2
                exit 2
            fi
            mv master.zip libev.zip
        fi

        unzip libev.zip
        mv libev-master libev
        cd libev/src
        chmod u+x autogen.sh
        ./autogen.sh
        ./configure --prefix=${BUILD_PATH}/NebulaDepend
        make -j$CPU_NUM
        make install
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
        cd ${BUILD_PATH}/lib_build
        rm -rf libev
    fi

    # install hiredis
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -f hiredis_v0.13.0.zip ]
        then
            echo "directory hiredis exist, skip download."
        else
            wget https://github.com/redis/hiredis/archive/v0.13.0.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download hiredis!" >&2
                exit 2
            fi
            mv v0.13.0.zip hiredis_v0.13.0.zip
        fi

        unzip hiredis_v0.13.0.zip
        mv hiredis-0.13.0 hiredis
        cd hiredis
        make -j$CPU_NUM
        mkdir -p ../../NebulaDepend/include/hiredis
        cp -r adapters *.h ../../NebulaDepend/include/hiredis/
        cp libhiredis.so ../../NebulaDepend/lib/libhiredis.so.0.13
        cd ../../NebulaDepend/lib/
        ln -s libhiredis.so.0.13 libhiredis.so
        cd -
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
        cd ${BUILD_PATH}/lib_build
        rm -rf hiredis
    fi

    # install openssl
    if $DEPLOY_WITH_CUSTOM_SSL
    then
        cd ${BUILD_PATH}/lib_build
        if ! $DEPLOY_LOCAL
        then
            if [ -f OpenSSL_1_1_0.zip ]
            then
                echo "openssl-OpenSSL_1_1_0 exist, skip download."
            else
                wget https://github.com/openssl/openssl/archive/OpenSSL_1_1_0.zip
                if [ $? -ne 0 ]
                then
                    echo "failed to download openssl!" >&2
                    exit 2
                fi
            fi

            unzip OpenSSL_1_1_0.zip
            cd openssl-OpenSSL_1_1_0
            ./config --prefix=${BUILD_PATH}/NebulaDepend
            make -j$CPU_NUM
            make install
            if [ $? -ne 0 ]
            then
                echo "failed, teminated!" >&2
                exit 2
            fi
            cd ${BUILD_PATH}/lib_build
            rm -rf openssl-OpenSSL_1_1_0
        fi
    fi

    # install crypto++
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -f CRYPTOPP_8_0_0.zip ]
        then
            echo "cryptopp-CRYPTOPP_8_0_0 exist, skip download."
        else
            wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_8_0_0.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download crypto++!" >&2
                exit 2
            fi
        fi

        unzip CRYPTOPP_8_0_0.zip
        cd cryptopp-CRYPTOPP_8_0_0
        make -j$CPU_NUM libcryptopp.so
        mkdir -p ../../NebulaDepend/include/cryptopp
        cp *.h ../../NebulaDepend/include/cryptopp/
        cp libcryptopp.so ../../NebulaDepend/lib/libcryptopp.so.8
        cd ../../NebulaDepend/lib
        ln -s libcryptopp.so.8 libcryptopp.so
        cd -
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
        cd ${BUILD_PATH}/lib_build
        rm -rf cryptopp-CRYPTOPP_8_0_0
    fi

    # copy libs to deploy path
    if ! $DEPLOY_LOCAL
    then
        cd ${BUILD_PATH}/NebulaDepend/lib
        tar -zcvf neb_depend.tar.gz lib*.so lib*.so.* 
        mv neb_depend.tar.gz ${DEPLOY_PATH}/lib/
        cd ${DEPLOY_PATH}/lib
        rm -r lib* >> /dev/null 2>&1
        tar -zxvf neb_depend.tar.gz
        rm neb_depend.tar.gz
    fi
fi


# shutdown running nebula server
if $DEPLOY_LOCAL -a $DEPLOY_ONLY_NEBULA
then
    cd ${DEPLOY_PATH}
    echo "yes" | ./shutdown.sh
    rm log/* >> /dev/null 2>&1
fi


# download Nebula 
cd ${BUILD_PATH}
if ! $DEPLOY_LOCAL
then
    if [ -f Nebula.zip ]
    then
        echo "Nebula exist, skip download."
    else
        wget https://github.com/Bwar/Nebula/archive/master.zip
        if [ $? -ne 0 ]
        then
            echo "failed to download Nebula!" >&2
            exit 2
        fi
        mv master.zip Nebula.zip
    fi
fi
if [ -d Nebula ]
then
    echo "Nebula directory exist."
else
    unzip Nebula.zip 
    mv Nebula-master Nebula
    mkdir Nebula/include
    mkdir Nebula/lib
fi
cd Nebula/proto
${BUILD_PATH}/NebulaDepend/bin/protoc *.proto --cpp_out=../src/pb
if [ $? -ne 0 ]
then
    echo "failed, teminated!" >&2
    exit 2
fi

# modify Nebula Makefile and make
cd ${BUILD_PATH}/Nebula/src
sed -i 's/gcc-6/gcc/g' Makefile
sed -i 's/g++-6/g++/g' Makefile
if $DEPLOY_WITH_SSL
then
    sed -i 's/-D__GUNC__/-D__GUNC__ -DWITH_OPENSSL/g' Makefile
    sed -i "/-L\$(LIB3RD_PATH)\/lib -lprotobuf/a\           -L\$(SYSTEM_LIB_PATH)\/lib -lssl \x5c" Makefile
    if [ ! -z "$SSL_INCLUDE_PATH" ]
    then
        sed -i "/-I \$(LIB3RD_PATH)\/include/a\        -I ${SSLINCLUDE_PATH} \x5c" Makefile
    fi
    if [ ! -z "$SSL_LIB_PATH" ]
    then
        sed -i "s/-L\$(SYSTEM_LIB_PATH)\/lib -lssl/-L${SSL_LIB_PATH} -lssl/g" Makefile
    fi
elif $DEPLOY_WITH_CUSTOM_SSL
then
    sed -i 's/-D__GUNC__/-D__GUNC__ -DWITH_OPENSSL/g' Makefile
    sed -i "/-L\$(LIB3RD_PATH)\/lib -lprotobuf/a\           -L\$(LIB3RD_PATH)\/lib -lssl \x5c" Makefile
    if [ ! -z "$SSL_INCLUDE_PATH" ]
    then
        sed -i "/-I \$(LIB3RD_PATH)\/include/a\        -I ${SSLINCLUDE_PATH} \x5c" Makefile
    fi
    if [ ! -z "$SSL_LIB_PATH" ]
    then
        sed -i "s/-L\$(LIB3RD_PATH)\/lib -lssl/-L${SSL_LIB_PATH} -lssl/g" Makefile
    fi
else
    sed -i 's/-DWITH_OPENSSL / /g' Makefile
    sed -i '/-L$(LIB3RD_PATH)\/lib -lssl/d' Makefile
    sed -i '/-L$(SYSTEM_LIB_PATH)\/lib -lssl/d' Makefile
fi
make clean; make -j$CPU_NUM
cp libnebula.so ${DEPLOY_PATH}/lib/
if [ $? -ne 0 ]
then
    echo "failed, teminated!" >&2
    exit 2
fi
cd ${BUILD_PATH}

# download NebulaBootstrap server
cd ${BUILD_PATH}
for server in $NEBULA_BOOTSTRAP
do
    if ! $DEPLOY_LOCAL
    then
        if [ -f "${server}.zip" ]
        then
            echo "${server} exist, skip download."
        else
            wget https://github.com/Bwar/${server}/archive/master.zip 
            if [ $? -ne 0 ]
            then
                echo "failed to download ${server}!" >&2
                exit 2
            fi
            mv master.zip ${server}.zip
        fi
    fi
    if [ -d ${server} ]
    then
        echo "${server} directory exist."
    else
        unzip ${server}.zip
        mv ${server}-master ${server}
    fi
    if [ -d ${server}/proto ]
    then
        cd ${server}/proto
        ${BUILD_PATH}/NebulaDepend/bin/protoc *.proto --cpp_out=../src
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
    fi
    cd ${BUILD_PATH}/${server}/src/
    sed -i 's/gcc-6/gcc/g' Makefile
    sed -i 's/g++-6/g++/g' Makefile
    if $DEPLOY_WITH_SSL
    then
        sed -i 's/-L$(LIB3RD_PATH)\/lib -lssl/-L$(SYSTEM_LIB_PATH)\/lib -lssl/g' Makefile
        if [ ! -z "$SSL_INCLUDE_PATH" ]
        then
            sed -i "/-I \$(LIB3RD_PATH)\/include/i\        -I ${SSLINCLUDE_PATH} \x5c" Makefile
        fi
        if [ ! -z "$SSL_LIB_PATH" ]
        then
            sed -i "s/-L\$(SYSTEM_LIB_PATH)\/lib -lssl/-L${SSL_LIB_PATH} -lssl/g" Makefile
        fi
    elif $DEPLOY_WITH_CUSTOM_SSL
    then
        if [ ! -z "$SSL_INCLUDE_PATH" ]
        then
            sed -i "/-I \$(LIB3RD_PATH)\/include/i\        -I ${SSLINCLUDE_PATH} \x5c" Makefile
        fi
        if [ ! -z "$SSL_LIB_PATH" ]
        then
            sed -i "s/-L\$(SYSTEM_LIB_PATH)\/lib -lssl/-L${SSL_LIB_PATH} -lssl/g" Makefile
        fi
    else
        sed -i '/-L$(LIB3RD_PATH)\/lib -lssl/d' Makefile
    fi
    make clean; make -j$CPU_NUM
    if [ $? -ne 0 ]
    then
        echo "failed, teminated!" >&2
        exit 2
    fi
    cp ${server} ${DEPLOY_PATH}/bin/
    if [[ "$replace_config" == "yes" ]]
    then
        cd ${BUILD_PATH}/${server}/conf/
        cp *.json ${DEPLOY_PATH}/conf/
    fi

    cd ${BUILD_PATH}
done


# download NebulaDynamic and build
if ! $DEPLOY_LOCAL && [ "$NEBULA_BOOTSTRAP" != "NebulaDemo" ] 
then
    if [ -f "NebulaDynamic.zip" ]
    then
        echo "NebulaDynamic exist, skip download."
    else
        wget https://github.com/Bwar/NebulaDynamic/archive/master.zip 
        if [ $? -ne 0 ]
        then
            echo "failed to download NebulaDynamic!" >&2
            exit 2
        fi
        mv master.zip NebulaDynamic.zip
    fi
fi
if [ -d NebulaDynamic ]
then
    echo "NebulaDynamic directory exist."
else
    unzip NebulaDynamic.zip
    mv NebulaDynamic-master NebulaDynamic
fi
if [ -d ${BUILD_PATH}/NebulaDynamic ]
then
    cd ${BUILD_PATH}/NebulaDynamic/Hello/src/
    sed -i 's/gcc-6/gcc/g' Makefile
    sed -i 's/g++-6/g++/g' Makefile
    make clean; make -j$CPU_NUM
    cp *.so ${DEPLOY_PATH}/plugins/logic/ >> /dev/null
fi
cd ${BUILD_PATH}


# startup nebula server
cd ${DEPLOY_PATH}/
if [[ "$replace_config" == "yes" ]]
then
    ./configure.sh
fi
./startup.sh 


