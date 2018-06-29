#! /bin/bash 

NEB_HOME=`dirname $0`
SCRIPT_NAME=`basename $0`
cd ${NEB_HOME}
NEB_HOME=`pwd`

NEB_BIN=${NEB_HOME}/bin
NEB_CONF=${NEB_HOME}/conf
NEB_LIB=${NEB_HOME}/lib
NEB_LOG=${NEB_HOME}/log
NEB_TEMP=${NEB_HOME}/temp
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${NEB_LIB}

LOG_FILE="${NEB_HOME}/log/${SCRIPT_NAME}.log"

. ${NEB_HOME}/script/bash_lib.sh
source ${NEB_HOME}/script/json.bash 

if [ $# -ge 1 ]
then
    for neb_bin in $@
    do
        if [ ! -f "${NEB_BIN}/$neb_bin" ]
        then
            echo "error: ${NEB_BIN}/$neb_bin not exist!"
        elif [ ! -f "${NEB_CONF}/$neb_bin.json" ]
        then
            echo "error: ${NEB_CONF}/$neb_bin.json not exist!"
        else
            json_tree=$(cat ${NEB_CONF}/$neb_bin.json | JSON.load)
            neb_server=$(JSON.get /server_name json_tree)
            neb_port=$(JSON.get /port json_tree)
            running_neb_pid=`netstat -apn 2>>/dev/null | grep -w "$neb_port" | awk -F/ '/^tcp/{print $1}' | awk '/LISTEN/{print $NF}'`
            if [ -n "$running_neb_pid" ]
            then
                echo "kill $running_neb_pid    $neb_server" 
                kill $running_neb_pid
            fi
        fi
    done
    exit 0
fi


echo "do you want to stop all server process? [yes|no]"
read stop_neb_server
if [ "yes" != "$stop_neb_server" ]
then
    echo "cancel"
    exit 0
fi
neb_server_bin_files=`ls ${NEB_BIN}/`
for neb_bin in $neb_server_bin_files
do
    if [ -f "${NEB_CONF}/${neb_bin}.json" ]
    then
        json_tree=$(cat ${NEB_CONF}/$neb_bin.json | JSON.load)
        neb_server=$(JSON.get /server_name json_tree)
        neb_port=$(JSON.get /port json_tree)
        running_neb_pid=`netstat -apn 2>>/dev/null | grep -w "$neb_port" | awk -F/ '/^tcp/{print $1}' | awk '/LISTEN/{print $NF}'`
        if [ -n "$running_neb_pid" ]
        then
            echo "kill $running_neb_pid    $neb_server"
            kill $running_neb_pid
        fi
    fi
done


