const Web3 = require("web3");
const abi = [
    {
        "anonymous": false,
        "inputs": [
            {
                "indexed": true,
                "name": "_sender",
                "type": "address"
            },
            {
                "indexed": false,
                "name": "msg",
                "type": "string"
            }
        ],
        "name": "Notify",
        "type": "event"
    }
]
//如果不用事件监听用这个
// let web3 = new Web3("https://rinkeby.infura.io/v3/e8947e207ac142519554d382200e663b");
//如果需要事件监听,必须用这个wss来进行
let web3 = new Web3(new Web3.providers.WebsocketProvider("wss://rinkeby.infura.io/ws/v3/e8947e207ac142519554d382200e663b"));
const contractAddress = '0x1e342db1d7557b1cb488a30b4d3615c2e9759b25';
let ins = new web3.eth.Contract(abi,contractAddress);
ins.events.Notify(function(err,res){
    console.log(err,res);
})