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
const ACT_TALK = 8;
const ACT_PUT = 9;
const ACT_EQUIP = 10;
const ACT_DROP = 11;
const ACT_EAT = 12;
const ACT_13 = 13;
const ACT_INVENTORY = 14;

let actions_lbl = [
	"look", "kill", "shop",
	"drink", "open", "chop",
	"fill", "get", "talk",
        "put", "equip", "drop",
        "eat", "reserved", "inventory",
];

let actions_tiles = [];

function tilemap_cache() {
	let p = 0;
	for (let i = 0; i < 5; i++) {
		for (let j = 0; j < 3; j++, p++) {
			ctx.clearRect(0, 0, 16, 16);
			ctx.drawImage(tilemap, 16 * j, 16 * i,
				16, 16, 0, 0, 16, 16);

                        actions_tiles.push(canvas.toDataURL());
		}
	}

        console.log(actions_tiles);
}

tilemap.onload = function () {
	setTimeout(tilemap_cache, 0);
};

document.body.appendChild(tilemap);
