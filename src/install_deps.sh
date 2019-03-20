#!/usr/bin/env bash
#*******************************************************************************
#*   (c) 2018 ZondaX GmbH
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

SCRIPT_DIR=$(cd $(dirname $0) && pwd)

# PK = 0425966465974196228d1d8a72e3c4cb6b62d6d5b8ffdeeba3af6677551e5413a60c47517e0bee963af31f5606c33a9483e8a6dc102c63bc295691ca05ee2c5d5c
# SK = 0130a1c6fa9154cad78d91a8ecbbdbba7e1efbff01840997949130bba5cb38cd

# python -m ledgerblue.setupCustomCA --name dev --public 0425966465974196228d1d8a72e3c4cb6b62d6d5b8ffdeeba3af6677551e5413a60c47517e0bee963af31f5606c33a9483e8a6dc102c63bc295691ca05ee2c5d5c --targetId 0x31100003

os_string="$(uname -s)"
case "${os_string}" in
	Linux*)
		sudo apt-get install libusb-1.0.0 libudev-dev
		pip install -U setuptools
		pip install -U --no-cache ledgerblue ecpy
		;;
	Darwin*)
		brew install libusb
		pip install -U ledgerblue ecpy
		;;
	*)
		echo "OS not recognized"
		;;
esac
