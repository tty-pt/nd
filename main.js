function _extends() { _extends = Object.assign || function (target) { for (var i = 1; i < arguments.length; i++) { var source = arguments[i]; for (var key in source) { if (Object.prototype.hasOwnProperty.call(source, key)) { target[key] = source[key]; } } } return target; }; return _extends.apply(this, arguments); }

let out_buf = "";

String.prototype.replaceAt = function (index, replacement) {
  return this.substr(0, index) + replacement + this.substr(index + replacement.length);
};

const MCP_ON = 1;
const MCP_KEY = 2;
const MCP_MULTI = 4;
const MCP_ARG = 8;
const MCP_NOECHO = 16;
const MCP_SKIP = 32;
const MCP_INBAND = 64;
const MCP_NAME = 128;
const MCP_HASH = 256;
const MCP_CONFIRMED = 4;
let mcp = {
  state: 1,
  args: [],
  name: "",
  cache: "",
  args_l: 0,
  cache_i: 0,
  flags: 0
},
    mcp_arr;

function mcp_clear() {
  mcp.args = [];
  mcp.name = mcp.cache = "";
  mcp.args_l = 0;
  mcp_cache_i = 0;
  mcp.flags = 0;
  mcp.state = 1;
}

function mcp_emit() {
  mcp_arr.push(mcp.args.reduce((a, i) => Object.assign(a, {
    [i.key]: i.value
  }), {
    key: mcp.name
  }));
  mcp_clear();
}

function inband_emit() {
  mcp_arr.push({
    key: "inband",
    data: mcp.cache
  });
  mcp_clear();
}

function mcp_proc_ch(p) {
  if (mcp.flags & MCP_SKIP) {
    if (p != '\n') return;
    mcp.flags ^= MCP_SKIP;
    mcp.state = 1;
    return;
  }

  switch (p) {
    case '#':
      switch (mcp.state) {
        case 3:
        case 1:
          mcp.state++;
          return;
      }

      break;

    case '$':
      if (mcp.state == 2) {
        mcp.state++;
        return;
      }

      break;

    case '*':
      if (mcp.state == MCP_CONFIRMED) {
        mcp.flags = MCP_MULTI | MCP_NOECHO;
        mcp.state = 0;
        return;
      }

      if (!(mcp.flags & MCP_ON) || !(mcp.flags & MCP_KEY)) break;
      {
        mcp.flags |= MCP_MULTI | MCP_SKIP;
      }
      ;
      const arg = {
        key: mcp.cache,
        value: null
      };
      mcp.args.push(arg);
      mcp.args_l++;
      mcp.cache = "";
      mcp.cache_i = 0;
      return;

    case ':':
      if (mcp.flags & MCP_MULTI) {
        if (mcp.state == MCP_CONFIRMED) {
          mcp.state = 0;
          mcp.args[mcp.args_l - 1].value = mcp.cache;
          mcp.cache = "";
          mcp.cache_i = 0;
          mcp.args_l++;
          mcp_emit();
          mcp.flags = MCP_SKIP;
        } else mcp.flags &= ~MCP_NOECHO;

        return;
      } else if (mcp.flags & MCP_KEY) {
        const arg = {
          key: mcp.cache,
          value: null
        };
        mcp.args.push(arg);
        mcp.args_l++;
        mcp.cache = "";
        mcp.cache_i = 0;
        return;
      }

      break;

    case '"':
      if (!(mcp.flags & MCP_ON)) break;
      mcp.flags ^= MCP_KEY;

      if (mcp.flags & MCP_KEY) {
        mcp.args[mcp.args_l - 1].value = mcp.cache;
        mcp.cache = "";
        mcp.cache_i = 0;
      }

      return;

    case ' ':
      if (!(mcp.flags & MCP_ON)) break;else if (mcp.flags & MCP_NAME) {
        mcp.flags ^= MCP_NAME | MCP_KEY;
        mcp.name = mcp.cache;
        mcp.cache = "";
        mcp.cache_i = 0;
        if (mcp.name) mcp.args_l = 0;else mcp.flags = MCP_SKIP;
        return;
      } else if (mcp.flags & MCP_HASH) {
        mcp.flags ^= MCP_HASH | MCP_KEY | MCP_NOECHO;
        return;
      } else if (mcp.flags & MCP_KEY) return;
      break;

    case '\n':
      mcp.state = 1;

      if (mcp.flags & MCP_MULTI) {
        mcp.cache += '\n';
        mcp.cache_i++;
      }

      return;
  }

  if (mcp.state == MCP_CONFIRMED) {
    if (mcp.name) mcp_emit();else if (mcp.flags & MCP_INBAND) inband_emit();
    mcp.flags = MCP_ON | MCP_NAME;
    mcp.state = 0;
  } else if (mcp.state) {
    if (mcp.flags & MCP_ON) mcp_emit();
    if (!(mcp.flags & MCP_MULTI)) mcp.flags = MCP_INBAND;
    mcp.cache += "\n#$#".substr(0, mcp.state);
    mcp.cache_i += mcp.state;
    mcp.state = 0;
  }

  if (!(mcp.flags & MCP_NOECHO)) {
    mcp.cache += p;
    mcp.cache_i++;
  }
}

