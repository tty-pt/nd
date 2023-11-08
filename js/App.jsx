import React, { useState, useEffect, useRef } from "react";
import { Terminal as XTerm } from 'xterm';
// import { WebglAddon } from 'xterm-addon-webgl';
import { FitAddon } from 'xterm-addon-fit';
import { WebLinksAddon } from 'xterm-addon-web-links';
// import { OverlayAddon } from './addons/overlay';
import { usePopper } from 'react-popper';
import { useMagic, withMagic, makeMagic } from "@tty-pt/styles";
import { Sub, reflect } from "@tty-pt/sub";
import ReactDOM from "react-dom";
import ACTIONS, { ACTIONS_LABEL } from "./actions";
import tty_proc from "./tty";
// import canvas from "./canvas";
import 'xterm/css/xterm.css';
import "./vim.css";
const baseDir = process.env.CONFIG_BASEDIR || "";
let do_echo = true;
let raw = false;

function echo(...args) {
  console.log(...args);
  return args[args.length - 1];
}

const fitAddon = new FitAddon();
// const overlayAddon = new OverlayAddon();
function resize(cols, rows) {
  const IAC = 255;
  const SB = 250;
  const NAWS = 31;
  const SE = 240;

  // Convert the column and row numbers into high byte and low byte.
  const colsHighByte = cols >> 8;
  const colsLowByte = cols & 0xFF;
  const rowsHighByte = rows >> 8;
  const rowsLowByte = rows & 0xFF;

  // Create a buffer for the command.
  const nawsCommand = new Uint8Array([
    IAC, SB, NAWS,
    colsHighByte, colsLowByte,
    rowsHighByte, rowsLowByte,
    IAC, SE
  ]);

  // Send the command over the WebSocket.
  wsSub.value.ws?.send(nawsCommand);
}

class TermSub extends Sub {
  constructor() {
    super(null);
  }

  @reflect()
  init(parent) {
    const term = new XTerm({
      fontSize: 13,
      fontFamily: 'Consolas,Liberation Mono,Menlo,Courier,monospace',
      theme: {
        foreground: '#93ada5',
        background: 'rgba(0, 0, 0, 0.2)',
        cursor: '#73fa91',
        black: '#112616',
        red: '#7f2b27',
        green: '#2f7e25',
        yellow: '#717f24',
        blue: '#2f6a7f',
        magenta: '#47587f',
        cyan: '#327f77',
        white: '#647d75',
        brightBlack: '#3c4812',
        brightRed: '#e08009',
        brightGreen: '#18e000',
        brightYellow: '#bde000',
        brightBlue: '#00aae0',
        brightMagenta: '#0058e0',
        brightCyan: '#00e0c4',
        brightWhite: '#73fa91',
      },
      allowProposedApi: true,
    });

    term.loadAddon(fitAddon);
    term.loadAddon(new WebLinksAddon());
    term.open(parent);
    term.inputBuf = "";
    term.perm = "";

    term.onResize(({ cols, rows }) => {
      console.log("RESIZE", cols, rows);
      resize(cols, rows);
    });

    term.element.addEventListener("focusin", () => {
      term.focused = true;
    });
    term.element.addEventListener("focusout", () => {
      term.focused = false;
    });
    if (isMobile) {
      term.element.addEventListener("input", function (event) {
        switch (event.inputType) {
          case "deleteContentBackward":
            term.inputBuf = term.inputBuf.slice(0, term.inputBuf.length - 2);
            return true;
          case "deleteContentForward":
          case "deleteByCut":
            return true;
        }
        if (event.data !== " ")
          term.inputBuf = event.data;
        else
          term.perm = term.inputBuf;
        // console.log("input", term.inputBuf);
        term.lastInput = true;
        return true;
      });
    }
    term.onKey(event => {
      console.log("term.onKey", event, event.key);
      if (event.key === "\r") {
        if (isMobile) {
          const msg = (term.perm ? term.perm + " " : "") + term.inputBuf;
          term.write(msg + "\r\n");
          sendMessage(msg);
          term.perm = "";
        } else {
          if (echo) term.write("\r\n");
          wsSub.value.ws.send(term.inputBuf);
          // sendMessage(term.inputBuf);
          // const initialSize = term.getSize();
        }
        term.inputBuf = "";
      } else if (!do_echo) {
        if (raw)
          sendMessage(event.key);
        else
          term.inputBuf += event.key;
        return;
      } else if (event.key === "\u001b")
        term.blur();
      else if (event.key === "\u001b[A")
        sendMessage("k");
      else if (event.key === "\u001b[1;2A")
        sendMessage("K");
      else if (event.key === "\u001b[B")
        sendMessage("j");
      else if (event.key === "\u001b[1;2B")
        sendMessage("J");
      else if (event.key === "\u001b[D")
        sendMessage("h");
      else if (event.key === "\u001b[C")
        sendMessage("l");
      else if (event.key === "\u007f") {
        term.write("\b \b");
        term.inputBuf = term.inputBuf.slice(0, term.inputBuf.length - 1);
      } else {
        if (!isMobile && do_echo)
          term.write(event.key);
        term.inputBuf += event.key;
      }
      term.lastInput = false;
    });
    fitAddon.fit();
    return term;
  }
}

