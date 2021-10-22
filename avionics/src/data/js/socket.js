var height = document.getElementById('height');
var speed = document.getElementById('speed');
var apogee = document.getElementById('apogee');
var state = document.getElementById('state');
var history_height = 0;
var mess = '';

function socket_receive() {
    socket.onmessage = function (event) {
        var data = event.data.split(',');

        // Type:data[0], Time:data[1], Height:data[2], Speed:data[3], Status:data[4]
        if (data[0] == 's' || data[0] == 'f') {
            height.innerHTML = data[2];
            speed.innerHTML = data[3];
            if (data[4] == '0') state.innerHTML = 'Unknown';
            else if (data[4] == '1') state.innerHTML = 'Rising';
            else if (data[4] == '2') state.innerHTML = 'Falling';

            if (parseFloat(data[2]).toFixed(2) > history_height) {
                apogee.innerHTML = data[2];
                history_height = parseFloat(data[2]).toFixed(2);
            }
        }
        if (data[0] != 's') {
            if(data[0] == '\n')
                textarea.value += event.data.substring(2, event.data.length-1) + "\n";
            else
                textarea.value += 'R: ' + event.data + "\n";
            textarea.scrollTop = textarea.scrollHeight;
        }
    }
}

function rbf_send() {
    socket.send('RBF');
}

function angle_send(angle) {
    socket.send('angle' + angle);
}

function init_send() {
    socket.send('init');
}

function stall_send() {
    socket.send('stall');
}

function preLaunch_send() {
    socket.send('preLaunch');
}

function launch_send() {
    socket.send('launch');
}

function abort_send() {
    socket.send('abort');
}

function message_send(message) {
    socket.send(message);
}

function parachute_send() {
    socket.send('open');
}