# Ledger-cosmos
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![CircleCI](https://circleci.com/gh/cosmos/ledger-cosmos/tree/master.svg?style=shield)](https://circleci.com/gh/cosmos/ledger-cosmos/tree/master)

This repository contains two Ledger Nano S apps for Tendermint/Cosmos:

- Ledger Nano S Cosmos BOLOS app
- Ledger Nano S Tendermint validator BOLOS app
- C++ unit tests

Source code for apps is linked as submodules to allow for Ledger's build infrastructure.

For development purposes, this repo is recommended as it includes unit tests, tools, etc.  

---------------------

## Installing

### Cosmos app

The Cosmos app is already available in [Ledger Live](https://www.ledger.com/pages/ledger-live). Our preferred and recommended hardware wallet!

::: Until Voyager integration has been reviewed, the app requires enabling *developer mode*

- Open Ledger Live and go to Settings (gear icon on the right):

![](docs/img/cosmos_app1.png)

- Enable developer mode (last option):

![](docs/img/cosmos_app2.png)

- Now go back to manager and search for Cosmos:

![](docs/img/cosmos_app3.png)


### Validator app

The validator app is still under review/development. 

To install, please follow the build instructions in the following section.

---------------------

## Building

The following document describes how to build the apps: [Build instructions](docs/BUILD.md)

---------------------

## Specifications

### Cosmos App

- [APDU Protocol](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/PROTOSPEC.md)
- [Transaction format](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/TXSPEC.md)
- [User interface](https://github.com/ZondaX/ledger-cosmos-app/tree/master/docs/UISPEC.md)

### Validator App

The target of this app are validators. The app signs votes/proposals via the Tendermint/Cosmos KMS (Key management system).

### Specifications

- [APDU Protocol](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/APDUSPEC.md)
- [Transaction format](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/TXSPEC.md)
- [User interface](https://github.com/ZondaX/ledger-cosmos-val-app/blob/master/docs/UISPEC.md)
