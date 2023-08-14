import { processTest, populateTransaction } from "../test.fixture";

const contractName = "TSSwapGeneric";  // <= Name of the smart contract

const testLabel = "ethereum_swap_warning_received"; // <= Name of the test
const testDirSuffix = "swap_warning_received"; // <= directory to compare device snapshots to
const testNetwork = "ethereum";
const signedPlugin = false;

const contractAddr = "0x213255345a740324cbce0242e32076ab735906e2";   // <= Address of the smart contract
const chainID = 1;

// Mocked transaction with unknown token address
const inputData = "0xb5b4527a000000000000000000000000CACA0DEADBEF0C0FFEEC0FFEEC0FFEEC0FFEEDAD000000000000000000000000b8c77482e45f1f44de1745f52c74426c631bdd52000000000000000000000000865c59cf0e0bdb766dcd45cf55815fd6e5108315000000000000000000000000000000000000000000000000000000000001e078000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000e0000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000000080123456789abcdef0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008abcdef0123456789000000000000000000000000000000000000000000000000";

// Create serializedTx and remove the "0x" prefix
const serializedTx = populateTransaction(contractAddr, inputData, chainID);

const devices = [
    {
        name: "nanos",
        label: "Nano S",
        steps: 8, // <= Define the number of steps for this test case and this device
    },
    {
        name: "nanox",
        label: "Nano X",
        steps: 6, // <= Define the number of steps for this test case and this device
    },
    {
        name: "nanosp",
        label: "Nano S+",
        steps: 6, // <= Define the number of steps for this test case and this device
    }
];

devices.forEach((device) =>
    processTest(device, contractName, testLabel, testDirSuffix, "", signedPlugin, serializedTx, testNetwork)
);
