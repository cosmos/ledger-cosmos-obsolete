#!/usr/bin/env bash
SCRIPT_DIR=$(cd $(dirname $0) && pwd)

cd ${SCRIPT_DIR}/../ledger-user
git remote add nanos-secure-sdk https://github.com/LedgerHQ/nanos-secure-sdk.git
git fetch nanos-secure-sdk
#git subtree add --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk master
git subtree merge --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk/master