const termSub = new TermSub();
const isMobile = window.navigator.maxTouchPoints > 1;

makeMagic({
  "?pre": {
    margin: 0,
  },
  opacity2: {
    opacity: 0.2,
  },
  target: {
    minWidth: "300px",
    minHeight: "33vh",
  },
  "?#map": {
    maxWidth: "none",
    width: "200px",
    fontFamily: "monospace",
    whiteSpace: "pre",
  },
  deepShadow: {
    textShadow: "2px 2px 3px black, -2px 2px 3px black, -2px -2px 3px black, 2px -2px 3px black",
  },
  round: {
    lineHeight: "32px",
    width: "calc(32px + 2 * 8px)",
    height: "calc(32px + 2 * 8px)",
    fontWeight: 900,
    "& > img": {
      width: "32px",
      height: "32px",
      imageRendering: "pixelated",
    },
  },
  backgroundSemi: {
    backgroundColor: "color-mix(in srgb, #3c403c, transparent 50%)",
  },
  modal: {
    top: 0,
    left: 0,
    right: 0,
    bottom: 0,
  },
  sizeSuper: {
    width: "256px",
    height: "256px",
  },
  focus: {
    border: "solid thin #9589c5 !important",
  },
});

class Modal extends React.Component {
	constructor(props) {
		super(props);
		// this.ref = React.createRef();
		this.el = props.el;
	}

	componentDidMount() {
		this.el.onclick = e => {
			if (e.target == this.el)
				this.props.setOpen(false);
		};
		this.el.className = 'modal abs vertical-0 align-items c fcc oh f';
		// this.el.classList.remove('dn');
	}

	componentWillUnmount() {
		this.el.classList.add('dn');
	}

	onKeyDown(e) {
		if (e.keyCode == 27) // escape
			this.props.setOpen(false);
	}

	render() {
		return ReactDOM.createPortal((<span
			onKeyDown={e => this.onKeyDown(e)}
			onClick={e => e.stopPropagation()}
			className="r bo0 focus background-semi p oa"
		>
			{ this.props.children }
		</span>), this.el);
	}
}

const modal = document.querySelector('.modal');

function _useModal(el, Component, props = {}) {
	const [ isOpen, setOpen ] = useState(false);

	const modal = isOpen ? (
		<Modal setOpen={setOpen} el={el}>
			<Component
				{ ...props }
				close={() => setOpen(false)}
			/>
		</Modal>
	) : null;

	return [ modal, isOpen, setOpen ];
}

const useModal = _useModal.bind(null, modal);

// window.onorientationchange = scroll_reset;

