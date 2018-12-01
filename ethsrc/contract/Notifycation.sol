pragma solidity ^0.4.24;

contract Test2 {

    event Notify(address indexed _sender, string msg);

    string public notification = "nothind";

    function notify(string _notification) external {
        notification = _notification;
        emit Notify(msg.sender, _notification);
    }

}


interface Service {
     function notify(string _notification) external;
}

contract Test3 {

    Service public test2Instance = Service(0x1e342db1d7557b1cb488a30b4d3615c2e9759b25);

    function callNotify(string _notify) external {
        test2Instance.notify(_notify);
    }
}