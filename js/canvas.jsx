let canvas = document.querySelector("canvas");
let ctx = canvas.getContext('2d');
let tilemap = document.createElement("img");
tilemap.src = process.env.CONFIG_BASEDIR + "/art/tilemap.png";
tilemap.classList.add("dn");

let actions_tiles = [];

function tilemap_cache() {
	let p = 0;
	for (let i = 0; i < 6; i++) {
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