function mcp_proc(data) {
  let in_i;
  mcp_arr = [];

  for (in_i = 0; in_i < data.length; in_i++) {
    mcp_proc_ch(data.charAt(in_i));
  }

  if (mcp.flags & MCP_MULTI) ;else if (mcp.flags & MCP_ON) mcp_emit();else if (mcp.flags & MCP_INBAND) inband_emit();
  return mcp_arr;
}

function mcp_reset() {
  last_i = out_i = 0;
  out_i++;
}

function mcp_init() {
  mcp.state = 1;
  mcp_reset();
  mcp_clear();
}

function params_push(tty, x) {
  let fg = tty.c_attr.fg,
      bg = tty.c_attr.bg;

  switch (x) {
    case 0:
      fg = 7;
      bg = 0;
      break;

    case 1:
      fg += 8;
      break;

    default:
      if (x >= 40) bg = x - 40;else if (x >= 30) fg = (fg >= 8 ? 8 : 0) + x - 30;
  }

  tty.csi.fg = fg;
  tty.csi.bg = bg;
  tty.csi.x = x;
}

function csi_change(tty) {
  const a = tty.csi.fg != 7,
        b = tty.csi.bg != 0;
  tty.output += tty.end_tag;

  if (a || b) {
    tty.output += "<span class=\"";
    if (a) tty.output += "cf" + tty.csi.fg;
    if (b) tty.output += " c" + tty.csi.bg;
    tty.output += "\">";
    ;
    tty.end_tag = "</span>";
  } else tty.end_tag = "";
}

function tty_init() {
  return {
    csi: {
      fg: 7,
      bg: 0,
      x: 0
    },
    c_attr: {
      fg: 7,
      bg: 0,
      x: 0
    },
    end_tag: "",
    esc_state: 0,
    output: ""
  };
}

function esc_state_0(tty, ch) {
  if (tty.csi_changed) {
    csi_change(tty);
    tty.csi_changed = 0;
  }

  switch (ch) {
    case '<':
      tty.output += "&lt;";
      return 4;

    case '>':
      tty.output += "&gt;";
      return 4;

    case '"':
      tty.output += "\"";
      return 2;
      tty.output += "\\";
  }

  tty.output += ch;
  return 1;
}

function tty_proc_ch(tty, ch) {
  switch (ch) {
    case '\x18':
    case '\x1a':
      tty.esc_state = 0;
      return 0;

    case '\x1b':
      tty.esc_state = 1;
      return 0;

    case '\x9b':
      tty.esc_state = 2;
      return 0;

    case '\x07':
    case '\x00':
    case '\x7f':
    case '\v':
    case '\r':
    case '\f':
      return 0;
  }

  switch (tty.esc_state) {
    case 0:
      return esc_state_0(tty, ch);

    case 1:
      switch (ch) {
        case '[':
          tty.esc_state = 2;
          break;

        case '=':
        case '>':
        case 'H':
          tty.esc_state = 0;
      }

      break;

    case 2:
      switch (ch) {
        case 'K':
        case 'H':
        case 'J':
          tty.esc_state = 0;
          return 0;

        case '?':
          tty.esc_state = 5;
          return 0;
      }

      params_push(tty, 0);
      tty.esc_state = 3;

    case 3:
      switch (ch) {
        case 'm':
          if (tty.c_attr.bg != tty.csi.bg || tty.c_attr.fg != tty.csi.fg) {
            tty.c_attr.fg = tty.csi.fg;
            tty.c_attr.bg = tty.csi.bg;
            tty.c_attr.x = 0;
            tty.csi.x = 0;
            tty.csi_changed = 1;
          }

          tty.esc_state = 0;
          break;

        case '[':
          tty.esc_state = 4;
          break;

        case ';':
          params_push(tty, 0);
          break;

        default:
          if (ch >= '0' && ch <= '9') params_push(tty, tty.csi.x * 10 + (ch - '0'));else tty.esc_state = 0;
      }

      break;

    case 5:
      params_push(tty, ch);
      tty.esc_state = 6;
      break;

    case 4:
    case 6:
      tty.esc_state = 0;
      break;
  }

  return 0;
}

