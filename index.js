import { Sub, SubscribedElement } from "@tty-pt/sub";
import { createPopper } from "@popperjs/core";
import ndc from "@tty-pt/ndc";
import "@xterm/xterm/css/xterm.css";
import "@tty-pt/ndc/ndc.css";
import "./vim.css";
import "./styles.css";
import pkg from "./package.json";

const ws = ndc.connect();
ndc.open(document.getElementById("term"));

globalThis.sendCmd = text => {
  ws.send(text + "\n");
};

// sub {{{
const sub = new Sub({
  me: null,
  authFail: true,
  stats: {},
  bars: { hp: 1, hpMax: 1, mp: 1, mpMax: 1 },
  here: null,
  target: null,
  equipment: {},
  db: {
    "-1": { contents: {} },
    "1": { art: "mineral/gold/1.jpeg" },
    [null]: {},
  },
  view: "",
});
window.sub = sub;

const authEmit = sub.makeEmit(
  (me, authFail, current) => ({ ...current, me, authFail })
);

const statsEmit = sub.makeEmit(
  (stats, current) => ({ ...current, stats }),
);

const barsEmit = sub.makeEmit(
  (bars, current) => ({ ...current, bars })
);

const hereEmit = sub.makeEmit(
  (here, current) => ({ ...current, here }),
);

const targetEmit = sub.makeEmit(
  (target, current) => ({ ...current, target }),
);

const equipmentEmit = sub.makeEmit((item, current) => ({
  ...current,
  equipment: {
    ...current.equipment,
    [item.eql]: {
      ...item,
      pname: tty_proc(item.pname),
      icon: tty_proc(item.icon),
    },
  },
}));

const viewEmit = sub.makeEmit(
  (data, current) => ({ ...current, view: tty_proc(data) })
);

const dbEmit = sub.makeEmit((obj, current) => {
  const loc = current.db[obj.loc];

  if (!loc)
    return current;

  return ({
    ...current,
    db: {
      ...current.db,
      [obj.dbref]: { contents: {}, ...obj },
      [obj.loc]: {
        ...loc,
        contents: { ...loc.contents, [obj.dbref]: true },
      },
    },
  });
});
// }}}

// actions {{{
const ACTION = {
  LOOK: "👁",
  INSPECT: "🔍",
  FIGHT: "⚔️",
  SHOP: "💰",
  DRINK: "🧪",
  OPEN: "📦",
  CHOP: "🪓",
  FILL: "💧",
  GET: "🖐️",
  TALK: "👄",
  PUT: "👐",
  EQUIP: "👕",
  DROP: "🪣",
  EAT: "🥄 ",
  DIE: "☠️",
  INVENTORY: "🎒",
  K: "K",
  J: "J",
  WALK: "🗺️",
};

const ACTION_INDEX = [
  ACTION.INSPECT, ACTION.FIGHT, ACTION.SHOP, ACTION.DRINK, ACTION.OPEN,
  ACTION.CHOP, ACTION.FILL, ACTION.GET, ACTION.TALK, ACTION.PUT,
  ACTION.EQUIP, ACTION.DROP, ACTION.EAT, ACTION.DIE, ACTION.INVENTORY,
  ACTION.K, ACTION.J, ACTION.WALK, ACTION.LOOK
];

const ACTION_MAP = {
  [ACTION.LOOK]: { label: "look" },
  [ACTION.INSPECT]: {
	  label: "inspect",
	  callback: ref => sendCmd(`look #${ref}`),
  },
  [ACTION.FIGHT]: { label: "fight" },
  [ACTION.SHOP]: { label: "shop" },
  [ACTION.DRINK]: { label: "drink" },
  [ACTION.OPEN]: { label: "open" },
  [ACTION.CHOP]: { label: "chop" },
  [ACTION.FILL]: { label: "fill" },
  [ACTION.GET]: {
    label: "get",
    callback: ref => sendCmd(sub.value.target ? `get #${sub.value.target} #${ref}` : `get #${ref}`),
  },
  [ACTION.TALK]: { label: "talk" },
  [ACTION.PUT]: { label: "put" },
  [ACTION.EQUIP]: { label: "equip" },
  [ACTION.DROP]: { label: "drop" },
  [ACTION.EAT]: { label: "eat" },
  [ACTION.DIE]: { label: "die" },
  [ACTION.INVENTORY]: { label: "inventory" },
  [ACTION.K]: {
    label: "K",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAASUlEQVQ4jWNgoBU4ePjWf2RMsSFUcwFRLkNXgG4ANjbRzsZrAC7N+LxDlM0EXUOKs3GKE+tsYgwmLEkzA0hNvlgNIKSJ9gaQigF3uf+dQNoz9QAAAABJRU5ErkJggg==",
  },
  [ACTION.J]: {
    label: "J",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNgoDU4ePjWf4o0wzDVNCOL4zQYnwJkMawGEDIdqwEwBqnORjGAWGdjFSfF2QQNwOYdUg0mLEkzA4hKJIQMIKSJ9gaQigFrg/+dXV9AmAAAAABJRU5ErkJggg==",
  },
  [ACTION.WALK]: { label: "walk" },
};
// }}}

