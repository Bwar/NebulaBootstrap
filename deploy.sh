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

function Usage()
{
    echo "Usage: $0 [OPTION]..."
    echo "options:"
    echo -e "  -h, --help\t\t\tdisplay this help and exit."
    echo -e "  -v, --version\t\t\tdisplay nebula version and exit."
    echo -e "  -L, --local\t\t\tdeploy from local without download any files from internet."
    echo -e "      --only-nebula\t\tonly build Nebula and NebulaBootstrap."
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
    --long help,version,local,only-nebula,with-ssl,with-custom-ssl,with-ssl-include:,with-ssl-lib: \
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

replace_config="no"
build_dir_num=`ls -l ${DEPLOY_PATH}/build | wc -l`
if $DEPLOY_ONLY_NEBULA
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
    if ! $DEPLOY_LOCAL
    then
        mkdir NebulaDepend lib_build >> /dev/null 2>&1
    fi

    # install protobuf
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -d protobuf-3.6.0 ]
        then
            echo "directory protobuf-3.6.0 exist, skip download."
        else
            wget https://github.com/google/protobuf/archive/v3.6.0.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download protobuf!" >&2
                exit 2
            fi
            unzip v3.6.0.zip
            rm v3.6.0.zip >> /dev/null 2>&1
        fi
    fi
    cd protobuf-3.6.0
    chmod u+x autogen.sh
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install
    if [ $? -ne 0 ]
    then
        echo "failed, teminated!" >&2
        exit 2
    fi

    # install libev
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -d libev ]
        then
            echo "directory libev exist, skip download."
        else
            wget https://github.com/kindy/libev/archive/master.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download libev!" >&2
                exit 2
            fi
            unzip master.zip
            rm master.zip
            mv libev-master libev
        fi
    fi
    cd libev/src
    chmod u+x autogen.sh
    ./autogen.sh
    ./configure --prefix=${BUILD_PATH}/NebulaDepend
    make
    make install
    if [ $? -ne 0 ]
    then
        echo "failed, teminated!" >&2
        exit 2
    fi

    # install hiredis
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -d hiredis ]
        then
            echo "directory hiredis exist, skip download."
        else
            wget https://github.com/redis/hiredis/archive/v0.13.0.zip
            if [ $? -ne 0 ]
            then
                echo "failed to download hiredis!" >&2
                exit 2
            fi
            unzip v0.13.0.zip
            rm v0.13.0.zip
            mv hiredis-0.13.0 hiredis
        fi
    fi
    cd hiredis
    make
    mkdir -p ../../NebulaDepend/include/hiredis
    cp -r adapters *.h ../../NebulaDepend/include/hiredis/
    cp libhiredis.so ../../NebulaDepend/lib/
    if [ $? -ne 0 ]
    then
        echo "failed, teminated!" >&2
        exit 2
    fi

    # install openssl
    if $DEPLOY_WITH_CUSTOM_SSL
    then
        cd ${BUILD_PATH}/lib_build
        if ! $DEPLOY_LOCAL
        then
            if [ -d openssl-OpenSSL_1_1_0 ]
            then
                echo "directory openssl-OpenSSL_1_1_0 exist, skip download."
            else
                wget https://github.com/openssl/openssl/archive/OpenSSL_1_1_0.zip
                if [ $? -ne 0 ]
                then
                    echo "failed to download openssl!" >&2
                    exit 2
                fi
                unzip OpenSSL_1_1_0.zip
                rm OpenSSL_1_1_0.zip >> /dev/null 2>&1
            fi
        fi
        cd openssl-OpenSSL_1_1_0
        ./config --prefix=${BUILD_PATH}/NebulaDepend
        make
        make install
        if [ $? -ne 0 ]
        then
            echo "failed, teminated!" >&2
            exit 2
        fi
    fi

    # install crypto++
    cd ${BUILD_PATH}/lib_build
    if ! $DEPLOY_LOCAL
    then
        if [ -d cryptopp-CRYPTOPP_6_0_0 ]
        then
            echo "directory cryptopp-CRYPTOPP_6_0_0 exist, skip download."
        else
            wget https://github.com/weidai11/cryptopp/archive/CRYPTOPP_6_0_0.tar.gz
            if [ $? -ne 0 ]
            then
                echo "failed to download crypto++!" >&2
                exit 2
            fi
            tar -zxvf CRYPTOPP_6_0_0.tar.gz
            rm CRYPTOPP_6_0_0.tar.gz
        fi
    fi
    cd cryptopp-CRYPTOPP_6_0_0
    make libcryptopp.so
    mkdir -p ../../NebulaDepend/include/cryptopp
    cp *.h ../../NebulaDepend/include/cryptopp/
    cp libcryptopp.so ../../NebulaDepend/lib/
    if [ $? -ne 0 ]
    then
        echo "failed, teminated!" >&2
        exit 2
    fi

    # copy libs to deploy path
    cd ${BUILD_PATH}/NebulaDepend/lib
    tar -zcvf neb_depend.tar.gz lib* pkgconfig 
    mv neb_depend.tar.gz ${DEPLOY_PATH}/lib/
    cd ${DEPLOY_PATH}/lib
    rm -r lib*
    tar -zxvf neb_depend.tar.gz
    rm neb_depend.tar.gz