function tty_proc(input) {
  let tty = tty_init();
  let in_i;

  for (in_i = 0; in_i < input.length; in_i++) tty_proc_ch(tty, input.charAt(in_i));

  return tty.output;
}

let canvas = document.querySelector("canvas");
let ctx = canvas.getContext('2d');
let tilemap = document.createElement("img");
tilemap.src = "./art/tilemap.png";
tilemap.classList.add("dn");
const ACT_LOOK = 0;
const ACT_KILL = 1;
const ACT_SHOP = 2;
const ACT_DRINK = 3;
const ACT_OPEN = 4;
const ACT_CHOP = 5;
const ACT_FILL = 6;
const ACT_GET = 7;
const ACT_DIE = 8;
const ACT_PUT = 9;
const ACT_EQUIP = 10;
const ACT_DROP = 11;
const ACT_EAT = 12;
const ACT_13 = 13;
const ACT_INVENTORY = 14;
let actions_lbl = ["look", "kill", "shop", "drink", "open", "chop", "fill", "get", "die", "put", "equip", "drop", "eat", "reserved", "inventory"];
let actions_tiles = [];

function tilemap_cache() {
  let p = 0;

  for (let i = 0; i < 5; i++) {
    for (let j = 0; j < 3; j++, p++) {
      ctx.clearRect(0, 0, 16, 16);
      ctx.drawImage(tilemap, 16 * j, 16 * i, 16, 16, 0, 0, 16, 16);
      actions_tiles.push(canvas.toDataURL());
    }
  }

  console.log(actions_tiles);
}

tilemap.onload = function () {
  setTimeout(tilemap_cache, 0);
};

document.body.appendChild(tilemap);
const {
  useState,
  useEffect,
  useRef,
  useCallback,
  useReducer,
  useContext
} = React;

class Modal extends React.Component {
  constructor(props) {
    super(props);
    this.el = props.el;
  }

  componentDidMount() {
    this.el.onclick = e => {
      if (e.target == this.el) this.props.setOpen(false);
    };

    this.el.className = 'modal abs sfv vn c fcc oh f';
  }

  componentWillUnmount() {
    this.el.classList.add('dn');
  }

  onKeyDown(e) {
    if (e.keyCode == 27) this.props.setOpen(false);
  }

  render() {
    return ReactDOM.createPortal( /*#__PURE__*/React.createElement("span", {
      onKeyDown: e => this.onKeyDown(e),
      onClick: e => e.stopPropagation(),
      className: "r c0 p oa"
    }, this.props.children), this.el);
  }

}

const modal = document.querySelector('.modal');

function _useModal(el, Component, props = {}) {
  const [isOpen, setOpen] = useState(false);
  const modal = isOpen ? /*#__PURE__*/React.createElement(Modal, {
    setOpen: setOpen,
    el: el
  }, /*#__PURE__*/React.createElement(Component, _extends({}, props, {
    close: () => setOpen(false)
  }))) : null;
  return [modal, isOpen, setOpen];
}

const useModal = _useModal.bind(null, modal);

