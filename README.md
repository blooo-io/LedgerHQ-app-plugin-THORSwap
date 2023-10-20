# Badges
[![Code style check](https://github.com/blooo-io/LedgerHQ-app-plugin-THORSwap/actions/workflows/lint-workflow.yml/badge.svg)](https://github.com/blooo-io/LedgerHQ-app-plugin-THORSwap/actions/workflows/lint-workflow.yml)
[![Compilation & tests](https://github.com/blooo-io/LedgerHQ-app-plugin-THORSwap/actions/workflows/ci-workflow.yml/badge.svg)](https://github.com/blooo-io/LedgerHQ-app-plugin-THORSwap/actions/workflows/ci-workflow.yml)

# Ledger THORSwap Plugin

This is a plugin for the Ethereum application which helps parsing and displaying relevant information when signing a thorswap transaction.

## Prerequisite

Clone the plugin to a new folder.

```shell
git clone https://github.com/blooo-io/LedgerHQ-app-plugin-THORSwap.git
```

Then in the same folder clone one more repository, which is the app-ethereum.

```shell
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     #app-ethereum
```
## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.adoc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network  |  Contract Name  | Address |
| ---      | ---             | ---     |
| Ethereum | TSSwapGeneric   | `0x213255345a740324cbCE0242e32076Ab735906e2`|
| Ethereum | TSLedgerAdapter | `0xb81c7c2d2d078205d7fa515ddb2dea3d896f4016`|
| Ethereum | THORChain_Router| `0xD37BbE5744D730a1d98d8DC97c42F0Ca46aD7146`|

## Methods

Methods covered by this plugin are:

| Conrtact Name | Selector | Method Name |
|      ---      |    ---   |     ---     |
| TSSwapGeneric    | 0xb5b4527a | swap |
| TSLedgerAdapter  | 0x4a95961e | ledgerCall (swapin)|
| THORChain_Router | 0x44bc937b | depositWithExpiry  |



## Build

Go to the global folder and run the below command.
```shell
sudo docker run --rm -ti -v "$(realpath .):/app" --user $(id -u $USER):$(id -g $USER) ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```
The script will build a docker image and attach a console.
When the docker image is running go to the "LedgerHQ-app-plugin-THORSwap" folder and build the ".elf" files.
```shell
cd LedgerHQ-app-plugin-THORSwap/tests       # go to the tests folder in LedgerHQ-app-plugin-THORSwap
./build_local_test_elfs.sh              # run the script build_local_test_elfs.sh
```

## Tests

To test the plugin go to the tests folder from the "LedgerHQ-app-plugin-THORSwap" and run the script "test"
```shell
cd LedgerHQ-app-plugin-THORSwap/tests       # go to the tests folder in LedgerHQ-app-plugin-THORSwap
yarn test                       # run the script test
```

## Loading on a physical device

This step will vary slightly depending on your platform.

Your physical device must be connected, unlocked and the screen showing the dashboard (not inside an application).

**Linux (Ubuntu)**

First make sure you have the proper udev rules added on your host :

```shell
# Run these commands on your host, from the app's source folder.
sudo cp .vscode/20-ledger.ledgerblue.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules 
sudo udevadm trigger
```

Then once you have [opened a terminal](#with-a-terminal) in the `app-builder` image and [built the app](#compilation-and-load) for the device you want, run the following command :

```shell
# Run this command from the app-builder container terminal.
make load    # load the app on a Nano S by default
```

[Setting the BOLOS_SDK environment variable](#compilation-and-load) will allow you to load on whichever supported device you want.

**macOS / Windows (with PowerShell)**

It is assumed you have [Python](https://www.python.org/downloads/) installed on your computer.

Run these commands on your host from the app's source folder once you have [built the app](#compilation-and-load) for the device you want :

```shell
# Install Python virtualenv
python3 -m pip install virtualenv 
# Create the 'ledger' virtualenv
python3 -m virtualenv ledger
```

Enter the Python virtual environment

* macOS : `source ledger/bin/activate`
* Windows : `.\ledger\Scripts\Activate.ps1`

```shell
# Install Ledgerblue (tool to load the app)
python3 -m pip install ledgerblue 
# Load the app.
python3 -m ledgerblue.runScript --scp --fileName bin/app.apdu --elfFile bin/app.elf
```
## Continuous Integration


The flow processed in [GitHub Actions](https://github.com/features/actions) is the following:

- Code formatting with [clang-format](http://clang.llvm.org/docs/ClangFormat.html)
- Compilation of the application for Ledger Nano S, S+ and X in [ledger-app-builder](https://github.com/LedgerHQ/ledger-app-builder)