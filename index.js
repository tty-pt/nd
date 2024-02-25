import { Sub, SubscribedElement } from "@tty-pt/sub";
import { createPopper } from "@popperjs/core";
import ndc from "@tty-pt/ndc";
import "xterm/css/xterm.css";
import "@tty-pt/ndc/ndc.css";
import "./vim.css";
import "./styles.css";

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
  bars: { hp: 1, mp: 1 },
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
  (bars, current) => ({ ...current, bars }),
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
  LOOK: "LOOK",
  KILL: "KILL",
  SHOP: "SHOP",
  DRINK: "DRINK",
  OPEN: "OPEN",
  CHOP: "CHOP",
  FILL: "FILL",
  GET: "GET",
  TALK: "TALK",
  PUT: "PUT",
  EQUIP: "EQUIP",
  DROP: "DROP",
  EAT: "EAT",
  DIE: "DIE",
  INVENTORY: "INVENTORY",
  K: "K",
  J: "J",
  WALK: "WALK",
};

const ACTION_INDEX = [
  ACTION.LOOK, ACTION.KILL, ACTION.SHOP, ACTION.DRINK, ACTION.OPEN,
  ACTION.CHOP, ACTION.FILL, ACTION.GET, ACTION.TALK, ACTION.PUT,
  ACTION.EQUIP, ACTION.DROP, ACTION.EAT, ACTION.DIE, ACTION.INVENTORY,
  ACTION.K, ACTION.J, ACTION.WALK,
];

const ACTION_MAP = {
  [ACTION.LOOK]: {
    label: "look",
    icon: 	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQklEQVQ4jWNgGL7g4OFb/9ExWZpwYZyaiXEBhiH4NBNSA+fg8wpWW5H1ETIAl98xvEHIEJyacfmdEB8vICsdjFAAAGW58imbroFwAAAAAElFTkSuQmCC",
  },
  [ACTION.KILL]: {
    label: "kill",
    icon: 	"data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jWNgoAY4ePjW/4OHb/0nWw+MQ6whWNUTawhedYQMIcoSXIpI8ia6YlLDCMMQcmKJdGdTxQW4/IyNT7KziXYJMemAoBewOROfHG5BAi4lRT1OAAA/Xu7MVtQQRgAAAABJRU5ErkJggg==",
  },
  [ACTION.SHOP]: {
    label: "shop",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAOUlEQVQ4jWNgGJTg4OFb/3FhkjXjEiNaMzFyWBXB+Li8RHsDKPYCIZfQxwCSvYBLIcWJiSTNQw8AAO8uLsItXTaGAAAAAElFTkSuQmCC",
  },
  [ACTION.DRINK]: {
    label: "drink",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgIBIcPHzrPzImWREyn2wDyHIBuhhJBhDUgM8QdDZJmim2nWwDcBlCkmZ0Q8iynSRN2AwYGP9T3QAYnyQDyAEAd7gQfVonw9EAAAAASUVORK5CYII=",
  },
  [ACTION.OPEN]: {
    label: "open",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAVElEQVQ4jaXRywkAMAgDUPffStzCZey1FE3UCt7Cw48IKTUPNQ+Wo8AauYEV8gIQycJVQ6ALwwlWq7DAGEBIegO07+iV3wBDWgBC2kCFjIAMGQOoDj9EP5M9YZdfAAAAAElFTkSuQmCC",
  },
  [ACTION.CHOP]: {
    label: "chop",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGLTg4OFb/5ExyRoGjwHIcmQZgi5HsiHY5EgyBJccQUOIsWBwGEKRAcMEAABAgauIKxMDjQAAAABJRU5ErkJggg==",
  },
  [ACTION.FILL]: {
    label: "fill",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAUElEQVQ4jWNgwAEOHr71H5cc0QBmCEWGUcUlZNuMjskyBJmmnwHYnEySNw4evvX/////TLhcRFAzJfLUMYAQxmsAIUMIaqaKAdgMIkkjKQAABfenzXQV7xsAAAAASUVORK5CYII=",
  },
  [ACTION.GET]: {
    label: "get",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcwWQYQkiPJFQQNgClGp8lOraMANwAA27bDE5yTd30AAAAASUVORK5CYII=",
    callback: ref => sendCmd(sub.value.target ? `get #${sub.value.target} #${ref}` : `get #${ref}`),
  },
  [ACTION.TALK]: {
    label: "talk",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAM0lEQVQ4jWNgGFTg4OFb/4nFOA0g1iK8EoRsHhgXkGQAxS6gyACKo5FYV1DkBeprHtoAAOijqCv2+qTkAAAAAElFTkSuQmCC",
  },
  [ACTION.PUT]: {
    label: "put",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jc3QwQ0AMAQFUPtvJbawjF4qaUVQcair/18EwB4kFiQW6IyW24gF/kKewQwpQWNABJcBWxq7IgU03C5HUH2ZZJBYwrPOx1no6mWI3bv5yiVebgFV9xc3cxaEggAAAABJRU5ErkJggg==",
  },
  [ACTION.EQUIP]: {
    label: "equip",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAALElEQVQ4jWNgGF7g4OFb/5ExsXI4FeHDRLuCJM3EGEJWWIxEA3AZQrTmEQYAvLnktS1HPJUAAAAASUVORK5CYII=",
  },
  [ACTION.DROP]: {
    label: "drop",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcw0Qaga6KaKwgaAFNMtmZ8BtHPAOSAI9sgipI2ukuwAQABHugSAyRIzQAAAABJRU5ErkJggg==",
  },
  [ACTION.EAT]: {
    label: "eat",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNgoDY4ePjWf7LkYRLoNLoasg2AaR6iBiBrpq0BuBQOEQPQFeIzHKcBMMWEXEeyAfhcR7Iisg3AJQcA7LE1fLFqxRoAAAAASUVORK5CYII=",
  },
  [ACTION.DIE]: {
    label: "die",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgoCU4ePjWfximSDNZhlDFAGw02a4gWTNJtuOzBZ84hhwxziXKS9gUURyVJGlEt5EkQwgFKE6DSLGForRBdQAAjPakzh7dkTsAAAAASUVORK5CYII=",
  },
  [ACTION.INVENTORY]: {
    label: "inventory",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGFTg4OFb/4nBBA0hRw7FBWTJE+U8fOqI0YxX7agBVDKAomhElqQoKdMdAADVVJq360zbaQAAAABJRU5ErkJggg==",
  },
  [ACTION.K]: {
    label: "K",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAASUlEQVQ4jWNgoBU4ePjWf2RMsSFUcwFRLkNXgG4ANjbRzsZrAC7N+LxDlM0EXUOKs3GKE+tsYgwmLEkzA0hNvlgNIKSJ9gaQigF3uf+dQNoz9QAAAABJRU5ErkJggg==",
  },
  [ACTION.J]: {
    label: "J",
    icon: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNgoDU4ePjWf4o0wzDVNCOL4zQYnwJkMawGEDIdqwEwBqnORjGAWGdjFSfF2QQNwOYdUg0mLEkzA4hKJIQMIKSJ9gaQigFrg/+dXV9AmAAAAABJRU5ErkJggg==",
  },
  [ACTION.WALK]: {
    label: "walk",
    icon: "./art/walking.png",
  },
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
  return ((arr[start + 1] << 8) | arr[start]) << 32;
}