const atiles = [
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQklEQVQ4jWNgGL7g4OFb/9ExWZpwYZyaiXEBhiH4NBNSA+fg8wpWW5H1ETIAl98xvEHIEJyacfmdEB8vICsdjFAAAGW58imbroFwAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jWNgoAY4ePjW/4OHb/0nWw+MQ6whWNUTawhedYQMIcoSXIpI8ia6YlLDCMMQcmKJdGdTxQW4/IyNT7KziXYJMemAoBewOROfHG5BAi4lRT1OAAA/Xu7MVtQQRgAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAOUlEQVQ4jWNgGJTg4OFb/3FhkjXjEiNaMzFyWBXB+Li8RHsDKPYCIZfQxwCSvYBLIcWJiSTNQw8AAO8uLsItXTaGAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgIBIcPHzrPzImWREyn2wDyHIBuhhJBhDUgM8QdDZJmim2nWwDcBlCkmZ0Q8iynSRN2AwYGP9T3QAYnyQDyAEAd7gQfVonw9EAAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAVElEQVQ4jaXRywkAMAgDUPffStzCZey1FE3UCt7Cw48IKTUPNQ+Wo8AauYEV8gIQycJVQ6ALwwlWq7DAGEBIegO07+iV3wBDWgBC2kCFjIAMGQOoDj9EP5M9YZdfAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGLTg4OFb/5ExyRoGjwHIcmQZgi5HsiHY5EgyBJccQUOIsWBwGEKRAcMEAABAgauIKxMDjQAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAUElEQVQ4jWNgwAEOHr71H5cc0QBmCEWGUcUlZNuMjskyBJmmnwHYnEySNw4evvX/////TLhcRFAzJfLUMYAQxmsAIUMIaqaKAdgMIkkjKQAABfenzXQV7xsAAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcwWQYQkiPJFQQNgClGp8lOraMANwAA27bDE5yTd30AAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAM0lEQVQ4jWNgGFTg4OFb/4nFOA0g1iK8EoRsHhgXkGQAxS6gyACKo5FYV1DkBeprHtoAAOijqCv2+qTkAAAAAElFTkSuQmCC",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jc3QwQ0AMAQFUPtvJbawjF4qaUVQcair/18EwB4kFiQW6IyW24gF/kKewQwpQWNABJcBWxq7IgU03C5HUH2ZZJBYwrPOx1no6mWI3bv5yiVebgFV9xc3cxaEggAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAALElEQVQ4jWNgGF7g4OFb/5ExsXI4FeHDRLuCJM3EGEJWWIxEA3AZQrTmEQYAvLnktS1HPJUAAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcw0Qaga6KaKwgaAFNMtmZ8BtHPAOSAI9sgipI2ukuwAQABHugSAyRIzQAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNgoDY4ePjWf7LkYRLoNLoasg2AaR6iBiBrpq0BuBQOEQPQFeIzHKcBMMWEXEeyAfhcR7Iisg3AJQcA7LE1fLFqxRoAAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgoCU4ePjWfximSDNZhlDFAGw02a4gWTNJtuOzBZ84hhwxziXKS9gUURyVJGlEt5EkQwgFKE6DSLGForRBdQAAjPakzh7dkTsAAAAASUVORK5CYII=",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGFTg4OFb/4nBBA0hRw7FBWTJE+U8fOqI0YxX7agBVDKAomhElqQoKdMdAADVVJq360zbaQAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAASUlEQVQ4jWNgoBU4ePjWf2RMsSFUcwFRLkNXgG4ANjbRzsZrAC7N+LxDlM0EXUOKs3GKE+tsYgwmLEkzA0hNvlgNIKSJ9gaQigF3uf+dQNoz9QAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNgoDU4ePjWf4o0wzDVNCOL4zQYnwJkMawGEDIdqwEwBqnORjGAWGdjFSfF2QQNwOYdUg0mLEkzA4hKJIQMIKSJ9gaQigFrg/+dXV9AmAAAAABJRU5ErkJggg==",
	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAEklEQVQ4jWNgGAWjYBSMAggAAAQQAAF/TXiOAAAAAElFTkSuQmCC"
];

const wsSub = new Sub({
  ws: null,
  unsubscribe: () => {},
});

function sendMessage(text) {
  wsSub.value.ws.send(text);
}

function onOpen() {
  const term = termSub.value;
  resize(term.cols, term.rows);

  if (process.env.development)
    sendMessage("auth test");
  else
    sendMessage("auth " + getCookie("QSESSION"));
}

const decoder = new TextDecoder('utf-8');

function ab2str(arr) {
  return decoder.decode(arr);
}

function read_16(arr, start) {
  return ((arr[start + 1] << 8) | arr[start]) << 32;
}

function read_32(arr, start) {
  return ((arr[start + 3] << 24) | (arr[start + 2] << 16) | (arr[start + 1] << 8) | arr[start]) << 32;
}

function read_u32(arr, start) {
  return read_32(arr, start) >>> 0;
}

// function read_64(arr, start) {
//   return ((arr[start + 7] << 56) | (arr[start + 6] << 48) | (arr[start + 5] << 40) | (arr[start + 4] << 32) | (arr[start + 3] << 24) | (arr[start + 2] << 16) | (arr[start + 1] << 8) | arr[start]);
// }

function read_string(arr, start, ret = {}) {
  const slice = arr.slice(start);
  let len = 0;

  for (const item of slice) {
    if (item === 0)
      break;
    len ++;
  }

  const rret = String.fromCharCode.apply(null, slice.slice(0, len));

  ret[start] = len;
  return rret;
}