// equipment {{{
const EQUIPMENT = {
  HEAD: "HEAD",
  NECK: "NECK",
  CHEST: "CHEST",
  BACK: "BACK",
  RHAND: "RHAND",
  LFINGER: "LFINGER",
  RFINGER: "RFINGER",
  PANTS: "PANTS",
};

const EQUIPMENT_MAP = {
  [EQUIPMENT.HEAD]: {
    label: "hand",
  },
  [EQUIPMENT.NECK]: {
    label: "neck",
  },
  [EQUIPMENT.CHEST]: {
    label: "chest",
  },
  [EQUIPMENT.BACK]: {
    label: "back",
  },
  [EQUIPMENT.RHAND]: {
    label: "weapon",
  },
  [EQUIPMENT.LFINGER]: {
    label: "lfinger",
  },
  [EQUIPMENT.RFINGER]: {
    label: "rfinger",
  },
  [EQUIPMENT.PANTS]: {
    label: "grieves",
  },
}
// }}}

// tty {{{
function params_push(tty, x) {
  let fg = tty.c_attr.fg, bg = tty.c_attr.bg;

  switch (x) {
  case 0: fg = 7; bg = 0; break;
  case 1: fg += 8; break;
  default:
    if (x >= 40)
      bg = x - 40;
    else if (x >= 30)
      fg = (fg >= 8 ? 8 : 0) + x - 30;
  }

  tty.csi.fg = fg;
  tty.csi.bg = bg;
  tty.csi.x = x;
}

function csi_change(tty) {
  const a = tty.csi.fg != 7, b = tty.csi.bg != 0;

  tty.output += tty.end_tag;

  if (a || b) {
    tty.output += "<span class=\"";
    if (a)
      tty.output += "cf" + tty.csi.fg;
    if (b)
      tty.output += " c" + tty.csi.bg;

    tty.output += "\">";;
    tty.end_tag = "</span>";
  } else
    tty.end_tag = "";
}

function
esc_state_0(tty, ch) {
  if (tty.csi_changed) {
    csi_change(tty);
    tty.csi_changed = 0;
  }

  switch (ch) {
  case '<': tty.output += "&lt;"; return 4;
  case '>': tty.output += "&gt;"; return 4;
  case '"': tty.output += "\""; return 2;
  }

  tty.output += ch;
  return 1;
}

function tty_proc_ch(tty, ch) {
	switch (ch) {
	case '\x18':
	case '\x1a': tty.esc_state = 0; return 0;
	case '\x1b': tty.esc_state = 1; return 0;
	case '\x9b': tty.esc_state = 2; return 0;
	case '\x07': 
	case '\x00':
	case '\x7f':
	case '\v':
	case '\r':
	case '\f': return 0;
	}

	switch (tty.esc_state) {
	case 0: return esc_state_0(tty, ch);
	case 1:
    switch (ch) {
    case '[': tty.esc_state = 2; break;
    case '=':
    case '>':
    case 'H': tty.esc_state = 0; /* IGNORED */
    }
    break;
	case 2: // just saw CSI
		switch (ch) {
		case 'K':
		case 'H':
		case 'J': tty.esc_state = 0; return 0;
		case '?': tty.esc_state = 5; return 0;
		}
		params_push(tty, 0);
		tty.esc_state = 3;
	case 3: // saw CSI and parameters
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
		case '[': tty.esc_state = 4; break;
		case ';': params_push(tty, 0); break;
		default:
			if (ch >= '0' && ch <= '9')
				params_push(tty, tty.csi.x * 10 + (ch - '0'));
			else
				tty.esc_state = 0;
		}
		break;

	case 5: params_push(tty, ch); tty.esc_state = 6; break;
	case 4:
	case 6: tty.esc_state = 0; break;
	}

	return 0;
}

function tty_proc(input) {
  let tty = {
    csi: { fg: 7, bg: 0, x: 0 },
    c_attr: { fg: 7, bg: 0, x: 0 },
    end_tag: "",
    esc_state: 0,
    output: "",
  };
  let in_i;

  for (in_i = 0; in_i < input.length; in_i++)
    tty_proc_ch(tty, input.charAt(in_i));

  return tty.output;
}
// }}}

