#!/usr/bin/env bash

# --- begin runfiles.bash initialization v3 ---
# Copy-pasted from the Bazel Bash runfiles library v3.
set -uo pipefail; set +e; f=bazel_tools/tools/bash/runfiles/runfiles.bash
source "${RUNFILES_DIR:-/dev/null}/$f" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "${RUNFILES_MANIFEST_FILE:-/dev/null}" | cut -f2- -d' ')" 2>/dev/null || \
  source "$0.runfiles/$f" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "$0.runfiles_manifest" | cut -f2- -d' ')" 2>/dev/null || \
  source "$(grep -sm1 "^$f " "$0.exe.runfiles_manifest" | cut -f2- -d' ')" 2>/dev/null || \
  { echo>&2 "ERROR: cannot find $f"; exit 1; }; f=; set -e
# --- end runfiles.bash initialization v3 ---


while [[ $# -gt 0 ]]; do
    case $1 in
        --msg)
            msg=$2
            shift
            shift
            ;;
        *)
            echo "Unknown option $1"
            exit 1
    esac
done

if [[ -z $msg ]]; then
    echo "Missing --msg argument"
    exit 1
fi

$(rlocation magnon/steps/step_3_make_msg_diagrams) --msg $msg &&
$(rlocation magnon/steps/step_6_compile_msg_figures) --msg $msg
