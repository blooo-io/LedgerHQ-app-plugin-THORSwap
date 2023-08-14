import { processTest, populateTransaction } from "../test.fixture";

const contractName = "THORChain_Router";  // <= Name of the smart contract

const testLabel = "ethereum_depositWithExpiry_normal_flow"; // <= Name of the test
const testDirSuffix = "depositWithExpiry_normal_flow"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0xd37bbe5744d730a1d98d8dc97c42f0ca46ad7146";   // <= Address of the smart contract
const chainID = 1;

// From https://etherscan.io/tx/0x1976d1e552bd2b75502f85bf1ecc501ef06a7fabe5752631f5417870d420b89d
const inputData = "0x44bc937b000000000000000000000000e6a30f4f3bad978910e2cbb4d97581f5b5a0ade0000000000000000000000000dac17f958d2ee523a2206206994597c13d831ec7000000000000000000000000000000000000000000000000000000003b9aca0000000000000000000000000000000000000000000000000000000000000000a00000000000000000000000000000000000000000000000000000018949b85fc100000000000000000000000000000000000000000000000000000000000000413d3a4254432e4254433a626331717133676435396c3434337a6c307068356d306a6d61766d6377706c7674687278656c326732743a333133373238383a743a333000000000000000000000000000000000000000000000000000000000000000";

// Create serializedTx and remove the "0x" prefix
const serializedTx = populateTransaction(contractAddr, inputData, chainID);

const devices = [
    {
        name: "nanos",
        label: "Nano S",
        steps: 5, // <= Define the number of steps for this test case and this device
    },
    {
        name: "nanox",
        label: "Nano X",
        steps: 5, // <= Define the number of steps for this test case and this device
    },
    {
        name: "nanosp",
        label: "Nano S+",
        steps: 5, // <= Define the number of steps for this test case and this device
    }
];

devices.forEach((device) =>
    processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
