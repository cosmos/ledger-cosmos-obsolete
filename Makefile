#*******************************************************************************
#*   (c) 2019 ZondaX GmbH
#*
#*  Licensed under the Apache License, Version 2.0 (the "License");
#*  you may not use this file except in compliance with the License.
#*  You may obtain a copy of the License at
#*
#*      http://www.apache.org/licenses/LICENSE-2.0
#*
#*  Unless required by applicable law or agreed to in writing, software
#*  distributed under the License is distributed on an "AS IS" BASIS,
#*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#*  See the License for the specific language governing permissions and
#*  limitations under the License.
#********************************************************************************

.PHONY: all deps build clean load delete

LEDGER_COSMOS_SRC=$(CURDIR)/src/ledger-user
LEDGER_TENDERMINT_SRC=$(CURDIR)/src/ledger-val

DOCKER_IMAGE=zondax/ledger-docker-bolos
DOCKER_BOLOS_SDK=/project/deps/nanos-secure-sdk

all: build

deps:
	@echo "Install dependencies"
	$(CURDIR)/src/install_deps.sh

build_cosmos:
	docker run -i --rm \
	-e BOLOS_SDK=$(DOCKER_BOLOS_SDK) -e BOLOS_ENV=/opt/bolos \
	-u $(shell id -u) -v $(shell pwd):/project \
	$(DOCKER_IMAGE) \
	make -C /project/src/ledger-user

build_tendermint:
	docker run -i --rm \
	-e BOLOS_SDK=$(DOCKER_BOLOS_SDK) -e BOLOS_ENV=/opt/bolos \
	-u $(shell id -u) -v $(shell pwd):/project \
	$(DOCKER_IMAGE) \
	make -C /project/src/ledger-val

clean_cosmos:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_COSMOS_SRC) clean

clean_tendermint:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_TENDERMINT_SRC) clean

load_cosmos: build_cosmos
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_COSMOS_SRC) load

load_tendermint: build_tendermint
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_TENDERMINT_SRC) load

delete_cosmos:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_COSMOS_SRC) delete

delete_tendermint:
	BOLOS_SDK=$(CURDIR)/deps/nanos-secure-sdk BOLOS_ENV=/opt/bolos \
	make -C $(LEDGER_TENDERMINT_SRC) delete

init:
	@echo "Initializing device with test mnemonic! WARNING TAKES 2 MINUTES AND REQUIRES RECOVERY MODE"
	@python -m ledgerblue.hostOnboard --apdu --id 0 --prefix "" --passphrase "" --pin 5555 --words "equip will roof matter pink blind book anxiety banner elbow sun young"

clean: clean_cosmos clean_tendermint
build: build_cosmos build_tendermint
load: build load_cosmos load_tendermint
delete: delete_cosmos delete_tendermint
