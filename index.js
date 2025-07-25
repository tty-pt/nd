import { Sub, SubscribedElement } from "@tty-pt/sub";
import { createPopper } from "@popperjs/core";
import ndc from "@tty-pt/ndc";
// import "@xterm/xterm/css/xterm.css";
// import "@tty-pt/ndc/ndc.css";

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
  hp: { val: 1, max: 1 },
  mp: { val: 1, max: 1 },
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

const hpEmit = sub.makeEmit(
  (hp, current) => ({ ...current, hp })
);

const mpEmit = sub.makeEmit(
  (mp, current) => ({ ...current, mp })
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

  if (!loc) {
    console.warn("dbEmit without loc already present", obj);
    return current;
  }

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
  K: "↗",
  J: "↘",
  WALK: "🗺️",
};

const ACTION_INDEX = [
	ACTION.INSPECT,
	ACTION.OPEN,
	ACTION.GET,
	ACTION.TALK,
	ACTION.FILL,
	ACTION.DRINK,
	ACTION.EAT,
	ACTION.FIGHT,
	ACTION.EQUIP,
	ACTION.SHOP,
	ACTION.CHOP,
  // ACTION.TALK, ACTION.PUT,
  // ACTION.DROP, ACTION.DIE, ACTION.INVENTORY,
  // ACTION.K, ACTION.J, ACTION.WALK, ACTION.LOOK
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
  [ACTION.K]: { label: "K" },
  [ACTION.J]: { label: "J" },
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

  if (!input)
	return tty.output;

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
  ITEM: 0,
  VIEW: 1,
  VIEW_BUFFER: 2,
  ROOM: 3,
  ENTITY: 4,
  AUTH_FAILURE: 5,
  AUTH_SUCCESS: 6,
  OUT: 7,
  TOD: 8,
  HP: 9,
  STATS: 10,
  EQUIPMENT: 11,
  MP: 12,
};

const BCP_MAP = {
  [BCP.HP]: { label: "hp" },
  [BCP.MP]: { label: "mp" },
  [BCP.BARS]: { label: "bars" },
  [BCP.STATS]: { label: "stats" },
  [BCP.ITEM]: { label: "item" },
  [BCP.VIEW_BUFFER]: { label: "view_buffer" },
  [BCP.AUTH_FAILURE]: { label: "auth_failure" },
  [BCP.AUTH_SUCCESS]: { label: "auth_success" },
  [BCP.OUT]: { label: "out" },
  [BCP.TOD]: { label: "tod" },
  [BCP.EQUIPMENT]: { label: "equipment" },
};

const title = document.getElementById("title");
title.parentNode.removeChild(title);

const bg = document.getElementById("main");

