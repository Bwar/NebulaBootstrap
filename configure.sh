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
export LD_LIBRARY_PATH=${LD_LIBRARY_PATH}:${NEB_LIB}

LOG_FILE="${NEB_HOME}/log/${SCRIPT_NAME}.log"

. ${NEB_HOME}/script/bash_lib.sh
source ${NEB_HOME}/script/json.bash

echo "Please input the IP address you want to bind to:"
read bind_ip_address
ifconfig >> /dev/null 2>&1
if [ $? -eq 0 ]
then
    valid_ip_list=`ifconfig | awk '/inet/{print $2}'`
    if [[ "$valid_ip_list" =~ "$bind_ip_address" ]]
    then
        echo "\"$bind_ip_address\" is valid."
    else
        echo "\"$bind_ip_address\" is invalid, please type \"ifconfig\" to get the valid address."
        exit 1
    fi
fi

server_conf_list=""
if [ $# -ge 1 ]
then
    for neb_bin in $@
    do
        if [ ! -f "${NEB_BIN}/$neb_bin" ]
        then
            echo "error: ${NEB_BIN}/$neb_bin not exist!"
        elif [ ! -f "${NEB_CONF}/${neb_bin}.json" ]
        then
            echo "error: ${NEB_CONF}/${neb_bin}.json not exist!"
        else
            json_tree=$(cat ${NEB_CONF}/${neb_bin}.json | JSON.load)
            #JSON.put /host $bind_ip_address json_tree 
            #JSON.dump json_tree > ${neb_bin}.json
            original_host=$(JSON.get /host json_tree)
            sed -i "s/$original_host/\"$bind_ip_address\"/g" ${NEB_CONF}/${neb_bin}.json
            server_conf_list="$server_conf_list ${neb_bin}.json"
        fi
    done
    echo "the \"host\" field in \"$server_conf_list\" had been changed."
    exit 0
fi


neb_servre_bin_files=`ls ${NEB_BIN}/`
for neb_bin in $neb_servre_bin_files
do
    if [ -f "${NEB_CONF}/${neb_bin}.json" ]
    then
        json_tree=$(cat ${NEB_CONF}/${neb_bin}.json | JSON.load)
        #JSON.put /host $bind_ip_address json_tree >> /dev/null 2>&1
        #JSON.dump json_tree > ${neb_bin}.json
        original_host=$(JSON.get /host json_tree)
        sed -i "s/$original_host/\"$bind_ip_address\"/g" ${NEB_CONF}/${neb_bin}.json
        server_conf_list="$server_conf_list ${neb_bin}.json"
    fi
done
echo "the \"host\" field in \"$server_conf_list\" had been changed."
echo "Successful."