mcp_init();
const atiles = ["data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQklEQVQ4jWNgGL7g4OFb/9ExWZpwYZyaiXEBhiH4NBNSA+fg8wpWW5H1ETIAl98xvEHIEJyacfmdEB8vICsdjFAAAGW58imbroFwAAAAAElFTkSuQmCC", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jWNgoAY4ePjW/4OHb/0nWw+MQ6whWNUTawhedYQMIcoSXIpI8ia6YlLDCMMQcmKJdGdTxQW4/IyNT7KziXYJMemAoBewOROfHG5BAi4lRT1OAAA/Xu7MVtQQRgAAAABJRU5ErkJggg==", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAOUlEQVQ4jWNgGJTg4OFb/3FhkjXjEiNaMzFyWBXB+Li8RHsDKPYCIZfQxwCSvYBLIcWJiSTNQw8AAO8uLsItXTaGAAAAAElFTkSuQmCC", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgIBIcPHzrPzImWREyn2wDyHIBuhhJBhDUgM8QdDZJmim2nWwDcBlCkmZ0Q8iynSRN2AwYGP9T3QAYnyQDyAEAd7gQfVonw9EAAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAVElEQVQ4jaXRywkAMAgDUPffStzCZey1FE3UCt7Cw48IKTUPNQ+Wo8AauYEV8gIQycJVQ6ALwwlWq7DAGEBIegO07+iV3wBDWgBC2kCFjIAMGQOoDj9EP5M9YZdfAAAAAElFTkSuQmCC", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGLTg4OFb/5ExyRoGjwHIcmQZgi5HsiHY5EgyBJccQUOIsWBwGEKRAcMEAABAgauIKxMDjQAAAABJRU5ErkJggg==", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAUElEQVQ4jWNgwAEOHr71H5cc0QBmCEWGUcUlZNuMjskyBJmmnwHYnEySNw4evvX/////TLhcRFAzJfLUMYAQxmsAIUMIaqaKAdgMIkkjKQAABfenzXQV7xsAAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcwWQYQkiPJFQQNgClGp8lOraMANwAA27bDE5yTd30AAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgoCU4ePjWfximSDNZhlDFAGw02a4gWTNJtuOzBZ84hhwxziXKS9gUURyVJGlEt5EkQwgFKE6DSLGForRBdQAAjPakzh7dkTsAAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jc3QwQ0AMAQFUPtvJbawjF4qaUVQcair/18EwB4kFiQW6IyW24gF/kKewQwpQWNABJcBWxq7IgU03C5HUH2ZZJBYwrPOx1no6mWI3bv5yiVebgFV9xc3cxaEggAAAABJRU5ErkJggg==", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAALElEQVQ4jWNgGF7g4OFb/5ExsXI4FeHDRLuCJM3EGEJWWIxEA3AZQrTmEQYAvLnktS1HPJUAAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcw0Qaga6KaKwgaAFNMtmZ8BtHPAOSAI9sgipI2ukuwAQABHugSAyRIzQAAAABJRU5ErkJggg==", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNgoDY4ePjWf7LkYRLoNLoasg2AaR6iBiBrpq0BuBQOEQPQFeIzHKcBMMWEXEeyAfhcR7Iisg3AJQcA7LE1fLFqxRoAAAAASUVORK5CYII=", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAEklEQVQ4jWNgGAWjYBSMAggAAAQQAAF/TXiOAAAAAElFTkSuQmCC", "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGFTg4OFb/4nBBA0hRw7FBWTJE+U8fOqI0YxX7agBVDKAomhElqQoKdMdAADVVJq360zbaQAAAABJRU5ErkJggg=="];
const GameContext = React.createContext({});

function useSession(onOpen, onMessage) {
  const [session, setSession] = useState(null);
  const connect = useCallback(() => {
    const ws = new WebSocket("ws" + "://" + window.location.hostname + ':4201', 'text');
    ws.binaryType = 'arraybuffer';
    setSession(ws);
  }, []);

  const updateOpenHandler = () => {
    if (!session) return;
    session.addEventListener('open', onOpen);
    return () => {
      session.removeEventListener('open', onOpen);
    };
  };

  const updateMessageHandler = () => {
    if (!session) return;
    session.addEventListener('message', onMessage);
    return () => {
      session.removeEventListener('message', onMessage);
    };
  };

  useEffect(connect, []);
  useEffect(updateOpenHandler, [session, onOpen]);
  useEffect(updateMessageHandler, [session, onMessage]);

  function sendMessage(text) {
    session.send(text + "\n");
  }

  return [connect, sendMessage, session];
}

function webLookReducer(state, action) {
  const dbref = parseInt(action.dbref);
  const {
    room
  } = action;
  const ret = { ...state,
    ...(room ? {
      here: dbref,
      target: null
    } : {
      target: dbref
    }),
    objects: { ...state.objects,
      [dbref]: { ...action,
        contents: {}
      }
    }
  };
  return ret;
}

function webLookContentReducer(state, action) {
  const dbref = parseInt(action.dbref);
  const loc = parseInt(action.loc);
  return { ...state,
    objects: { ...state.objects,
      [loc]: { ...state.objects[loc],
        contents: { ...state.objects[loc].contents,
          [dbref]: { ...action,
            icon: tty_proc(action.icon),
            pname: tty_proc(action.pname)
          }
        }
      }
    }
  };
}

function webInReducer(state, action) {
  const dbref = parseInt(action.dbref);
  const loc = parseInt(action.loc);

  if (!state.objects[loc]) {
    console.warn("web-in: actionable of loc", loc, "is not available");
    return state;
  }

  return { ...state,
    objects: { ...state.objects,
      [loc]: { ...state.objects[loc],
        contents: { ...state.objects[loc].contents,
          [dbref]: { ...action,
            icon: tty_proc(action.icon)
          }
        }
      }
    }
  };
}

function gameReducer(state, action) {
  console.log(action);

  switch (action.key) {
    case 'inband':
      if (action.data != "\n\r") return { ...state,
        terminal: state.terminal + tty_proc(action.data)
      };
      break;

    case 'web-view':
      return { ...state,
        view: tty_proc(action.data)
      };

    case 'web-look':
      return webLookReducer(state, action);

    case 'web-look-content':
      return webLookContentReducer(state, action);

    case 'web-in':
      return webInReducer(state, action);

    case 'web-out':
      const dbref = parseInt(action.dbref);
      const loc = parseInt(action.loc);
      if (!state.objects[loc]) return state;
      let newContents = { ...state.objects[loc].contents
      };
      delete newContents[dbref];
      return { ...state,
        objects: { ...state.objects,
          [loc]: { ...state.objects[loc],
            contents: newContents
          }
        }
      };

    case 'web-auth-success':
      return { ...state,
        me: parseInt(action.player)
      };

    case 'web-stats':
      return { ...state,
        stats: action
      };
  }

  return state;
}

function GameContextProvider(props) {
  const {
    children
  } = props;
  const [state, dispatch] = useReducer(gameReducer, {
    terminal: "",
    objects: {},
    target: null,
    here: null
  });

  function onMessage(ev) {
    const mcp_arr = mcp_proc(ev.data);

    for (let i = 0; i < mcp_arr.length; i++) {
      dispatch(mcp_arr[i]);
    }
  }

  function onOpen() {
    console.log("socket connection open");
  }

  const [connect, sendMessage, session] = useSession(onOpen, onMessage);
  return /*#__PURE__*/React.createElement(GameContext.Provider, {
    value: { ...state,
      session,
      sendMessage
    }
  }, children);
}

function Terminal() {
  const {
    terminal
  } = useContext(GameContext);
  const ref = useRef(null);
  useEffect(() => {
    ref.current.scrollTop = ref.current.scrollHeight;
  }, [terminal]);
  return /*#__PURE__*/React.createElement("pre", {
    id: "term",
    ref: ref,
    className: "fg oa",
    dangerouslySetInnerHTML: {
      __html: terminal
    }
  });
}

function RoomTitleAndArt() {
  const {
    here,
    objects
  } = useContext(GameContext);
  const obj = objects[here];
  if (!obj) return null;
  const src = "art/" + (obj.art || "unknown.jpg");
  return /*#__PURE__*/React.createElement("div", {
    className: "vn fg f fic"
  }, /*#__PURE__*/React.createElement("div", {
    className: "tm pxs tac"
  }, obj.name), /*#__PURE__*/React.createElement("img", {
    className: "sr2",
    src: src
  }));
}

function Tabs(props) {
  const {
    children
  } = props;
  const [activeTab, setActiveTab] = useState(0);
  return /*#__PURE__*/React.createElement("div", {
    className: "fg vn"
  }, /*#__PURE__*/React.createElement("div", {
    className: "_n"
  }, children.map((child, idx) => {
    const {
      label
    } = child.props;
    return /*#__PURE__*/React.createElement("a", {
      key: idx,
      onClick: () => setActiveTab(idx),
      className: activeTab == idx ? 'ps c0' : 'ps'
    }, label);
  })), children.map((child, idx) => {
    if (idx != activeTab) return null;
    return child;
  }));
}

function Stat(props) {
  const {
    label,
    value
  } = props;
  return /*#__PURE__*/React.createElement("div", {
    className: "_s"
  }, /*#__PURE__*/React.createElement("div", {
    className: "tb"
  }, label), /*#__PURE__*/React.createElement("div", null, value));
}

function PlayerTabs() {
  const {
    me,
    stats
  } = useContext(GameContext);
  if (!me) return null;
  return /*#__PURE__*/React.createElement(Tabs, null, /*#__PURE__*/React.createElement("div", {
    label: "stats",
    className: "ps vs"
  }, /*#__PURE__*/React.createElement(Stat, {
    label: "str",
    value: stats.str
  }), /*#__PURE__*/React.createElement(Stat, {
    label: "con",
    value: stats.con
  }), /*#__PURE__*/React.createElement(Stat, {
    label: "dex",
    value: stats.dex
  }), /*#__PURE__*/React.createElement(Stat, {
    label: "int",
    value: stats.int
  }), /*#__PURE__*/React.createElement(Stat, {
    label: "wiz",
    value: stats.wiz
  })), /*#__PURE__*/React.createElement("div", {
    label: "equipment",
    className: "ps vs"
  }, "Hello world"));
}

function MiniMap(props) {
  const {
    view,
    target
  } = useContext(GameContext);
  if (target) return null;
  return /*#__PURE__*/React.createElement("div", {
    className: "fac"
  }, /*#__PURE__*/React.createElement("pre", {
    id: "map",
    dangerouslySetInnerHTML: {
      __html: view
    }
  }));
}

function TargetTitleAndArt() {
  const {
    target,
    objects
  } = useContext(GameContext);
  if (!target) return null;
  const obj = objects[target];
  const src = "art/" + (obj.art || "unknown_small.jpg");
  return /*#__PURE__*/React.createElement(React.Fragment, null, /*#__PURE__*/React.createElement("div", {
    className: "tm pxs tac"
  }, obj.name), /*#__PURE__*/React.createElement("img", {
    className: "sr1 fac",
    src: src
  }));
}

function ContentsItem(props) {
  const {
    item,
    onClick,
    activeItem
  } = props;
  let iconEl = null;
  if (item.avatar) iconEl = /*#__PURE__*/React.createElement("img", {
    className: "s_xl svxl",
    src: "art/" + item.avatar
  });else iconEl = /*#__PURE__*/React.createElement("span", {
    className: "sxl txl tcv",
    dangerouslySetInnerHTML: {
      __html: item.icon
    }
  });
  const className = "f fic pxs _s " + (activeItem == item.dbref ? 'c0' : "");
  return /*#__PURE__*/React.createElement("a", {
    className: className,
    onClick: onClick
  }, iconEl, /*#__PURE__*/React.createElement("span", {
    dangerouslySetInnerHTML: {
      __html: item.pname
    }
  }));
}

function Contents(props) {
  const {
    onItemClick,
    activeItem
  } = props;
  const {
    target,
    here,
    objects
  } = useContext(GameContext);
  const obj = target ? objects[target] : objects[here];
  if (!here) return null;
  const contentsEl = Object.keys(obj.contents).map(k => {
    const item = obj.contents[k];
    return /*#__PURE__*/React.createElement(ContentsItem, {
      key: item.dbref,
      item: item,
      activeItem: activeItem,
      onClick: e => onItemClick(e, item)
    });
  });
  return /*#__PURE__*/React.createElement("div", {
    className: "vn fg oa icec"
  }, contentsEl);
}

function RB(props) {
  const {
    children,
    onClick
  } = props;
  return /*#__PURE__*/React.createElement("a", {
    className: "round txl ps c0",
    onClick: onClick
  }, children);
}

function RBT(props) {
  return /*#__PURE__*/React.createElement("a", {
    className: "round txl ps"
  });
}

function RBI(props) {
  const {
    onClick,
    src
  } = props;
  return /*#__PURE__*/React.createElement("a", {
    className: "round ps c0",
    onClick: onClick
  }, /*#__PURE__*/React.createElement("img", {
    className: "svl s_l",
    src: atiles[src]
  }));
}

function Directions() {
  const {
    sendMessage
  } = useContext(GameContext);
  return /*#__PURE__*/React.createElement("div", {
    className: "vn tar tnow abs al"
  }, /*#__PURE__*/React.createElement("div", {
    className: "_n"
  }, /*#__PURE__*/React.createElement(RBT, null), /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("k")
  }, "k"), /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("K")
  }, "K")), /*#__PURE__*/React.createElement("div", {
    className: "_n"
  }, /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("h")
  }, "h"), /*#__PURE__*/React.createElement(RBT, null), /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("l")
  }, "l")), /*#__PURE__*/React.createElement("div", {
    className: "_n"
  }, /*#__PURE__*/React.createElement(RBT, null), /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("j")
  }, "j"), /*#__PURE__*/React.createElement(RB, {
    onClick: () => sendMessage("J")
  }, "J")));
}

