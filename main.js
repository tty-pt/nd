import { strin, strout } from './wasm/metal-env.js';
import { mcp_init, mcp_proc, mcp_reset, mcp_input, memory } from '../wasm/cli/wasm-cli.js';
let ws = new WebSocket('wss://' + window.location.hostname + ':4202', 'text');

mcp_init();

let target = null;

let dir_lbl = [ 'h', 'j', 'k', 'l', 'down', 'up' ],
        action_lbl = [ 'look', 'kill', 'shop', 'drink', 'open', 'chop', 'fill' ],

        term = document.querySelector('#term'),
        form = document.querySelector('form'),
        input = document.querySelector('input'),
        modal = document.getElementById('modal'),
        forgetbtn = document.getElementById('forget'),
        contents_btns = document.getElementById('contents'),
        rtitle = document.querySelector('#title'),
        rdesc = document.querySelector('#description'),
        map = document.querySelector('#map'),
        dir_btns = dir_lbl.map(id => document.querySelector('#dir #' + id)),
        act = document.getElementById('act'),
        target_lbl = act.children[0],
        action_btns = action_lbl.map(id => {
                let a = document.createElement('a');
                a.style.backgroundImage = 'url(art/' + id + '.png)'
                a.classList.add("sh", "sv", "pxs");
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
        if (username && password) {
                login();
                help_hide();
        }
};

function scroll_reset() {
        term.scrollTop = term.scrollHeight;
}

function help_show() {
        modal.classList.add("f");
        modal.classList.remove("dn");
}

function help_hide() {
        modal.classList.add("dn");
        modal.classList.remove("f");
}


function output(stuff) {
        term.append(stuff);
        scroll_reset();
}

ws.onclose = function () {
        output("Socket connection closed\n");
        // input.disabled = true;
        forgetbtn.style.display = 'none';
};

if (!username)
        promptUsername();

let actionable = {};

function actions_init(name) {
        act.style.display = 'block';
        target = actionable[name];
        // target_lbl.innerHTML = target.pname;
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
        if (j.key == 'inband')
                output(j.data);
        if (j.key.startsWith("com-qnixsoft-web-auth-error"))
                forget();
        else if (j.key.startsWith("com-qnixsoft-web-view"))
                map.innerHTML = j.data;
        else if (j.key.startsWith("com-qnixsoft-web-art"))
                output('<img class="ah" src="' + j.src + '">');
        else if (j.key.startsWith("com-qnixsoft-web-look-content")) {
                actionable[j.name] = j;
                let className = "";
                let str = j.icon;
                let idx = str.indexOf("\"");

                if (idx >= 0) {
                        str = str.substr(idx + 1);
                        idx = str.indexOf("\"");
                        if (idx >= 0) {
                                className = str.substr(0, idx);
                                idx = str.indexOf(">");
                                if (idx >= 0) {
                                        str = str.substr(idx + 1, str.indexOf("<") - idx - 1);
                                }
                        }
                }

                const a = document.createElement("a");
                a.onclick = actions_init.bind(null, j.name);
                const icon = document.createElement("span");
                icon.innerHTML = str;
                icon.className = className;
                a.appendChild(icon);
                a.innerHTML += " " + j.name;
                contents_btns.appendChild(a);
        } else if (j.key.startsWith("com-qnixsoft-web-look")) {
                if (j.room) {
                        rtitle.innerHTML = j.name;
                        rdesc.innerHTML = j.description;
                        contents_btns.innerHTML = '';
                        dir_init(j.exits);
                        actionable = {};
                }
        } else if (j.key.startsWith("com-qnixsoft-web-meme")) {
                let a = document.createElement("a");
                output("\n" + j.who + " says:\n");
                a.href = j.url;
                let img = document.createElement("img");
                img.src = j.url;
                img.alt = j.url;
                img.onload = scroll_reset;
                a.appendChild(img);
                term.appendChild(a);
                output("\n");
        }
}

ws.onmessage = function (e) {
        // console.log("inserted", e.data);
        strin(memory, mcp_input(), e.data, 4096);
        let ptr = mcp_proc();
        if (ptr) {
                const str = strout(memory, ptr, 8282);
                // console.log('got', str);
                try {
                        const jsons = JSON.parse(str);
                        // console.log('json', jsons);
                        jsons.forEach(mcp_handler);
                } catch (e) {
                        console.warn(e);
                }
                mcp_reset();
        } // else
                // console.log('waiting');
};

function input_send(e) {
        let str = input.value;
        if (input.value.match("http")) {
                const url = str.substr(str.indexOf("h"));
                str = ":meme " + url; 
        }
        cmd(str);
        input.blur();
        input.value = "";
        return false;
};

input.onclick = function (e) {
        console.log("onclick", e);
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
        output("Thanks. What would your password be?\n");
        return false;
}

function focus() {
        input.focus();
        scroll_reset();
}

function promptUsername() {
        input.placeholder = "username";
        focus();
        form.onsubmit = gotUsername;
        output("Please insert username to login/register\n");
        return false;
}

if (username) {
        forgetbtn.style.display = 'inline-block';
        form.onsubmit = input_send;
} else {
        forgetbtn.style.display = 'none';
        form.onsubmit = gotUsername;
}

window.onkeydown = function(e) {
        help_hide();
        if (document.activeElement == input) {
                switch (e.key) {
                        case "Escape":
                                input.blur();
                }
                return;
        }
        switch (e.key) {
                case "s":
                        input.value = ":say ";
                        focus();
                        e.preventDefault();
                        break;
                case ":":
                        input.value = ":";
                        focus();
                        e.preventDefault();
                        break;
                case "a":
                        focus();
                        e.preventDefault();
                        break;
                case "ArrowUp":
                        input.value = "k";
                        input_send();
                        break;
                case "ArrowDown":
                        input.value = "j";
                        input_send();
                        break;
                case "ArrowLeft":
                        input.value = "h";
                        input_send();
                        break;
                case "ArrowRight":
                        input.value = "l";
                        input_send();
                        break;
                case "Shift":
                case "Control":
                case "Alt":
                        break;
                default:
                        input.value += e.key;
                        input_send();
        }
};

window.onorientationchange = scroll_reset;

// term.onclick = function (e) {
//         term.classList = term.classList == '' ? 'small' : '';
//         scroll_reset();
//         actions_reset();
// };

if (navigator.serviceWorker) {
        navigator.serviceWorker.register('sw.js').then(
                function(registration) {},
                function(err) { console.error('sw.js registration failed: ', err); },
        );
}