function onMessage(ev) {
  const arr = new Uint8Array(ev.data);
  if (String.fromCharCode(arr[0]) == "#" && String.fromCharCode(arr[1]) == "b") {
    const iden = arr[2];
    switch (iden) {
      case 1: { // BCP_BARS
        let aux;
        barsEmit({
          hp: read_32(arr, aux = 3),
          hpMax: read_32(arr, aux += 4),
          mp: read_32(arr, aux += 4),
          mpMax: read_32(arr, aux += 4),
        });
        return;

      } case 1: {// BCP_EQUIPMENT
        let aux;
        barsEmit({
          head: read_32(arr, aux = 3),
          neck: read_32(arr, aux += 4),
          chest: read_32(arr, aux += 4),
          back: read_32(arr, aux += 4),
          rhand: read_32(arr, aux += 4),
          lfinger: read_32(arr, aux += 4),
          rfinger: read_32(arr, aux += 4),
        });
        return;

      } case 3: {// BCP_STATS
        let aux;
        statsEmit({
          str: read_32(arr, aux = 3),
          con: read_32(arr, aux += 4),
          dex: read_32(arr, aux += 4),
          int: read_32(arr, aux += 4),
          wiz: read_32(arr, aux += 4),
          cha: read_32(arr, aux += 4),
          hp: read_16(arr, aux += 4),
          mov: read_16(arr, aux += 2),
          mdmg: read_16(arr, aux += 2),
          mdef: read_16(arr, aux += 2),
          dodge: read_16(arr, aux += 2),
          dmg: read_16(arr, aux += 2),
          def: read_16(arr, aux += 2),
        });
        return;

      } case 4: // BCP_ITEM
        let aux, base, ret = {};

        base = {
          dbref: read_32(arr, aux = 3),
          loc: read_32(arr, aux += 4),
          dynflags: arr[aux += 4],
          type: arr[aux += 1],
          actions: read_32(arr, aux += 1),
          name: read_string(arr, aux += 4, ret),
          pname: read_string(arr, aux += ret[aux] + 1, ret),
          icon: read_string(arr, aux += ret[aux] + 1, ret),
          art: read_string(arr, aux += ret[aux] + 1, ret),
        };

        switch (base.type) {
          case 0: // TYPE_ROOM
            base.exits = read_32(arr, aux += ret[aux] + 1);
            break;
          case 3: // TYPE_ENTITY
            base.flags = read_u32(arr, aux += ret[aux] + 1);
            break;
        }
        dbSubEmit(echo("BCP_ITEM", base));
        if (base.dynflags === 1) {
          dbEmit(base.dbref, base);
          if (base.type === 0) {
            hereEmit({
              dbref: base.dbref,
              contents: {},
            });
            targetEmit(null);
          } else
            targetEmit(base.dbref);

          termSub.value.write(base.description ? "You see: " + base.description + "\r\n" : "");
        }

        return;
      case 6: // BCP_VIEW_BUFFER
        viewEmit(String.fromCharCode.apply(null, arr.slice(3)));
        return;
      case 9: // BCP_AUTH_FAILURE
        authEmit(-1, true);
        return;
      case 10: // BCP_AUTH_SUCCESS
        authEmit(read_32(arr, 3));
        return;
      case 11: { // BCP_OUT
        let aux;

        const { loc, dbref } = {
          dbref: read_32(arr, aux = 3),
          loc: read_32(arr, aux += 4),
        };

        if (!dbSub.value[loc])
          return;

        let newContents = { ...dbSub.value[loc].contents };
        delete newContents[dbref];

        dbEmit(loc, { ...dbSub.value[loc], contents: newContents });
        return;
    }}
  } else if (arr[0] != 255) {
    const data = ab2str(arr);
    termSub.value.write(data);
  } else {
    do_echo = !do_echo;
    raw = !raw;
  }
}

const url = process.env.CONFIG_PROTO + "://" + window.location.hostname + ':4234';
console.log("URL", url);
const connect = wsSub.makeEmit(() => {
  let ws = null;

  function onClose() {
    return wsSub.update(init());
  }

  function init() {
    ws = new WebSocket(url, 'binary');

    ws.binaryType = 'arraybuffer';
    ws.addEventListener('open', onOpen);
    ws.addEventListener('message', onMessage);
    ws.addEventListener('close', onClose);

    return {
      ws,
      unsubscribe: () => {
        ws.removeEventListener('open', onOpen);
        ws.removeEventListener('message', onMessage);
        ws.removeEventListener('close', onClose);
      }
    };
  }

  return init();
});

