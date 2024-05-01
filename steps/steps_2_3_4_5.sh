#!/usr/bin/env bash

while [[ $# -gt 0 ]]; do
    case $1 in
        --msg)
            LOG_ID=$2
            shift
            shift
            ;;
        *)
            echo "Unknown option $1"
            exit 1
    esac
done

if [[ -z $LOG_ID ]]; then
    echo "Missing --msg argument"
    exit 1
fi

steps/step_2_make_msg_summary --msg $LOG_ID &&
steps/step_3_make_msg_diagrams --msg $LOG_ID &&
steps/step_4_make_msg_tex_file --msg $LOG_ID &&
steps/step_5_make_msg_tables --msg $LOG_ID