function ContentsAndActions(props) {
  const {
    sendMessage,
    here,
    me,
    target
  } = useContext(GameContext);
  const [actions, setActions] = useState([]);
  const [activeItem, setActiveItem] = useState(null);

  function onItemClick(ev, item) {
    let newActions = [];

    if (item.loc == here) {
      for (let p = 0; p < 9; p++) {
        if (!(parseInt(item.actions) & 1 << p)) continue;
        let id = actions_lbl[p];
        newActions.push([p, function () {
          sendMessage(id + " #" + item.dbref);
        }]);
      }
    } else if (item.loc == me) {
      newActions.push([ACT_EQUIP, function () {
        sendMessage("equip #" + item.dbref);
      }]);
      newActions.push([ACT_DROP, function () {
        sendMessage("drop #" + item.dbref);
      }]);
      newActions.push([ACT_EAT, function () {
        sendMessage("eat #" + item.dbref);
      }]);
    } else {
      newActions.push([ACT_GET, function () {
        sendMessage("get #" + target + "=#" + item.dbref);
      }]);
    }

    setActions(newActions);
    setActiveItem(item.dbref);
  }

  const actionsEl = actions.map(([p, cb]) => /*#__PURE__*/React.createElement(RBI, {
    key: p,
    onClick: cb,
    src: p
  }));
  return /*#__PURE__*/React.createElement(React.Fragment, null, /*#__PURE__*/React.createElement(Contents, {
    onItemClick: onItemClick,
    activeItem: activeItem
  }), /*#__PURE__*/React.createElement("div", {
    className: "_n icec"
  }, actionsEl));
}