const hereSub = new Sub({ dbref: null, contents: {} });
const hereEmit = hereSub.makeEmit();
const targetSub = new Sub(null);
const targetEmit = targetSub.makeEmit();
const dbSub = new Sub({});
const dbEmit = dbSub.makeEmit((dbref, obj) => ({
  ...dbSub.value,
  [dbref]: {
    contents: {},
    ...obj,
  },
}));
const dbSubEmit = dbSub.makeEmit(obj => {
  const loc = parseInt(obj.loc);
  const dbref = parseInt(obj.dbref);

  if (!dbSub.value[loc] && loc != -1) {
    console.warn("web-in: actionable of loc", loc, "is not available");
    return dbSub.value;
  }

  const current = dbSub.value[loc] ?? {};

  return {
    ...dbSub.value,
    [loc]: {
      ...current,
      contents: {
        ...current.contents,
        [dbref]: {
          ...obj,
          icon: tty_proc(obj.icon),
          pname: tty_proc(obj.pname),
        },
      }
    },
  };
});
const viewSub = new Sub("");
const viewEmit = viewSub.makeEmit(data => tty_proc(data));
const equipmentSub = new Sub({});
const equipmentEmit = equipmentSub.makeEmit(item => ({
  ...equipmentSub.value,
  [parseInt(item.eql)]: {
    ...item,
    pname: tty_proc(item.pname),
    icon: tty_proc(item.icon),
  },
}));
const authSub = new Sub({ me: null, authFail: true });
const authEmit = authSub.makeEmit((player, authFail) => ({
  me: parseInt(player),
  authFail,
}));
const statsSub = new Sub({});
const statsEmit = statsSub.makeEmit();
const barsSub = new Sub({ hp: 1, mp: 1 });
const barsEmit = barsSub.makeEmit();

function Terminal() {
	const ref = useRef(null);

	useEffect(() => {
    if (ref.current) {
      ref.current.scrollTop = ref.current.scrollHeight;
      termSub.init(ref.current);
    }
	}, []);

	// console.log(context);
  return <pre id="term" ref={ref} className="flex-grow overflow"/>;
}

function useBgImg(obj = {}) {
  const bsrc = obj.art ?? obj.avatar ?? "unknown.jpg";
	const src = baseDir + "/art/" + bsrc;
  const bname = bsrc.substring(0, bsrc.indexOf(".")).replaceAll("/", "-").replaceAll(" ", "-");

  useMagic(() => ({
    "?": {
      background: "url(" + src.replaceAll(" ", "\\ ") + ")",
      backgroundSize: "cover",
      backgroundPosition: "50% 50%",
    },
  }), "bg-img-" + bname);

  return "bg-img-" + bname;
}

function Tabs(props) {
	const { children } = props;
	const [ activeTab, setActiveTab ] = useState(0);

	return (<div className="flex-grow vertical-0">
		<div className="h0">
			{ children.map((child, idx) => {
				const { label } = child.props;
				return (<a key={idx}
					onClick={() => setActiveTab(idx)}
					className={activeTab == idx ? 'pad-small c0' : 'pad-small'}
				>
					{label}
				</a>);
			}) }
		</div>
		{ children.map((child, idx) => {
			// const { label } = child.props;
			if (idx != activeTab)
				return null;

			return child;
		}) }
	</div>);
}

function Stat(props) {
	const { label, value } = props;

	return (<div className="horizontal-small">
		<div className="tbbold">{label}</div><div>{value}</div>
	</div>);
}

const EQL_HEAD = 1;
const EQL_NECK = 2;
const EQL_CHEST = 3;
const EQL_BACK = 4;
const EQL_RHAND = 5;
const EQL_LFINGER = 6;
const EQL_RFINGER = 7;
const EQL_PANTS = 8;

const eql_label = {
	[EQL_HEAD]: "hand",
	[EQL_NECK]: "neck",
	[EQL_CHEST]: "chest",
	[EQL_BACK]: "back",
	[EQL_RHAND]: "weapon",
	[EQL_LFINGER]: "lfinger",
	[EQL_RFINGER]: "rfinger",
	[EQL_PANTS]: "grieves",
};

function Equipment(props) {
	const { eql } = props;
  const equipment = equipmentSub.use();
  const objects = dbSub.use();

	if (!equipment)
		return null;

	if (!equipment[eql])
		return <div className="s32 c0"></div>;

	return (<Avatar
		item={objects[equipment[eql]]}
    square
		size="l"
		onClick={() => sendMessage("unequip " + eql_label[eql])}
	/>);
}

function MiniMap() {
  const view = viewSub.use();
  const target = targetSub.use();

	if (target || !view)
		return null;

	return <div className="fac"><pre id="map" dangerouslySetInnerHTML={{ __html: view }}></pre></div>;
}

