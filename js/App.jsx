import React, {
	useState, useEffect, useRef,
	useCallback, useReducer, useContext,
} from "react";
import { Terminal as XTerm } from 'xterm';
// import { WebglAddon } from 'xterm-addon-webgl';
import { FitAddon } from 'xterm-addon-fit';
import { WebLinksAddon } from 'xterm-addon-web-links';
// import { OverlayAddon } from './addons/overlay';
import { usePopper } from 'react-popper';
import { useMagic, withMagic, makeMagic } from "@tty-pt/styles";
import ReactDOM from "react-dom";
import ACTIONS, { ACTIONS_LABEL } from "./actions";
import mcp from "./mcp";
import tty_proc from "./tty";
// import canvas from "./canvas";
import 'xterm/css/xterm.css';
import "./vim.css";
const baseDir = process.env.CONFIG_BASEDIR || "";

function easySub(defaultData) {
  const subs = new Map();
  const valueMap = { value: defaultData };

  function update(obj) {
    valueMap.value = obj;
    let allPromises = [];
    for (const [sub] of subs)
      allPromises.push(sub(obj));
    return Promise.all(allPromises);
  }

  function subscribe(sub) {
    subs.set(sub, true);
    return () => {
      subs.delete(sub);
    };
  }

  function easyEmit(cb = a => a) {
    return async (...args) => update(await cb(...args));
  }

  return { update, subscribe, data: valueMap, easyEmit };
}

function useSub(sub, defaultData = null) {
  const [data, setData] = useState(sub.data.value ?? defaultData);
  useEffect(() => sub.subscribe(setData), []);
  return data;
}

const fitAddon = new FitAddon();
// const overlayAddon = new OverlayAddon();

const termSub = easySub(null);

