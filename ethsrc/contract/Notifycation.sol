pragma solidity ^0.4.24;

contract Test2 {

    event Notify(address indexed _sender, string msg);

    string public notification = "nothind";

    function notify(string _notification) external {
        notification = _notification;
        emit Notify(msg.sender, _notification);
    }

}