function Help() {
  return /*#__PURE__*/React.createElement(React.Fragment, null, /*#__PURE__*/React.createElement("b", null, "Help"), /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), "Hopefully on a mobile phone you will be able to use", /*#__PURE__*/React.createElement("br", null), "the interface to interact with the game. But here are", /*#__PURE__*/React.createElement("br", null), "the keyboard commands for the game:", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Normal mode:"), /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("p", null, /*#__PURE__*/React.createElement("b", null, "s"), " To chat.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "a"), " to send commands.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "i"), " to access your inventory.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Left"), " or ", /*#__PURE__*/React.createElement("b", null, "h"), " to move west.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Right"), " or ", /*#__PURE__*/React.createElement("b", null, "l"), " to move east.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Up"), " or ", /*#__PURE__*/React.createElement("b", null, "k"), " to move north.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Down"), " or ", /*#__PURE__*/React.createElement("b", null, "j"), " to move south.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Shift+Up"), " or ", /*#__PURE__*/React.createElement("b", null, "K"), " to move up.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Shift+Down"), " or ", /*#__PURE__*/React.createElement("b", null, "J"), " to move down.", /*#__PURE__*/React.createElement("br", null)), /*#__PURE__*/React.createElement("b", null, "Input mode:"), /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("p", null, /*#__PURE__*/React.createElement("b", null, "Esc"), " to go back to normal mode.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Up"), " to travel up in history.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Down"), " to travel down in history.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("b", null, "Ctrl+u"), " to delete input text.", /*#__PURE__*/React.createElement("br", null)), "this game is very early stage, and it still requires a", /*#__PURE__*/React.createElement("br", null), "lot of work. but i might not be able to provide it full-time.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), "You can use it freely to chat with your friends.", /*#__PURE__*/React.createElement("br", null), "Meme at will.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), "You can check out the code ", /*#__PURE__*/React.createElement("a", {
    href: "https://github.com/tty-pt/neverdark"
  }, "here"), ".", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), "Hopefully the users will be able to create game content in the future.", /*#__PURE__*/React.createElement("br", null), "Meanwhile i will be working at the engine for that to be possible.", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("br", null), "The help command is very useful. Try issuing \"help help\".", /*#__PURE__*/React.createElement("br", null), /*#__PURE__*/React.createElement("p", null, /*#__PURE__*/React.createElement("b", null, "help startingout"), " will get you further."));
}

