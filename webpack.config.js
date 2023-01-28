const makeConfig = require("@tty-pt/scripts/webpack.config");
const webpack = require("webpack");

module.exports = function (env) {
	const CONFIG_BASEDIR = env.development ? "." : "/neverdark";
	const CONFIG_PROTO = env.development ? "ws" : "wss";

	const config = makeConfig({
		...env,
		CONFIG_PROTO,
		CONFIG_BASEDIR,
		srcdir: "js",
	});

	config.plugins.push(
		new webpack.DefinePlugin({
			"process.env": JSON.stringify({
				CONFIG_PROTO,
				CONFIG_BASEDIR,
				development: env.development,
			}),
		})
	);

	return config;
};
