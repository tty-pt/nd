function _jsnstr(buf, l) {
        let s = "";
        let i;

        for (i = 0; i < l; i++)
                s += String.fromCharCode(buf[i]);

        return s;
}

function jsnstr(mem, p, l) {
        return _jsnstr(new Uint8Array(mem, p, l), l);
}

let memory;

function _console_log(p, l) {
        console.log(jsnstr(memory.buffer, p, l));
}

function bcpy(bufd, bufo, d, o, len) {
        try {
                (new Uint8Array(bufd, d, len))
                        .set(new Uint8Array(bufo, o, len));

                return len;
        } catch (e) {
                console.error(e);
                return -1;
        }
}

function bmemcpy(memd, memo, d, o, len) {
        return bcpy(memd.buffer, memo.buffer, d, o, len);
}

let progs = { 0: 'kern' }

function umem(pid, d, o, len) {
        return bmemcpy(memory, progs[pid].memory, d, o, len);
}

function kmem(pid, d, o, len) {
        return bmemcpy(progs[pid].memory, memory, d, o, len);
}

let shbuf;

function js_emem(ofs, p, p_len) {
        return bcpy(memory.buffer, shbuf, p, ofs, p_len);
}

function init(_shbuf) {
        console.log('shmem_init');
        shbuf = _shbuf;
        evt_n = new Int32Array(shbuf, 0, 1);
        self.onmessage = null;
}

let got_events = new Promise(resolve => {
        self.onmessage = event => resolve(init(event.data));
});

let evt_n;

function evt_count(ms) {
        if (Atomics.wait(evt_n, 0, 0, ms) == 'timed-out')
                return -1;

        return Atomics.exchange(evt_n, 0, 0);
}

function js_run(ptr, len) {
        const buf = new Uint8Array(memory.buffer, ptr, len);
        const path = _jsnstr(buf, len);

        console.log('js run', path);
        run(path);
}

function flush(p, l) {
        const str = jsnstr(memory.buffer, p, l);
        console.log(str);
}

function js_shutdown(e) {}
function js_sleep() {}
function tty_read() {}
const kenv = {
        _console_log,
        flush,
        umem,
        kmem,

        js_shutdown,
        js_sleep,
        tty_read,

        js_emem,
        evt_count,
        js_run,

        memory: new WebAssembly.Memory({
                initial: 1,
                maximum: 32,
        }),
};

function wasm_load(path, env) {
        return fetch(path)
                .then(res => res.arrayBuffer())
                .then(buf => {
                        const mod = new WebAssembly.Module(buf)
                        return (new WebAssembly.Instance(mod, { env })).exports;
                });
}

let syscall, evt_loop;

const booted = wasm_load(
        '/metal/bin/metal.wasm',
        kenv,
).then(exports => {
        memory = exports.memory;
        syscall = {
                __syscall0: exports.__syscall0,
                __syscall1: exports.__syscall1,
                __syscall2: exports.__syscall2,
                __syscall3: exports.__syscall3,
                __syscall4: exports.__syscall4,
                __syscall5: exports.__syscall5,
                __syscall6: exports.__syscall6,
        };
        console.log('metal exports', exports, syscall);
        exports.start_kernel();
        evt_loop = exports.evt_loop;
        evt_loop();
        return syscall;
});

let next_pid = 1;

function run(path) {
        let prog = {};
        let dead;

        progs[next_pid] = prog;
        next_pid++;

        return wasm_load(path, syscall).then(exports => {
                console.log('loaded program', path, exports);
                dead = exports.dead;
                prog.memory = exports.memory;
                exports._start(exports.argc);
                dead();
        }).catch(err => {
                // 'unreachable executed'
                if (err.message == 'unreachable' && dead)
                        dead();
                else
                        setTimeout(function () { throw err; }, 0);
        }).finally(evt_loop);
}
