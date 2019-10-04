let ws = new WebSocket('wss://qnixsoft.com:4202', 'text');

const   term = document.querySelector('#term > pre'),
        form = document.querySelector('form'),
        input = document.querySelector('input'),
        forgetbtn = document.getElementById('forget'),
        map = document.querySelector('#map > pre');

let username = localStorage.getItem('username');
let password = localStorage.getItem('password');

function login() {
        localStorage.setItem('username', username);
        localStorage.setItem('password', password);
        ws.send(username + ' ' + password);
        forgetbtn.style.display = 'inline-block';
}

function forget() {
        localStorage.removeItem('username');
        localStorage.removeItem('password');
        location.reload();
}

forgetbtn.onclick = forget;

ws.binaryType = 'arraybuffer';

ws.onopen = function () {
        input.disabled = false;
        if (username && password)
                login();
};

function scroll_reset() {
        term.scrollTop = term.scrollHeight;
}

function output(stuff) {
        term.innerHTML += stuff;
        scroll_reset();
}

ws.onclose = function () {
        output("Socket connection closed\n");
        input.disabled = true;
        forgetbtn.style.display = 'none';
};

if (!username)
        promptUsername();

function mcp_handler(j) {
        if (j.key.startsWith("com-qnixsoft-web-auth-error"))
                forget();
        else if (j.key.startsWith("com-qnixsoft-web-view"))
                map.innerHTML = j.value;
        else if (j.key.startsWith("com-qnixsoft-web-art")) {
                output('<img class="ah" src="' + j.value + '">');
        }
}

ws.onmessage = function (e) {
        let str = e.data;
        let arr = JSON.parse(str);
        console.log('received', arr);
        for (let k in arr) {
                if (arr[k].key == 'inband')
                        output(arr[k].value);
                else
                        mcp_handler(arr[k]);
        }
        if (term.innerHTML.length > 8000)
                term.innerHTML = term.innerHTML.substr(4000);
};

function input_send(e) {
        ws.send(input.value);
        input.value = "";
        return false;
};

function gotPassword() {
        password = input.value;
        input.type = 'text';
        input.placeholder = input.value = "";

        login();
        form.onsubmit = input_send;
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
        form.onsubmit = gotUsername;
        return false;
}

if (username) {
        forgetbtn.style.display = 'inline-block';
        form.onsubmit = input_send;
} else {
        forgetbtn.style.display = 'none';
        form.onsubmit = gotUsername;
}

navigator.serviceWorker.register('sw.js').then(
        function(registration) {},
        function(err) { console.error('sw.js registration failed: ', err); },
);

window.addEventListener('orientationchange', scroll_reset);
