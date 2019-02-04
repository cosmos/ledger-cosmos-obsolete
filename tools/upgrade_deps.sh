#!/usr/bin/env bash
SCRIPT_DIR=$(cd $(dirname $0) && pwd)
USRAPP_DIR=$(realpath ${SCRIPT_DIR}/../src/ledger-user)
VALAPP_DIR=$(realpath ${SCRIPT_DIR}/../src/ledger-val)

echo ${USRAPP_DIR}
echo ${VALAPP_DIR}

# Update Nano S SDK

cd ${USRAPP_DIR}
git remote add nanos-secure-sdk https://github.com/LedgerHQ/nanos-secure-sdk.git
git fetch nanos-secure-sdk
#git subtree add --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk master
git subtree merge --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk/master

cd ${VALAPP_DIR}
git remote add nanos-secure-sdk https://github.com/LedgerHQ/nanos-secure-sdk.git
git fetch nanos-secure-sdk
#git subtree add --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk master
git subtree merge --prefix deps/nanos-secure-sdk --squash nanos-secure-sdk/master

# Update ZXLIB

cd ${USRAPP_DIR}
git remote add ledger-zxlib https://github.com/ZondaX/ledger-zxlib.git
git fetch ledger-zxlib
#git subtree add --prefix deps/ledger-zxlib --squash ledger-zxlib/master
git subtree merge --prefix deps/ledger-zxlib --squash ledger-zxlib/master

cd ${VALAPP_DIR}
git remote add ledger-zxlib https://github.com/ZondaX/ledger-zxlib.git
git fetch ledger-zxlib
#git subtree add --prefix deps/ledger-zxlib --squash ledger-zxlib/master
git subtree merge --prefix deps/ledger-zxlib --squash ledger-zxlib/master