// onMessage {{{
function read_16(arr, start) {
  return (arr[start + 1] << 8) | arr[start];
}

function read_32(arr, start) {
  return ((arr[start + 3] << 24) | (arr[start + 2] << 16) | (arr[start + 1] << 8) | arr[start]) << 32;
}

function read_u16(arr, start) {
  return (arr[start + 1] << 8) | arr[start];
}

function read_u32(arr, start) {
  return read_32(arr, start) >>> 0;
}

function read_string(arr, start, ret = {}) {
  const slice = arr.slice(start);
  let len = 0;

  for (const item of slice)
    if (item === 0)
      break;
    else
      len ++;

  const rret = String.fromCharCode.apply(null, slice.slice(0, len));

  ret[start] = len;
  return rret;
}

const BCP = {
  BARS: 1,
  EQUIPMENT: 2,
  STATS: 3,
  ITEM: 4,
  VIEW_BUFFER: 6,
  AUTH_FAILURE: 9,
  AUTH_SUCCESS: 10,
  OUT: 11,
};

const BCP_MAP = {
  [BCP.BARS]: { label: "bars" },
  [BCP.EQUIPMENT]: { label: "equipment" },
  [BCP.STATS]: { label: "stats" },
  [BCP.ITEM]: { label: "item" },
  [BCP.VIEW_BUFFER]: { label: "view_buffer" },
  [BCP.AUTH_FAILURE]: { label: "auth_failure" },
  [BCP.AUTH_SUCCESS]: { label: "auth_success" },
  [BCP.OUT]: { label: "out" },
};

const bg = document.getElementById("bg");

ndc.setOnMessage(function onMessage(ev) {
  const arr = new Uint8Array(ev.data);
  if (String.fromCharCode(arr[0]) == "#" && String.fromCharCode(arr[1]) == "b") {
    const iden = arr[2];
    // console.log('BCP', BCP_MAP[iden]);
    switch (iden) {
    case BCP.BARS: {
      let aux;
      barsEmit({
        hp: read_u16(arr, aux = 3),
        hpMax: read_u16(arr, aux += 2),
        mp: read_u16(arr, aux += 2),
        mpMax: read_u16(arr, aux += 2),
      });
      return;

    } case BCP.EQUIPMENT: {
      let aux;
      equipmentEmit({
        head: read_32(arr, aux = 3),
        neck: read_32(arr, aux += 4),
        chest: read_32(arr, aux += 4),
        back: read_32(arr, aux += 4),
        rhand: read_32(arr, aux += 4),
        lfinger: read_32(arr, aux += 4),
        rfinger: read_32(arr, aux += 4),
      });
      return;

    } case BCP.STATS: {
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

    } case BCP.ITEM:
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

      dbEmit(base);
      if (base.dynflags === 1) {
        // dbEmit(base.dbref, base);
        if (base.type === 0) {
          hereEmit(base.dbref);
          targetEmit(null);
          bg.style = `background-image: url('${pkg.publicPath}/art/${base.art}')`;
        } else
          targetEmit(base.dbref);

        ndc.term.write(base.description ? "You see: " + base.description + "\r\n" : "");
      }

      return;
    case BCP.VIEW_BUFFER:
      viewEmit(String.fromCharCode.apply(null, arr.slice(3)));
      return;
      case BCP.AUTH_FAILURE:
      authEmit(-1, true);
      return;
    case BCP.AUTH_SUCCESS:
      authEmit(read_32(arr, 3), undefined);
      return;
    case BCP.OUT: {
      let aux;

      const { dbref } = {
        dbref: read_32(arr, aux = 3),
        loc: read_32(arr, aux += 4), // new loc
      };

      const obj = sub.value.db[dbref];
      const oldLoc = sub.value.db[obj.loc];
      let newContents = { ...oldLoc.contents };
      delete newContents[dbref];
      dbEmit({ ...oldLoc, contents: newContents });

      return;
    }}
  } else
    return true;
});
// }}}

function mayDash(str) {
  return str;
}

// function mayDash(str) {
//   return str ? "-" + str : "";
// }

class Bar extends SubscribedElement {
  static get observedAttributes() {
    return ['type', 'x'];
  }

  connectedCallback() {
    const type = this.getAttribute('type');
    this.subscribe(sub, "value", "bars." + type);
    this.subscribe(sub, "max", "bars." + type + "Max");
    this.render();
  }