const termEmit = termSub.easyEmit((parent) => {
  const term = new XTerm({
    fontSize: 13,
    fontFamily: 'Consolas,Liberation Mono,Menlo,Courier,monospace',
    theme: {
      foreground: '#637d75',
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
  term.onKey(event => {
    if (event.key === "\r") {
      term.write("\r\n");
      sendMessage(term.inputBuf);
      term.inputBuf = "";
    } else {
      term.write(event.key);
      term.inputBuf += event.key;
    }
  });
  fitAddon.fit();
  return term;
});

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
    marginLeft: "-1rem",
    marginBottom: "-1rem",
    maxWidth: "none",
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

mcp.init();

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

const GameContext = React.createContext({});

const wsSub = easySub({
  ws: null,
  unsubscribe: () => {},
});

function sendMessage(text) {
  wsSub.data.value.ws.send(text + "\n");
}

function onOpen() {
  if (process.env.development) {
    sendMessage("auth test");
  } else {
    sendMessage("auth " + getCookie("QSESSION"));
  }
}

function onMessage(ev) {
  const mcp_arr = mcp.proc(ev.data);
  for (let i = 0; i < mcp_arr.length; i++) {
    const item = mcp_arr[i];
    if (mcp_map[item.key])
      mcp_map[item.key](item);
  }
}

const url = process.env.CONFIG_PROTO + "://" + window.location.hostname + ':4201';
const connect = wsSub.easyEmit(() => {
  let ws = null;

  function onClose() {
    return wsSub.update(init());
  }

  function init() {
    ws = new WebSocket(url, 'text');

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

function useSession() {
  useEffect(() => connect().unsubscribe, []);
	return useSub(wsSub).ws;
}

const terminalSub = easySub("");
const terminalEmit = terminalSub.easyEmit((text) => {
  return terminalSub.data.value + text;
});
const hereSub = easySub({ dbref: null, contents: {} });
const hereEmit = hereSub.easyEmit();
const targetSub = easySub(null);
const targetEmit = targetSub.easyEmit();
const dbSub = easySub({});
const dbEmit = dbSub.easyEmit((dbref, obj) => ({
  ...dbSub.data.value,
  [dbref]: {
    contents: {},
    ...obj,
  },
}));
const dbSubEmit = dbSub.easyEmit(obj => {
  const loc = parseInt(obj.loc);
  const dbref = parseInt(obj.dbref);

  if (!dbSub.data.value[loc]) {
      console.warn("web-in: actionable of loc", loc, "is not available");
      return dbSub.data.value;
  }

  return {
    ...dbSub.data.value,
    [loc]: {
      ...dbSub.data.value[loc],
      contents: {
        ...dbSub.data.value[loc].contents,
        [dbref]: {
          ...obj,
          icon: tty_proc(obj.icon),
          pname: tty_proc(obj.pname),
        },
      }
    },
  };
});
const viewSub = easySub("");
const viewEmit = viewSub.easyEmit(data => tty_proc(data));
const equipmentSub = easySub({});
const equipmentEmit = equipmentSub.easyEmit(item => ({
  ...equipmentSub.data.value,
  [parseInt(item.eql)]: {
    ...item,
    pname: tty_proc(item.pname),
    icon: tty_proc(item.icon),
  },
}));
const authSub = easySub({ me: null, authFail: true });
const authEmit = authSub.easyEmit((player, authFail) => ({
  me: parseInt(player),
  authFail,
}));
const statsSub = easySub({});
const statsEmit = statsSub.easyEmit();
const barsSub = easySub({ hp: 1, mp: 1 });
const barsEmit = barsSub.easyEmit();

const mcp_map = {
  'inband': action => {
    if (action.data != "\n\r") {
      console.log("output", action.data.substring(1), action.data.charAt(0));
      termSub.data.value.write(action.data.substring(1) + "\r\n");
      // terminalEmit(tty_proc(action.data));
    }
  },
  "web-view": action => viewEmit(action.data),
  "web-look": action => {
    const dbref = parseInt(action.dbref);
    dbEmit(dbref, action);

    if (action.room) {
      hereEmit({
        dbref,
        contents: {},
      });
      targetEmit(null);
    } else
      targetEmit(dbref);

    // terminalEmit(action.description ? "\nYou see: " + action.description : "");
    termSub.data.value.write(action.description ? "You see: " + action.description + "\r\n" : "");
  },
  "web-look-content": dbSubEmit,
  "web-in": dbSubEmit,
  "web-out": action => {
		const dbref = parseInt(action.dbref);
		const loc = parseInt(action.loc);

		if (!dbSub.data.value[loc])
      return;

		let newContents = { ...dbSub.data.value[loc].contents };
		delete newContents[dbref];

    dbEmit(loc, { ...dbSub.data.value[loc], contents: newContents });
  },
  "web-auth-success": action => authEmit(action.player),
  "web-auth-fail": action => authEmit(action.player, true),
  "web-stats": statsEmit,
  "web-bars": barsEmit,
  "web-equipment-item": equipmentEmit,
};

function GameContextProvider(props) {
	const { children } = props;
	const session = useSession();

	return (<GameContext.Provider
		value={{ session }}
	>
		{ children }
	</GameContext.Provider>);
}

function Terminal() {
  const terminal = useSub(terminalSub);
	const ref = useRef(null);

	useEffect(() => {
    if (ref.current) {
      ref.current.scrollTop = ref.current.scrollHeight;
      termEmit(ref.current);
    }
	}, [terminal]);

	// console.log(context);
  return (
    
    <pre id="term" ref={ref} className="flex-grow overflow"
      dangerouslySetInnerHTML={{ __html: terminal }}>
    </pre>
  );
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
  const equipment = useSub(equipmentSub);

	if (!equipment)
		return null;

	if (!equipment[eql])
		return <div className="s32 c0"></div>;

	return (<Avatar
		item={equipment[eql]}
    square
		size="l"
		onClick={() => sendMessage("unequip " + eql_label[eql])}
	/>);
}

function MiniMap(props) {
  const view = useSub(viewSub);
  const target = useSub(targetSub);

	if (target || !view)
		return null;

	return <div className="fac"><pre id="map" dangerouslySetInnerHTML={{ __html: view }}></pre></div>;
}

function TargetTitleAndArt() {
  const objects = useSub(dbSub);
  const target = useSub(targetSub);
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
  const target = useSub(targetSub);
  const { dbref: here } = useSub(hereSub);
  const objects = useSub(dbSub);
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

function RBT(props) {
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
  const { dbref: here } = useSub(hereSub);
  const rtarget = useSub(targetSub);
  const { me } = useSub(authSub);
  const objects = useSub(dbSub);
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
		} else if (item.loc == me) {
			// action_add(ACTIONS.PUT, function () {
			// output("\nPUT is not implemented yet!");
			// });

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

		} else if (objects[item.loc].shop) {
			newActions.push([ACTIONS.SHOP, function () {
				sendMessage("buy #" + item.dbref);
        return unselect();
			}]);
		} else {
			newActions.push([ACTIONS.GET, function () {
				sendMessage("get #" + rtarget + "=#" + item.dbref);
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
			<b>s</b> To chat.<br />
			<b>a</b> to send commands.<br />
			<b>i</b> to access your inventory.<br />
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
			<b>Up</b> to travel up in history.<br />
			<b>Down</b> to travel down in history.<br />
			<b>Ctrl+u</b> to delete input text.<br />
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
	const { bars } = useContext(GameContext);

	if (!bars)
		return null;

	const { hp, hpMax, mp, mpMax } = bars;

	return (<div className="horizontal-small pad-small">
		<Bar value={hp} max={hpMax} color="1" />
		<Bar value={mp} max={mpMax} color="12" />
	</div>);
}

function StatsModal() {
	const stats = useSub(statsSub);

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
	const { session } = useContext(GameContext);
  const { dbref: here } = useSub(hereSub);
  const objects = useSub(dbSub);
	const [ modal, isOpen, setOpen ] = useModal(Help, {});
  const [ referenceElement, setReferenceElement ] = useState(null);
  const obj = objects[here];
  const bgClass = useBgImg(obj);

	function keyDownHandler(e) {
		switch (e.keyCode) {
			case 38: // ArrowUp
				if (e.shiftKey)
					sendMessage("K");
				else
					sendMessage("k");
				break;
			case 40: // ArrowDown
				if (e.shiftKey)
					sendMessage("J");
				else
					sendMessage("j");
				break;
			case 37: // ArrowLeft
				sendMessage("h");
				break;
			case 39: // ArrowRight
				sendMessage("l");
				break;
			case 73: // i
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
		window.addEventListener('keydown', keyDownHandler);
		return () => window.removeEventListener('keydown', keyDownHandler);
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
	return (<GameContextProvider>
		<Game />
	</GameContextProvider>);
});
