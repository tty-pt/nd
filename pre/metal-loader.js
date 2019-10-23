/*
 * Metal - A webassembly kernel
 * Copyright (C) 2018 Paulo André Azevedo Quirino
 * SPDX-License-Identifier: GPL-2.0-only
 */

const worker = new Worker('./pre/metal.js');

function config(el) {
        handle_write1_bound = handle_write1.bind(null, el);
        worker.onmessage = handle_write1_bound;
}

const start = 4;
const end = 16 * 1024;
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
        const abuf = new Uint8Array(2);
        const enc = new TextEncoder();
        const bbuf = enc.encode(str);
        abuf[0] = str.length;
        abuf[1] = str.length >> 8;
        evt_write(abuf, abuf.length);
        evt_write(bbuf, bbuf.length);
}

function evt_finish() {
        Atomics.add(evt_n, 0, 1);
}

function handle_write1(el, message) {
        el.innerHTML += message.data;
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

function write(str) {
	evt_head(1);
	evt_write_str(str);
	evt_finish();
}

window.metal = {
	config,
        run,
        shutdown,
        write,
};
