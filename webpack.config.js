const makeConfig = require("@tty-pt/scripts/webpack.config");

module.exports = function (env) {
	const config = makeConfig({
		srcdir: "js",
	});

	return config;
};
