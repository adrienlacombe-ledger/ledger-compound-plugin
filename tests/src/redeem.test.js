import "core-js/stable";
import "regenerator-runtime/runtime";
import { processTest} from './test.fixture';

// EDIT THIS: Replace with your contract address
const contractAddr = "0x70e36f6bf80a52b3b46b3af8e106cc0ed743e8e4";
// EDIT THIS: Replace `boilerplate` with your plugin name
const abi_path = '../compound/abis/' + contractAddr + '.json';
const rawTxHex = "0xf88b8211ce850df8475800830927c09470e36f6bf80a52b3b46b3af8e106cc0ed743e8e480a4db006a75000000000000000000000000000000000000000000000000000002af8edf610725a084e5076df6cc7bfb749d08da27cd031ab959ac4c58a0b3a314f5106b6ae33da9a030c16cd471d720a7420dde7ef109def72ebfa113a3c41e38b5eaddf9c3a63fcf";
const testLabel = "repay_borrow_on_behalf";
const testNetwork = "ethereum";
const signedPlugin = false;
const contractName = "Compound"
const devices = [
  {
    name: "nanos",
    label: "nano S",
    steps: 6
  }
];
// Reference transaction for this test:
// https://etherscan.io/tx/0x2caf4f71c3766b94f1d662df3da04b45e9b6d1ddca1601950471375f3d1cdeab

devices.forEach((device) =>  processTest(device, contractName, testLabel, rawTxHex, signedPlugin,"",testNetwork));