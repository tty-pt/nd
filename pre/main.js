let ws = new WebSocket('wss://qnixsoft.com:4202', 'text');

let target = null;

const   dir_lbl = [ 'h', 'j', 'k', 'l', 'down', 'up' ],
        action_lbl = [ 'look', 'kill', 'shop', 'drink', 'open', 'chop', 'fill' ],

        term = document.querySelector('#term > pre'),
        form = document.querySelector('form'),
        input = document.querySelector('input'),
        forgetbtn = document.getElementById('forget'),
        contents_btns = document.getElementById('contents'),
        rtitle = document.querySelector('#map > div > h4'),
        rdesc = document.querySelector('#map > div > small'),
        map = document.querySelector('#map > pre'),
        dir_btns = dir_lbl.map(id => document.querySelector('#dir #' + id));
        act = document.getElementById('act'),
        target_lbl = act.children[0],
        action_btns = action_lbl.map(id => {
                let a = document.createElement('a');
                a.style.backgroundImage = 'url(art/' + id + '.png)'
                a.style.display = 'none';
                a.onclick = function () {
                        cmd(":" + id + " " + target.name);
                };
                act.appendChild(a);
                return a;
        });

function actions_reset() {
        act.style.display = 'none';
        target = null;
}

function cmd(str) {
        ws.send(str);
        actions_reset();
}

const nop_opacity = 0.2;

dir_btns.forEach(function(el) {
        el.onclick = cmd.bind(null, el.innerHTML);
        el.style.opacity = nop_opacity;
});

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

let actionable = {};

function actions_init(name) {
        act.style.display = 'block';
        target = actionable[name];
        target_lbl.innerHTML = target.pname;
        for (let i = 0; i < action_lbl.length; i++)
                action_btns[i].style.display
                        = (target.actions & (1 << i)) ? '' : 'none';
}

function dir_init(mask) {
        for (let i = 0; i < dir_lbl.length; i++)
                dir_btns[i].style.opacity
                        = (mask & 1 << i) ? 1 : nop_opacity;
}

function mcp_handler(j) {
        if (j.key.startsWith("com-qnixsoft-web-auth-error"))
                forget();
        else if (j.key.startsWith("com-qnixsoft-web-view"))
                map.innerHTML = j.data;
        else if (j.key.startsWith("com-qnixsoft-web-art"))
                output('<img class="ah" src="' + j.src + '">');
        else if (j.key.startsWith("com-qnixsoft-web-look-content")) {
                actionable[j.name] = j;
                contents_btns.innerHTML += j.icon.replace(/span/g, 'a')
                        .replace(/<a cl/, '<a onclick="actions_init(\'' + j.name + '\')" cl');
        } else if (j.key.startsWith("com-qnixsoft-web-look")) {
                if (j.room) {
                        rtitle.innerHTML = j.name;
                        rdesc.innerHTML = j.description;
                        contents_btns.innerHTML = '';
                        dir_init(j.exits);
                        actionable = {};
                }
        }
}

ws.onmessage = function (e) {
        let str = e.data;
        let arr = JSON.parse(str);
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
        cmd(input.value);
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

document.body.onkeypress = function(e) {
        if (document.activeElement == input)
                return;
        input.focus();
        if (e.key.length == 1)
                input.value += e.key;
};

window.onorientationchange = scroll_reset;

term.onclick = function (e) {
        term.classList = term.classList == '' ? 'small' : '';
        scroll_reset();
        actions_reset();
};

if (navigator.serviceWorker) {
        navigator.serviceWorker.register('sw.js').then(
                function(registration) {},
                function(err) { console.error('sw.js registration failed: ', err); },
        );
}
