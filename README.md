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

Then in the same folder clone two more repositories, which is the plugin-tools and app-ethereum.

```shell
git clone https://github.com/LedgerHQ/plugin-tools.git                          #plugin-tools
git clone --recurse-submodules https://github.com/LedgerHQ/app-ethereum.git     #app-ethereum
```
## Documentation

Need more information about the interface, the architecture, or general stuff about ethereum plugins? You can find more about them in the [ethereum-app documentation](https://github.com/LedgerHQ/app-ethereum/blob/master/doc/ethapp_plugins.adoc).

## Smart Contracts

Smart contracts covered by this plugin are:

| Network  |  Contract Name  | Address |
| ---      | ---             | ---     |
| Ethereum | TSSwapGeneric   | `0x213255345a740324cbCE0242e32076Ab735906e2`|
| Ethereum | TSLedgerAdapter | `0xaD545d047E34862121695F8E37aeEB71324a9E78`|
| Ethereum | THORChain_Router| `0xD37BbE5744D730a1d98d8DC97c42F0Ca46aD7146`|

## Methods

Methods covered by this plugin are:

| Conrtact Name | Selector | Method Name |
|      ---      |    ---   |     ---     |
| TSSwapGeneric    | 0xb5b4527a | swap |
| TSLedgerAdapter  | 0x4a95961e | ledgerCall (swapin)|
| THORChain_Router | 0x44bc937b | depositWithExpiry  |



## Build

Go to the plugin-tools folder and run the "./start" script.
```shell
cd plugin-tools  # go to plugin folder
./start.sh       # run the script start.sh
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
## Continuous Integration


The flow processed in [GitHub Actions](https://github.com/features/actions) is the following:

- Code formatting with [clang-format](http://clang.llvm.org/docs/ClangFormat.html)
- Compilation of the application for Ledger Nano S, S+ and X in [ledger-app-builder](https://github.com/LedgerHQ/ledger-app-builder)