function TargetTitleAndArt() {
  const objects = dbSub.use();
  const target = targetSub.use();
	const obj = objects[target];
  const bgClass = useBgImg(obj);
  if (!obj)
    return null;

	return (<div className={"relative vertical-0 size-super align-items"}>
    <div className={"absolute position-top-0 position-bottom-0 position-left-0 position-right-0 " + bgClass} />
    <div className="absolute position-top-0 position-bottom-0 position-left-0 position-right-0 deep-shadow text-align tm pxs">{ obj?.name ?? "Unnamed" }</div>
	</div>);
}

function Avatar(props) {
	const { className, item, square, size = "xl", ...rest } = props;

	if (!item)
		return null;

  return <span
    className={className + " s" + size + " t" + size + " tcv size-biggest deep-shadow" + (square ? "" : " round")}
    dangerouslySetInnerHTML={{ __html: item.icon }}
    { ...rest }
  />;
}

function ContentsItem(props) {
	const { item, onClick, activeItem, isShop } = props;
	const className = "horizontal align-items " + (activeItem == item.dbref ? 'c0' : "");
  const bgClass = useBgImg(item);

	return (<a className={className + " round-pad pad display relative"} onClick={onClick}>
		<Avatar item={item} className={bgClass} />
		<span dangerouslySetInnerHTML={{
			__html: item.pname + (isShop ? " " + item.price + "P" : ""),
		}}></span>
	</a>);
}

function Contents(props) {
	const { onItemClick, activeItem } = props;
  const target = targetSub.use();
  const { dbref: here } = hereSub.use();
  const objects = dbSub.use();
	const obj = target ? objects[target] : objects[here];

	if (!here)
    return <div className="flex-grow" />;

	const contentsEl = Object.keys(obj.contents).map(k => {
		const item = obj.contents[k];

		return <ContentsItem key={item.dbref} item={item}
			isShop={obj.shop}
			activeItem={activeItem}
			onClick={e => onItemClick(e, item)} />;
	});

	return (<div className="vertical-0 flex-grow overflow icec">
		{ contentsEl }
	</div>);
}

function RB(props) {
	const { children, onClick } = props;
	return <a className="round ts26 p8 c0" onClick={onClick}>{ children }</a>;
}

function RBT() {
	return <a className="round ts26 p8"></a>;
}

function RBI(props) {
	const { className = "", onClick, src } = props;
	return (<a className={"round p8 c0 " + className} onClick={onClick}>
		<img className="svl shl" src={atiles[src] ?? src} />
	</a>);
}

function ContentsAndActions() {
  const targetState = useState(null);
  const { dbref: here } = hereSub.use();
  const rtarget = targetSub.use();
  const { me } = authSub.use();
  const objects = dbSub.use();
  const [referenceElement, setReferenceElement] = useState(null);
  const [popperElement, setPopperElement] = useState(null);
	const [ actions, setActions ] = useState([]);

  function unselect() {
      setActions([]);
      targetState[1](null);
      setReferenceElement(null);
  }

	function onItemClick(ev, item) {
    let target = ev.target;

    while (target.tagName !== "A")
      target = target.parentElement;

		let newActions = [];

    if (targetState[0] === item.dbref)
      return unselect();

		if (item.loc == here) {
			for (let p = 0; p < 9; p++) {
				if (!(parseInt(item.actions) & (1 << p)))
					continue;

				let id = ACTIONS_LABEL[p];
				newActions.push([p, function () {
					sendMessage(id + " #" + item.dbref);
          return unselect();
				}]);
			}
      } else {
        if (item.loc == me) {
          newActions.push([ACTIONS.EQUIP, function () {
            sendMessage("equip #" + item.dbref);
            return unselect();
          }]);

          newActions.push([ACTIONS.DROP, function () {
            sendMessage("drop #" + item.dbref);
            return unselect();
          }]);

          newActions.push([ACTIONS.EAT, function () {
            sendMessage("eat #" + item.dbref);
            return unselect();
          }]);

          newActions.push([ACTIONS.SHOP, function () {
            sendMessage("sell #" + item.dbref);
            return unselect();
          }]);
        } else if (objects[item.loc].shop)
          newActions.push([ACTIONS.SHOP, function () {
            sendMessage("buy #" + item.dbref);
            return unselect();
          }]);

        else
          newActions.push([ACTIONS.GET, function () {
            sendMessage("get #" + rtarget + "=#" + item.dbref);
            return unselect();
          }]);

        newActions.push([ACTIONS.LOOK, function () {
          sendMessage("look #" + item.dbref);
          return unselect();
        }]);
      }

		setActions(newActions);
		targetState[1](item.dbref);
    setReferenceElement(target);
    ev.stopPropagation();
	}

  function clickHandler(e) {
    return unselect();
  };

  useEffect(() => {
    window.addEventListener('click', clickHandler);
    return () => window.removeEventListener('click', clickHandler);
  }, []);

  const { styles, attributes } = usePopper(referenceElement, popperElement, {
    placement: "left",
  });

	const actionsEl = actions.map(([p, cb]) => (
		<RBI key={p} onClick={cb} src={p} className="c0" />
	));

	return (<>
		<Contents onItemClick={onItemClick} activeItem={targetState[0]} />
    <div ref={setPopperElement} className="margin-right-small" style={styles.popper} {...attributes.popper}>
      <div className="horizontal-small flex-wrap icec">
        { actionsEl }
      </div>
    </div>
	</>);
}