ndc.setOnMessage(function onMessage(ev) {
  const arr = new Uint8Array(ev.data);
  if (String.fromCharCode(arr[0]) == "#" && String.fromCharCode(arr[1]) == "b") {
    const iden = arr[2];
    console.log(iden, BCP_MAP[iden]);
    // TODO remove repeate code in emits!!
    switch (iden) {
    case BCP.HP: {
      let aux;
      hpEmit({
        val: read_u16(arr, aux = 3),
        max: read_u16(arr, aux += 2),
      });
      return;

    } case BCP.MP: {
      let aux;
      mpEmit({
        val: read_u16(arr, aux = 3),
        max: read_u16(arr, aux += 2),
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
        icon: {
          fg: read_32(arr, aux += ret[aux] + 1),
          flags: read_u32(arr, aux += 4),
          ch: arr[aux += 4],
        },
        art: read_string(arr, aux += 1, ret),
      };

      switch (base.type) {
      case 0: // TYPE_ROOM
        base.exits = read_32(arr, aux += ret[aux] + 1);
        break;
      case 3: // TYPE_ENTITY
        base.flags = read_u32(arr, aux += ret[aux] + 1);
        break;
      }

      console.log("ITEM", base);
      dbEmit(base);
      if (base.dynflags === 1) {
        // dbEmit(base.dbref, base);
        if (base.type === 0) {
          hereEmit(base.dbref);
          targetEmit(null);
          bg.style = `background: url('/nd/art/${base.art}') center/cover no-repeat;`;
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
    case BCP.TOD: {
      const res = read_u32(arr, 3);
      if (res) {
        bg.classList.add("night");
        bg.classList.remove("day");
      } else {
        bg.classList.remove("night");
        bg.classList.add("day");
      }
      return;
    }
    case BCP.OUT: {
      let aux;

      const { dbref } = {
        dbref: read_32(arr, aux = 3),
        loc: read_32(arr, aux += 4), // new loc
      };

      const obj = sub.value.db[dbref];
      const oldLoc = sub.value.db[obj.loc];
      if (oldLoc) {
	      let newContents = { ...oldLoc.contents };
	      delete newContents[dbref];
	      dbEmit({ ...oldLoc, contents: newContents });
      }

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
    this.subscribe(sub, "value", type + ".val");
    this.subscribe(sub, "max", type + ".max");
    this.render();
  }


  render() {
    const type = this.getAttribute('type');
    const cls = this.getAttribute("x");
    const width = this.value / this.max;

    this.innerHTML = ``
      + `<div style="width: ${width * 100}%" class="svf ${cls}"></div>`
      + `<div class="abs a ${cls} tr5"></div>`
      + `<div class="ts9 shad abs tac a cf15">${this.value}/${this.max}</div>`;
  }
}

customElements.define('nd-bar', Bar);


class Button extends SubscribedElement {
  static get observedAttributes() {
    return ['size', 'square', "icon", "src", "x", "text-size", "pad", "fit", "hid"];
  }

  render() {
    super.render();
    const icon = this.textContent.trim();
    const src = this.getAttribute('src') ?? ACTION_MAP[icon]?.icon;
    const content = src ? "" : icon;

    const size = this.getAttribute('size');

    this.className = "btn " + this.getAttribute("x")
	  + " p" + mayDash(this.getAttribute('pad') ?? "8")
	  + " bs" + (
		  this.getAttribute("fit") === null
		  ? mayDash(size ?? "8") : "f"
	  );

    if (!src)
	  this.className += " ts" + mayDash(this.getAttribute('text-size') ?? "17");

    if (size)
	  this.className += ' s' + mayDash(size);

    if (!this.getAttribute('square'))
	  this.className += " round";

  if (src)
	  this.style = `background-image: url('${src}')`;

    if (this.getAttribute("hid") !== null) {
	this.tabIndex = -1;
        this.className += ' transparent';
    }

    this.innerHTML = content;
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
    const src = "/nd/art/" + (obj?.art ?? obj?.avatar ?? "unknown.jpg");
    this.setAttribute("src", src);
    this.setAttribute("x", "cf" + (obj.icon.fg + 8));
    this.textContent = String.fromCharCode(obj.icon.ch);
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
      <div class="tbbold">${name}</div>
      <div>${this.value}</div>
    `;
  }
}

customElements.define('nd-stat', Stat);

class Map extends SubscribedElement {
  connectedCallback() {
    this.subscribe(sub, "content", "view");
    this.render();
  }

  render() {
    super.render();
    this.innerHTML = this.content;
  }
}

customElements.define('nd-map', Map);

class NoTarget extends SubscribedElement {
  connectedCallback() {
    this.subscribe(sub, "target", "target");
    this.render();
  }

  render() {
    super.render();
    if (this.target)
	  this.style.display = 'none';
    else
	  this.style.display = 'flex';
  }
}

customElements.define('nd-notarget', NoTarget);

const holder = document.getElementById("holder");

if ('ontouchstart' in window)
	holder.classList.add('touch');

class LookAt extends SubscribedElement {
  connectedCallback() {
    this.subscribe(sub, "db", "db");
    this.subscribe(sub, "target", "target");
    this.render();
  }

  render() {
    super.render();

    if (!this.target) {
	  this.style.display = 'none';
          holder.classList.remove("targetting");
	  return;
    }

    holder.classList.add("targeting");
    const target = this.db[this.target];

    if (!target)
	  return;

    this.style.display = 'flex';
    this.innerHTML = `
    <div id="target-title-and-art" class="rel v0 fic">
      <div style="background-image: url('/nd/art/${target.art}')" class="abs a background-unique bsf"></div>
      <div id="target-title" class="abs a deep-shadow tac tm pxs">
        ${target.pname}
      </div>
    </div>`;
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
    const text = this.item.pname + (this.item.shop ? " " + this.item.price + "P" : "");
    if (!this.item)
      return;
    const background = this.parent.active === this.getAttribute("ref") ? "ctb" : "";
    this.className = background;
    this.innerHTML = `
      <nd-avatar ref="${this.item.dbref}"></nd-avatar>
      <span>${text}</span>
    `;
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

const popperOpts = {
  placement: "left",
  modifiers: [
    {
      name: 'flip',
      options: {
        fallbackPlacements: ['bottom'], // alternativa se não couber à esquerda
      },
    },
    {
      name: 'preventOverflow',
      options: {
        boundary: 'clippingParents',
      },
    },
  ],
};

class Contents extends SubscribedElement {
  active = "";

  connectedCallback() {
    this.subscribe(sub, "db", "db");
    this.subscribe(sub, "here", "here");
    this.subscribe(sub, "target", "target");

    this.addEventListener("scroll", () => {
      this.setActive(undefined);
    });

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
      this.actions.classList.remove("dn");
      this.activeEl = this.querySelector(`nd-item[ref='${ref}']`);
      this.popper = createPopper(
        this.activeEl,
        this.actions,
        popperOpts
      );
      this.actions.setActive(ref);
      this.activeEl.render();
    } else
      this.actions.classList.add("dn");
  }

  render() {
    super.render();
    const loc = this.db[this.target ?? this.here];
    const contentsEl = Object.keys(loc?.contents ?? {}).map(ref => {
      const activeAttr = this.active === ref ? "active" : "";
      return `<nd-item ref="${ref}" ${activeAttr}></nd-item>`;
    }).join("\n");
    this.innerHTML = `
    ${contentsEl}
    <nd-content-actions class="dn"></nd-content-actions>
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

ndc.term.element.addEventListener("focusin", () => {
	holder.classList.add("terminal-mode");
});

ndc.term.element.addEventListener("focusout", () => {
	holder.classList.remove("terminal-mode");
});
