#!/bin/bash

NEB_HOME=`dirname $0`
SCRIPT_NAME=`basename $0`
cd ${NEB_HOME}
NEB_HOME=`pwd`

NEB_BIN=${NEB_HOME}/bin
NEB_CONF=${NEB_HOME}/conf
NEB_LIB=${NEB_HOME}/lib
NEB_LOG=${NEB_HOME}/log
NEB_TEMP=${NEB_HOME}/temp
export LD_LIBRARY_PATH=${NEB_LIB}

LOG_FILE="${NEB_HOME}/log/${SCRIPT_NAME}.log"

. ${NEB_HOME}/script/bash_lib.sh
source ${NEB_HOME}/script/json.bash

server_list=""
port_list=""
if [ $# -ge 1 ]
then
    for neb_bin in $@
    do
        if [ ! -f "${NEB_BIN}/${neb_bin}" ]
        then
            echo "error: ${NEB_BIN}/${neb_bin} not exist!"
        elif [ ! -f "${NEB_CONF}/${neb_bin}.json" ]
        then
            echo "error: ${NEB_CONF}/${neb_bin}.json not exist!"
        else
            json_tree=$(cat ${NEB_CONF}/${neb_bin}.json | JSON.load)
            neb_server=$(JSON.get /server_name json_tree | awk -F\" '{print $2}')
            ${NEB_BIN}/${neb_bin} ${NEB_CONF}/${neb_bin}.json
            if [ $? -eq 0 ]
            then
                info_log "${NEB_HOME}/bin/${neb_bin} start successfully."
                server_list="$server_list $neb_server"
            else
                error_log "failed to start ${neb_bin}"
                echo "failed to start ${neb_bin}"
            fi
        fi
    done
    sleep 1
    for s in $server_list
    do
        ps -ef | awk -v pname=$s '{idx=index($8,pname); if (idx == 1)print}'
    done
    exit 0
fi


neb_server_bin_files=`ls ${NEB_BIN}/`
neb_server_queue=""

for neb_bin in $neb_server_bin_files
do
    if [[ "$neb_bin" =~ "Beacon" || "$neb_bin" =~ "beacon" ]]
    then
        neb_server_queue="${neb_bin} ${neb_server_queue}"
    else
        neb_server_queue="${neb_server_queue} ${neb_bin}"
    fi
done

for neb_bin in $neb_server_queue
do
    if [ -f "${NEB_CONF}/${neb_bin}.json" ]
    then
        json_tree=$(cat ${NEB_CONF}/${neb_bin}.json | JSON.load)
        neb_server=$(JSON.get /server_name json_tree | awk -F\" '{print $2}')
        neb_port=$(JSON.get /port json_tree)
        running_neb_pid=`netstat -apn 2>>/dev/null | grep -w "$neb_port" | awk -F/ '/^tcp/{print $1}' | awk '/LISTEN/{print $NF}'`
        if [ -z "$running_neb_pid" ]
        then
            ${NEB_BIN}/${neb_bin} ${NEB_CONF}/${neb_bin}.json
            if [ $? -eq 0 ]
            then
                info_log "${NEB_HOME}/bin/${neb_bin} start successfully."
                server_list="$server_list $neb_server"
            else
                error_log "failed to start ${neb_bin}"
                echo "failed to start ${neb_bin}"
            fi
            ps -ef | awk -v pname=$neb_server '{idx=index($8,pname); if (idx == 1)print}'
        else
            info_log "the process for ${neb_bin} was exist!"
        fi
    fi
done

