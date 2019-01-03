# Ledger-cosmos
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CircleCI](https://circleci.com/gh/cosmos/ledger-cosmos/tree/master.svg?style=shield)](https://circleci.com/gh/cosmos/ledger-cosmos/tree/master)

> It is work in progress and subject to further modifications and testing.
>
> WARNING: DO NOT USE THIS APP IN A LEDGER WITH REAL FUNDS.**

This repository contains two Ledger Nano S apps for Tendermint/Cosmos: 

- Ledger Nano S User app/firmware
- Ledger Nano S Validator app/firmware
- C++ unit tests

Source code for apps is linked as submodules to allow for Ledger's build infrastructure. 

For development purposes, this repo is recommended as it includes unit tests, tools, etc.  

# Building

The following document describes how to build the apps: [Build instructions](docs/BUILD.md)

# User App

The target of this app are final users. The app allows signing transactions in the Tendermint/Cosmos ecosystem. 

### Specifications

- [APDU Protocol](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/PROTOSPEC.md)
- [Transaction format](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/TXSPEC.md)
- [User interface](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/UISPEC.md)

# Validator App

The target of this app are validators. The app signs votes/proposals via the Tendermint/Cosmos KMS (Key management system).

### Specifications

- [APDU Protocol](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/APDUSPEC.md)
- [Transaction format](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/TXSPEC.md)
- [User interface](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/UISPEC.md)