function Help() {
	return (<>
		<b>Help</b><br />
		<br />
		Input "X" to teleport to the starting position.<br />

		<br />
		<b>More details: Normal mode:</b><br />
		<p>
			<b>i</b> to access your inventory.<br />
			<b>I</b> to enter Input mode.<br />
			<b>o</b> to look "out".<br />
			<b>Left</b> or <b>h</b> to move west.<br />
			<b>Right</b> or <b>l</b> to move east.<br />
			<b>Up</b> or <b>k</b> to move north.<br />
			<b>Down</b> or <b>j</b> to move south.<br />
			<b>Shift+Up</b> or <b>K</b> to move up.<br />
			<b>Shift+Down</b> or <b>J</b> to move down.<br />
		</p>
		<b>Input mode:</b><br />
		<p>
			<b>Esc</b> to go back to normal mode.<br />
			<b>Left</b> to move west.<br />
			<b>Right</b> to move east.<br />
			<b>Up</b> to move north.<br />
			<b>Down</b> to move south.<br />
			<b>Shift+Up</b> to move up.<br />
			<b>Shift+Down</b> to move down.<br />
			other terminal bindings.<br />
		</p>
		<br />
		You can check out the code <a href="https://github.com/tty-pt/neverdark">here</a>.<br />
		<br />
		The help command is very useful. Try issuing "help help".<br />

		<p><b>also see help X, walking, w</b> </p>
	</>);
}

function Bar(props) {
	const { max, value, color } = props;
	const style = {
		width: (100 * value / max) + "%",
		};

	return (<div className="flex-grow svs b0">
		<div className={"svf c" + color} style={style}></div>
	</div>);
}

function PlayerBars() {
	const bars = barsSub.use();

	if (!bars)
		return null;

	const { hp, hpMax, mp, mpMax } = bars;

	return (<div className="horizontal-small pad-small">
		<Bar value={hp} max={hpMax} color="1" />
		<Bar value={mp} max={mpMax} color="12" />
	</div>);
}

function StatsModal() {
	const stats = statsSub.use();

  return (<div label="stats" className="p8 h8 f">
    <div className="v8 fg">
      <Stat label="str" value={stats.str} />
      <Stat label="con" value={stats.con} />
      <Stat label="dex" value={stats.dex} />
      <Stat label="int" value={stats.int} />
      <Stat label="wiz" value={stats.wiz} />
      <Stat label="cha" value={stats.cha} />
    </div>
    <div className="v8 fg">
      <Stat label="dodge" value={stats.dodge} />
      <Stat label="dmg" value={stats.dmg} />
      <Stat label="mdmg" value={stats.mdmg} />
      <Stat label="def" value={stats.def} />
      <Stat label="mdef" value={stats.mdef} />
    </div>
  </div>);
}

function StatsButton() {
	const [ modal, isOpen, setOpen ] = useModal(StatsModal, {});

  return (<>
    <RB onClick={() => setOpen(true)}>s</RB>
    { isOpen ? modal : null }
  </>);
}

function EquipmentModal() {
  return (<div label="equipment" className="p8 v8 c">
    <div className="h8 f">
      <div className="s32"></div>
      <div className="s32"></div>
      <Equipment eql={EQL_HEAD} />
      <div className="s32"></div>
      <div className="s32"></div>
    </div>
    <div className="h8 f">
      <Equipment eql={EQL_RHAND} />
      <div className="s32"></div>
      <Equipment eql={EQL_NECK} />
      <Equipment eql={EQL_BACK} />
      <div className="s32"></div>
    </div>
    <div className="h8 f">
      <Equipment eql={EQL_RFINGER} />
      <div className="s32"></div>
      <Equipment eql={EQL_CHEST} />
      <div className="s32"></div>
      <Equipment eql={EQL_LFINGER} />
    </div>
    <div className="h8 f">
      <div className="s32"></div>
      <div className="s32"></div>
      <Equipment eql={EQL_PANTS} />
      <div className="s32"></div>
      <div className="s32"></div>
    </div>
  </div>);
}