fi

# download Nebula 
cd ${BUILD_PATH}
if ! $DEPLOY_LOCAL
then
    if [ -d Nebula ]
    then
        echo "directory Nebula exist, skip download."
    else
        wget https://github.com/Bwar/Nebula/archive/master.zip
        if [ $? -ne 0 ]
        then
            echo "failed to download Nebula!" >&2
            exit 2
        fi
        unzip master.zip
        rm master.zip
        mv Nebula-master Nebula
        mkdir Nebula/include
        mkdir Nebula/lib
    fi
fi
cd Nebula/proto
${BUILD_PATH}/NebulaDepend/bin/protoc *.proto --cpp_out=../src/pb
if [ $? -ne 0 ]
then
    echo "failed, teminated!" >&2
    exit 2
fi

# download NebulaBootstrap server
cd ${BUILD_PATH}
for server in $NEBULA_BOOTSTRAP
do
    if ! $DEPLOY_LOCAL
    then
        if [ -d "${server}" ]
        then
            echo "directory ${server} exist, skip download."
        else
            wget https://github.com/Bwar/${server}/archive/master.zip 
            if [ $? -ne 0 ]
            then
                echo "failed to download ${server}!" >&2
                exit 2
            fi
            unzip master.zip
            rm master.zip
            mv ${server}-master ${server}
            if [ $? -ne 0 ]
            then
                echo "failed, teminated!" >&2
                exit 2
            fi
        fi
    fi
done

if $DEPLOY_LOCAL -a $DEPLOY_ONLY_NEBULA
then
    cd ${DEPLOY_PATH}
    echo "yes" | ./shutdown.sh
    rm log/* >> /dev/null 2>&1
fi

# modify Makefile and make
cd ${BUILD_PATH}/Nebula/src
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
    sed -i 's/-DWITH_OPENSSL / /g' Makefile
    sed -i '/-L$(LIB3RD_PATH)\/lib -lssl/d' Makefile
fi
make clean; make
cp libnebula.so ${DEPLOY_PATH}/lib/
if [ $? -ne 0 ]
then
    echo "failed, teminated!" >&2
    exit 2
fi


for server in $NEBULA_BOOTSTRAP
do
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
    make clean; make
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
done


cd ${BUILD_PATH}/NebulaDynamic/Hello/src/
sed -i 's/gcc-6/gcc/g' Makefile
sed -i 's/g++-6/g++/g' Makefile
make clean; make
cp *.so ${DEPLOY_PATH}/plugins/logic/ >> /dev/null


cd ${DEPLOY_PATH}/
if [[ "$replace_config" == "yes" ]]
then
    ./configure.sh
fi
./startup.sh 