function read_32(arr, start) {
  return ((arr[start + 3] << 24) | (arr[start + 2] << 16) | (arr[start + 1] << 8) | arr[start]) << 32;
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
    console.log("BCP", iden, BCP_MAP[iden]);
    switch (iden) {
    case BCP.BARS: {
      let aux;
      barsEmit({
        hp: read_32(arr, aux = 3),
        hpMax: read_32(arr, aux += 4),
        mp: read_32(arr, aux += 4),
        mpMax: read_32(arr, aux += 4),
      });
      return;

    } case BCP.EQUIPMENT: {
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
          bg.style = `background-image: url('./art/${base.art}')`;
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
  return str ? "-" + str : "";
}

class Button extends SubscribedElement {
  static get observedAttributes() {
    return ['size', 'square', "icon", "src"];
  }

  render() {
    super.render();
    const size = mayDash(this.getAttribute('size') ?? "medium");
    const square = this.getAttribute('square');
    const icon = this.textContent.trim();
    const src = this.getAttribute('src') ?? ACTION_MAP[icon]?.icon;
    const fit = this.getAttribute('fit') === null ? null : "-fit";
    const shape = square ? "" : "round";
    const content = src ? "" : icon;
    const vis = src || icon;
    const cls = `background-size${fit ?? size} size${size} font-size${size} `
      + `${shape} ${vis ? "" : "transparent"}`;

    this.innerHTML = `<button style="background-image: url('${src}')" `
      + `class="${cls}" tabindex="${vis ? "" : -1}">${content}</button>`;
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
    const src = "./art/" + (obj?.art ?? obj?.avatar ?? "unknown.jpg");
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
    this.setAttribute("size", "medium");
    this.setAttribute("square", true);
    this.setAttribute("onclick", `sendCmd('unequip ${location}')`);
    this.render();
  }
}

customElements.define('nd-equipment', Equipment);

class Stat extends HTMLElement {
  connectedCallback() {
    this.render();
  }

  render() {
    const name = this.getAttribute('name');
    const value = 0;

    this.innerHTML = `
    <div class="horizontal-small">
      <div class="tbbold">${name}</div>
      <div>${value}</div>
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
    <div id="target-title-and-art" class="relative vertical-0 size-super align-items">
      <div style="background-image: url('${target.art}')" class="absolute position-0 background-unique background-size-fit"></div>
      <div id="target-title" class="absolute position-top-0 position-bottom-0 position-left-0 position-right-0 deep-shadow text-align tm pxs">
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
    <a class="horizontal align-items round-pad pad display relative ${background}" onclick="">
      <nd-avatar size="biggest" ref="${this.item.dbref}"></nd-avatar>
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
    >${action}</nd-button>`)).join("\n");

    this.innerHTML = `
    <div class="margin-right-small popper">
      <div class="horizontal-small flex-wrap icec">${actionsEl}</div>
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
    <div class="vertical-0 flex-grow overflow icec">${contentsEl}</div>
    <nd-content-actions class="hidden"></nd-content-actions>
    `;
    this.actions = this.querySelector("nd-content-actions");
    this.active = null;
  }
}

customElements.define('nd-contents', Contents);

function keyUpHandler(e) {
  console.log("keyUpHandler", e.keyCode, e, ndc.term.focused);
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
        termSub.value.focus();
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
  "help": document.getElementById("help"),
  "equipment": document.getElementById("equipment"),
  "stats": document.getElementById("stats"),
};

const modalParent = modals.help.parentElement;

let currentModal = modals.help;

globalThis.modal_open = id => {
  modalParent.classList.remove("display-none");
  currentModal.classList.add("display-none");
  currentModal = modals[id];
  currentModal.classList.remove("display-none");
};

globalThis.modal_close = () => {
  modalParent.classList.add("display-none");
};

const directions = document.getElementById("directions");
const actions = document.getElementById("actions");

globalThis.directions_show = () => {
  directions.classList.remove("display-none");
  actions.classList.add("display-none");
};

document.addEventListener("click", () => {
  directions.classList.add("display-none");
  actions.classList.remove("display-none");
});