function EquipmentButton() {
	const [ modal, isOpen, setOpen ] = useModal(EquipmentModal, {});

  return (<>
    <RBI onClick={() => setOpen(true)} src={ACTIONS.EQUIP} />
    { isOpen ? modal : null }
  </>);
}

function Game() {
	const session = wsSub.use().ws;
  const { dbref: here } = hereSub.use();
  const objects = dbSub.use();
	const [ modal, isOpen, setOpen ] = useModal(Help, {});
  const [ referenceElement, setReferenceElement ] = useState(null);
  const obj = objects[here];
  const bgClass = useBgImg(obj);

	function keyUpHandler(e) {
    if (termSub.value.focused || raw)
      return;
    switch (e.keyCode) {
    case 75: // k
    case 38: // ArrowUp
      if (e.shiftKey)
        sendMessage("K");
      else
        sendMessage("k");
      break;
    case 74: // j
    case 40: // ArrowDown
      if (e.shiftKey)
        sendMessage("J");
      else
        sendMessage("j");
      break;
    case 72: // h
    case 37: // ArrowLeft
      sendMessage("h");
      break;
    case 76: // l
    case 39: // ArrowRight
      sendMessage("l");
      break;
    case 73: // i
      if (e.shiftKey)
        termSub.value.focus();
      else
        sendMessage("inventory");
      break;
    case 79: // o
      sendMessage("look");
      break;
    default:
      console.log(e);
    }
  }
	useEffect(() => {
		window.addEventListener('keyup', keyUpHandler);
		return () => window.removeEventListener('keyup', keyUpHandler);
	}, [session]);

	function toggle_help() {
		setOpen(!isOpen);
	}

  function clickHandler(e) {
    setReferenceElement(null);
  };

  useEffect(() => {
    window.addEventListener('click', clickHandler);
    return () => window.removeEventListener('click', clickHandler);
  }, []);

	return (<>
    <div className={"absolute opacity-2 position-left-0 position-right-0 position-top-0 position-bottom-0 " + bgClass} />
    <div className="pad-small horizontal-small absolute position-left-0 position-right-0 position-top-0 position-bottom-0">
      <div className="vertical-small flex-grow shf overflow">
        <div className="tm text-align">{ obj?.name ?? "Unnamed" }</div>

        <PlayerBars />

        <Terminal />
      </div>

      <div className="vertical-small flex-grow align-items sh33">
        <MiniMap />
        <TargetTitleAndArt />
        <ContentsAndActions />
        { referenceElement ? (
        <div className="vertical-0 tar tnow">
          <div className="horizontal-0">
            <RBT />
            <RB onClick={() => sendMessage("k")}>&uarr;</RB>
            <RBI onClick={() => sendMessage("K")} src={ACTIONS.K} />
          </div>
          <div className="horizontal-0">
            <RB onClick={() => sendMessage("h")}>&larr;</RB>
            <RBT />
            <RB onClick={() => sendMessage("l")}>&rarr;</RB>
          </div>
          <div className="horizontal-0">
            <RBT />
            <RB onClick={() => sendMessage("j")}>&darr;</RB>
            <RBI onClick={() => sendMessage("J")} src={ACTIONS.J} />
          </div>
        </div>
        ) : (
        <div className="horizontal-small flex-wrap justify-content">
          {/* <RB onClick={disconnect}>X</RB> */}
          <RB onClick={toggle_help}>?</RB>
          <StatsButton />
          <EquipmentButton />
          <RBI onClick={() => sendMessage('inventory')} src={ACTIONS.OPEN} />
          <RBI onClick={() => sendMessage('look')} src={ACTIONS.LOOK} />
          <RBI onClick={(ev) => { setReferenceElement(ev.target); ev.stopPropagation(); }} src={baseDir + "/art/walking.png"} />
        </div>
        ) }
      </div>
    </div>

		{ modal }
	</>);
}

function getCookie(cname) {
	let name = cname + "=";
	let decodedCookie = decodeURIComponent(document.cookie);
	let ca = decodedCookie.split(';');
	for(let i = 0; i < ca.length; i++) {
		let c = ca[i];
		while (c.charAt(0) == ' ') {
			c = c.substring(1);
		}
		if (c.indexOf(name) == 0) {
			return c.substring(name.length, c.length);
		}
	}
	return "";
}

export default
withMagic(function App() {
	const session = wsSub.use();
  useEffect(() => {
    connect();
    return session.unsubscribe();
  }, []);
	return <Game />;
});
