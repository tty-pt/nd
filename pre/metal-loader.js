/*
 * Metal - A webassembly kernel
 * Copyright (C) 2018 Paulo André Azevedo Quirino
 * SPDX-License-Identifier: GPL-2.0-only
 */

const worker = new Worker('./pre/metal.js');

// function set_tty(el) {
        // el.onkeydown = handle_keydown;
        // el.onkeyup = handle_keyup;
        // el.focus();
        // handle_write1_bound = handle_write1.bind(null, el);
        // worker.onmessage = handle_write1_bound;
// }

const start = 4;
const end = 2 * 1024;
const evt_len = end - start;
const shbuf = new SharedArrayBuffer(end);
let i = 0;

worker.postMessage(shbuf);

const evt_n = new Int32Array(shbuf, 0, 1);
const evt_data = new Uint8Array(shbuf, start, end - start);

function evt_head(flags) {
        const next_i = i + 1;

        Atomics.store(evt_data, i, flags);

        if (next_i >= evt_len)
                i = 0;

        else
                i++;
}

function evt_write(data, len) {
        let j;

        if (i + len >= evt_len)
                i = 0;

        for (j = 0; j < len; j++, i++)
                Atomics.store(evt_data, i, data[j]);

        // return j;
}

function evt_write_str(str) {
        const abuf = new Uint8Array(1);
        const enc = new TextEncoder();
        const bbuf = enc.encode(str);
        abuf[0] = str.length;

        evt_write(abuf, abuf.length);
        evt_write(bbuf, bbuf.length);
}

function evt_finish() {
        Atomics.add(evt_n, 0, 1);
}

const keys = {
        Escape: 1,
        Digit1: 2,
        Digit2: 3,
        Digit3: 4,
        Digit4: 5,
        Digit5: 6,
        Digit6: 7,
        Digit7: 8,
        Digit8: 9,
        Digit9: 10,
        Digit0: 11,
        Minus: 12,
        Equal: 13,
        Backspace: 14,

        // alpha
        Tab: 15, // 0xff09
        KeyQ: 16, // 0xff71
        KeyW: 17, // 0xff77
        KeyE: 18, // 0xff65
        KeyR: 19, // 0xff72
        KeyT: 20, // 0xff74
        KeyY: 21, // 0xff79
        KeyU: 22, // 0xff75
        KeyI: 23, // 0xff69
        KeyO: 24, // 0xff6f
        KeyP: 25, // 0xff70
        BracketLeft: 26,
        BracketRight: 27,
        Enter: 28, // 0xff0d

        ControlLeft: 29, // 0xffe3
        KeyA: 30, // 0xff61
        KeyS: 31, // 0xff73
        KeyD: 32, // 0xff64
        KeyF: 33, // 0xff66
        KeyG: 34, // 0xff67
        KeyH: 35, // 0xff68
        KeyJ: 36, // 0xff6a
        KeyK: 37, // 0xff6b
        KeyL: 38, // 0xff6c
        Semicolon: 39, // 0xff6c
        Quote: 40, // 0xff6c
        Backquote: 41, // 0xff6c

        ShiftLeft: 42, // 0xffe1
        Backslash: 43,
        KeyZ: 44, // 0xff7a
        KeyX: 45, // 0xff78
        KeyC: 46, // 0xff63
        KeyV: 47, // 0xff76
        KeyB: 48, // 0xff62
        KeyN: 49, // 0xff6e
        KeyM: 50, // 0xff6d
        Comma: 51,
        Period: 52,
        Slash: 53,
        ShiftRight: 54,

        // KpAsterisk: 55,
        AltLeft: 56,
        Space: 57,
};

function handle_write0(key, key_up) {
        const code = keys[key]; // TODO consistency
        if (code !== undefined) {
                const abuf = new Uint8Array(2);
                abuf[0] = key_up;
                abuf[1] = keys[key];
                evt_head(1);
                evt_write(abuf, 2);
                evt_finish();
                return false;
        }
        return true;
        // const abuf = new Uint8Array(1);
        // abuf[0] = key_up;
        // evt_write(abuf, 1);
        // evt_write_str(key);
        // evt_finish();
        // return false;
}

// TODO caps lock is not detected. In the future,
// make sure to send modifier state on init
// TODO use key instead, map to standard keyboard codes and write

// function handle_keydown(e) { return handle_write0(e.code, 0); }
// function handle_keyup(e) { return handle_write0(e.code, 1); }

function handle_write1(el, message) {
        el.innerHTML = el.innerHTML + message.data + '<br />';
}

let handle_write1_bound;

function run(path) {
        evt_head(4);
        evt_write_str(path);
        evt_finish();
}

function shutdown() {
        evt_head(6);
        evt_finish();
}

function write0(str) {
}

window.metal = {
        run,
        shutdown,
        write0,
};
