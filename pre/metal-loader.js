/*
 * Metal - A webassembly kernel
 * Copyright (C) 2018 Paulo André Azevedo Quirino
 * SPDX-License-Identifier: GPL-2.0-only
 */

const metal = {
	worker: new Worker('./pre/metal.js'),

	write: function (str) {
		evt_head(1);
		evt_write_str(str);
		evt_finish();
	},

	run: function (path) {
		evt_head(4);
		evt_write_str(path);
		evt_finish();
	},

	shutdown: function () {
		evt_head(6);
		evt_finish();
	},
};

const start = 4;
const end = 16 * 1024;
const evt_len = end - start;
const shbuf = new SharedArrayBuffer(end);
const evt_n = new Int32Array(shbuf, 0, 1);
const evt_data = new Uint8Array(shbuf, start, end - start);

let i = 0;

metal.worker.postMessage(shbuf);

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
