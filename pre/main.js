/* based on websockify js demo code:
 *      VT100.js - GNU LGPL v2.1 by Frank Bi <bi@zompower.tk>
 *      wstelnet.js - 
 * No copyright infringement intended. GPLv2 <q@qnixsoft.com>
 */

let websocket = new WebSocket('wss://qnixsoft.com:4202', 'text');

const   term = document.getElementById('terminal'),
        form = document.querySelector('form'),
        input = document.querySelector('input'),
        forget = document.getElementById('forget');

let username = localStorage.getItem('username');
let password = localStorage.getItem('password');

function login() {
        localStorage.setItem('username', username);
        localStorage.setItem('password', password);
        websocket.send(username + ';' + password);
        forget.style.display = 'inline-block';
}

forget.onclick = function () {
        localStorage.removeItem('username');
        localStorage.removeItem('password');
        location.reload();
};

websocket.binaryType = 'arraybuffer';

websocket.onopen = function () {
        input.disabled = false;
        if (username && password)
                login();
};

function output(stuff) {
        term.innerHTML += stuff;
        term.parentNode.scrollTop = term.parentNode.scrollHeight;
}

websocket.onclose = function () {
        output("Socket connection closed\n");
        input.disabled = true;
        forget.style.display = 'none';
};

if (!username)
        promptUsername();

websocket.onmessage = function (e) {
        output(e.data);
};

function send(e) {
        websocket.send(input.value);
        input.value = "";
        return false;
};

function gotPassword() {
        password = input.value;
        input.type = 'text';
        input.placeholder = input.value = "";

        login();
        form.onsubmit = send;
        return false;
}

function gotUsername() {
        username = input.value;
        input.value = "";
        input.placeholder = input.type = 'password';
        form.onsubmit = gotPassword;
        return false;
}

function promptUsername() {
        input.placeholder = "username";
        password = null;
        form.onsubmit = gotUsername;
        return false;
}

if (username) {
        forget.style.display = 'inline-block';
        form.onsubmit = send;
} else {
        forget.style.display = 'none';
        form.onsubmit = gotUsername;
}

navigator.serviceWorker.register('sw.js').then(
        function(registration) {},
        function(err) { console.error('sw.js registration failed: ', err); },
);