  render() {
    const type = this.getAttribute('type');
    const cls = this.getAttribute("x");
    const width = this.value / this.max;

    this.innerHTML = `<div class="rel">`
      + `<div style="width: ${width * 100}%" class="sv ${cls}"></div>`
      + `<div class="abs a ${cls} tr5"></div>`
      + `<div class="ts9 shad abs tac a cf15">${this.value}/${this.max}</div>`
      + `</div>`;
  }
}

customElements.define('nd-bar', Bar);


class Button extends SubscribedElement {
  static get observedAttributes() {
    return ['size', 'square', "icon", "src", "x", "text-size", "pad", "fit", "hid"];
  }

  render() {
    super.render();
    const textSize = mayDash(this.getAttribute('text-size') ?? "17");
    const size = mayDash(this.getAttribute('size') ?? "8");
    const sizeCls = this.getAttribute("size") ?  "s" + size : "";
    const padSize = mayDash(this.getAttribute('pad') ?? "8");
    const square = this.getAttribute('square');
    const icon = this.textContent.trim();
    const src = this.getAttribute('src') ?? ACTION_MAP[icon]?.icon;
    const fit = this.getAttribute('fit') === null ? null : "f";
    const shape = square ? "" : "round";
    const content = src ? "" : icon;
    const customCls = this.getAttribute("x");
    const hid = this.getAttribute("hid") !== null;
    const cls = `btn bs${fit ?? size} p${padSize} ${sizeCls} ts${textSize} `
      + `${shape} ${customCls} ${hid ? "transparent" : ""}`;

    this.innerHTML = `<button style="background-image: url('${src}')" `
      + `class="${cls}" tabindex="${hid ? -1 : ""}">${content}</button>`;
  }
}

customElements.define('nd-button', Button);

class Avatar extends Button {
  static get observedAttributes() {
    return ['size', 'square', 'ref'];
  }

  connectedCallback() {
    const ref = this.getAttribute('ref');
    this.subscribe(sub, this.setImageClass, "db." + ref ?? "");
    this.setAttribute("fit", "");
    this.render();
  }

  setImageClass(obj) {
    const src = pkg.publicPath + "/art/" + (obj?.art ?? obj?.avatar ?? "unknown.jpg");
    this.setAttribute("src", src);
  }
}

customElements.define('nd-avatar', Avatar);

class Equipment extends Avatar {
  connectedCallback() {
    const location = this.getAttribute('location');
    if (location) {
      this.subscribe(sub, "ref", "equipment/" + location ?? "");
      this.setAttribute("ref", "unknown");
    } else
      this.setAttribute("ref", null);
    this.setAttribute("size", "24");
    this.setAttribute("square", true);
    this.setAttribute("onclick", `sendCmd('unequip ${location}')`);
    this.setAttribute("ontouchend", `sendCmd('unequip ${location}')`);
    this.render();
  }
}

customElements.define('nd-equipment', Equipment);

class Stat extends SubscribedElement {
  connectedCallback() {
    const name = this.getAttribute('name');
    this.subscribe(sub, "value", `stats.${name}`);
    this.render();
  }

  render() {
    const name = this.getAttribute('name');
    const value = 0;

    this.innerHTML = `
    <div class="f h8">
      <div class="tbbold">${name}</div>
      <div>${this.value}</div>
    </div>`;
  }
}

customElements.define('nd-stat', Stat);

class LookAt extends SubscribedElement {
  connectedCallback() {
    this.subscribe(sub, "content", "view");
    this.subscribe(sub, "db", "db");
    this.subscribe(sub, "target", "target");
    this.render();
  }

  render() {
    super.render();
    const target = this.target ? this.db[this.target] : null;
    this.innerHTML = target
      ? `
    <div id="target-title-and-art" class="rel v0 size-super fic">
      <div style="background-image: url('${pkg.publicPath}/art/${target.art}')" class="abs a background-unique bsf"></div>
      <div id="target-title" class="abs a deep-shadow tac tm pxs">
        ${target.pname}
      </div>
    </div>`
      : `<div class="fac"><pre id="map">${this.content}</pre></div>`;
  }
}

customElements.define('nd-look-at', LookAt);

class Item extends SubscribedElement {
  constructor() {
    super();
    this.parent = this.closest("nd-contents");
    this.addEventListener("click", () => {
      const ref = this.getAttribute('ref');
      this.parent.setActive(
        this.parent.active === ref ? undefined : ref
      );
    });
  }

  connectedCallback() {
    const ref = this.getAttribute('ref');
    this.subscribe(sub, "item", `db.${ref ?? ""}`);
    this.render();
  }