function Game() {
  const {
    sendMessage,
    session
  } = useContext(GameContext);
  const [modal, isOpen, setOpen] = useModal(Help, {});
  const input = useRef(null);

  function keyDownHandler(e) {
    if (document.activeElement == input.current) {
      switch (e.keyCode) {
        case 27:
          input.current.blur();
          break;

        case 85:
          if (e.ctrlKey) input.current.value = "";
          break;
      }

      return;
    }

    switch (e.keyCode) {
      case 83:
        input.current.value = "say ";
        input.current.focus();
        e.preventDefault();
        break;

      case 65:
        input.current.focus();
        e.preventDefault();
        break;

      case 75:
      case 38:
        if (e.shiftKey) sendMessage("K");else sendMessage("k");
        break;

      case 74:
      case 40:
        if (e.shiftKey) sendMessage("J");else sendMessage("j");
        break;

      case 72:
      case 37:
        sendMessage("h");
        break;

      case 76:
      case 39:
        sendMessage("l");
        break;

      case 73:
        sendMessage("inventory");
        break;
    }
  }

  useEffect(() => {
    window.addEventListener('keydown', keyDownHandler);
    return () => window.removeEventListener('keydown', keyDownHandler);
  }, [session]);

  function toggle_help() {
    setOpen(!isOpen);
  }

  function onSubmit(e) {
    e.preventDefault();
    const fd = new FormData(e.target);
    sendMessage(fd.get("cmd"));
    input.current.value = "";
    input.current.blur();
  }

  return /*#__PURE__*/React.createElement(React.Fragment, null, /*#__PURE__*/React.createElement("span", {
    className: "vn f"
  }, /*#__PURE__*/React.createElement(RB, {
    onClick: toggle_help
  }, "?"), /*#__PURE__*/React.createElement(RBI, {
    onClick: () => sendMessage('inventory'),
    src: ACT_OPEN
  }), /*#__PURE__*/React.createElement(RBI, {
    onClick: () => sendMessage('look'),
    src: ACT_LOOK
  })), /*#__PURE__*/React.createElement("form", {
    className: "vn f fg s_f",
    onSubmit: onSubmit
  }, /*#__PURE__*/React.createElement("div", {
    className: "_n f"
  }, /*#__PURE__*/React.createElement(PlayerTabs, null), /*#__PURE__*/React.createElement(RoomTitleAndArt, null)), /*#__PURE__*/React.createElement(Terminal, null), /*#__PURE__*/React.createElement("input", {
    ref: input,
    name: "cmd",
    className: "cf",
    autoComplete: "off",
    autoCapitalize: "off"
  })), /*#__PURE__*/React.createElement("div", {
    className: "vn f fg s_33"
  }, /*#__PURE__*/React.createElement(MiniMap, null), /*#__PURE__*/React.createElement(TargetTitleAndArt, null), /*#__PURE__*/React.createElement(Directions, null), /*#__PURE__*/React.createElement(ContentsAndActions, null)), modal);
}

function App() {
  return /*#__PURE__*/React.createElement(GameContextProvider, null, /*#__PURE__*/React.createElement(Game, null));
}

ReactDOM.render( /*#__PURE__*/React.createElement(App, null), document.getElementById('main'));

