var socket;

function init() {
    socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    socket.onmessage = function(event){
        document.getElementById("rxConsole").value += event.data + "\n";
      }
}

function openFairing() {
    socket.send('open fairing');
}

function closeFairing() {
    socket.send('close fairing');
}

function sendText() {
    if(document.getElementById("txBar").value.length != 0){
        socket.send(document.getElementById("txBar").value);
        document.getElementById("txBar").value = "";
    }
}

function clearText() {
    document.getElementById("rxConsole").value = "";
}

function calibrate() {
    socket.send('calibrate');
}

function record() {
    socket.send('record');
}

function stopRecord() {
    socket.send('stop record');
}