  render() {
    super.render();
    if (!this.item)
      return;
    const background = this.parent.active === this.getAttribute("ref") ? "c0" : "";
    const text = this.item.pname + (this.item.shop ? " " + this.item.price + "P" : "");
    this.innerHTML = `
    <a class="f h fic round-pad p display rel ${background}" onclick="">
      <nd-avatar size="64" ref="${this.item.dbref}"></nd-avatar>
      <span>${text}</span>
    </a>`;
  }
}

customElements.define('nd-item', Item);

globalThis.actionCallback = (id, ref) => {
  const action = ACTION_MAP[id];
  if (action.callback)
    action.callback(ref);
  else
    sendCmd(action.label + " #" + ref + "\n");
};

class ContentActions extends SubscribedElement {
  active = null;

  connectedCallback() {
    this.subscribe(sub, "db", "db");
    this.render();
  }

  setActive(ref) {
    this.active = ref;
    this.render();
  }

  render() {
    const actions = [];
    const item = this.db[this.active];

    for (let p = 0; p < 9; p++)
      if ((parseInt(item.actions) & (1 << p)))
        actions.push(ACTION_INDEX[p]);

    const actionsEl = actions.map(action => (`<nd-button
      ref="${this.active}"
      onclick="actionCallback('${action}', ${this.active})"
      ontouchend="actionCallback('${action}', ${this.active})"
    >${action}</nd-button>`)).join("\n");

    this.innerHTML = `
    <div class="mr8 popper">
      <div class="f h8 fw icec">${actionsEl}</div>
    </div>`;
  }
}

customElements.define('nd-content-actions', ContentActions);

class Contents extends SubscribedElement {
  active = "";

  connectedCallback() {
    this.subscribe(sub, "db", "db");
    this.subscribe(sub, "here", "here");
    this.subscribe(sub, "target", "target");
    this.render();
  }

  setActive(ref) {
    this.active = ref;
    const last = this.activeEl;
    if (last) {
      this.activeEl = null;
      this.popper.destroy();
      last.render();
    }
    if (ref) {
      this.actions.classList.remove("hidden");
      this.activeEl = this.querySelector(`nd-item[ref='${ref}']`);
      this.popper = createPopper(
        this.activeEl,
        this.actions,
        { placement: "left" }
      );
      this.actions.setActive(ref);
      this.activeEl.render();
    } else
      this.actions.classList.add("hidden");
  }

  render() {
    super.render();
    const loc = this.db[this.target ?? this.here];
    const contentsEl = Object.keys(loc?.contents ?? {}).map(ref => {
      const activeAttr = this.active === ref ? "active" : "";
      return `<nd-item ref="${ref}" ${activeAttr}></nd-item>`;
    }).join("\n");
    this.innerHTML = `
    <div class="v0 fg overflow icec">${contentsEl}</div>
    <nd-content-actions class="hidden"></nd-content-actions>
    `;
    this.actions = this.querySelector("nd-content-actions");
    this.active = null;
  }
}

customElements.define('nd-contents', Contents);

function keyUpHandler(e) {
  if (ndc.term.focused)
    return;
  switch (e.keyCode) {
    case 75: // k
    case 38: // ArrowUp
      if (e.shiftKey)
        sendCmd("K");
      else
        sendCmd("k");
      break;
    case 74: // j
    case 40: // ArrowDown
      if (e.shiftKey)
        sendCmd("J");
      else
        sendCmd("j");
      break;
    case 72: // h
    case 37: // ArrowLeft
      sendCmd("h");
      break;
    case 76: // l
    case 39: // ArrowRight
      sendCmd("l");
      break;
    case 73: // i
      if (e.shiftKey)
        term.value.focus();
      else
        sendCmd("inventory");
      break;
    case 79: // o
      sendCmd("look");
      break;
    default:
  }
}

window.addEventListener('keyup', keyUpHandler);

const modals = {
  "equipment": document.getElementById("equipment"),
  "stats": document.getElementById("stats"),
};

window.help_show = function () {
	ndc.term.focus();
	sendCmd("help begin");
};

const modalParent = modals.equipment.parentElement;
let currentModal = modals.equipment;

globalThis.modal_open = id => {
  modalParent.classList.remove("dn");
  currentModal.classList.add("dn");
  currentModal = modals[id];
  currentModal.classList.remove("dn");
};

globalThis.modal_close = () => {
  modalParent.classList.add("dn");
};

const directions = document.getElementById("directions");
const actions = document.getElementById("actions");

globalThis.directions_show = () => {
  directions.classList.remove("dn");
  actions.classList.add("dn");
};

document.addEventListener("click", () => {
  directions.classList.add("dn");
  actions.classList.remove("dn");
});
