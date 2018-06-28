#!/bin/bash

# global variable $LOG_FILE mush be defined before calling write_log()
function write_log()
{
   local log_time=`date +"%Y-%m-%d %H:%M:%S"`
   local log_level="INFO"
   local log_msg=$1
   if [ $# -ge 2 ]
   then
       log_level="$1"
       log_msg="$2"
   fi
   echo "[$log_time][$log_level] $log_msg" >> $LOG_FILE
}

function info_log()
{
    write_log "INFO" "$@"
}

function debug_log()
{
    write_log "DEBUG" "$@"
}

function error_log()
{
    write_log "ERROR" "$@"
}

function warn_log()
{
    write_log "WARN" "$@"
}

function fatal_log()
{
    write_log "FATAL" "$@"
}

