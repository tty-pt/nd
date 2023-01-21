const makeConfig = require("@tty-pt/scripts/webpack.config");
const webpack = require("webpack");

module.exports = function (env) {
	const config = makeConfig({
    ...env,
    CONFIG_PROTO: env.development ? "ws" : "wss",
		srcdir: "js",
	});

  config.plugins.push(
    new webpack.DefinePlugin({
      "process.env": JSON.stringify({
        CONFIG_BASEDIR: env.CONFIG_BASEDIR,
        CONFIG_PROTO: env.development ? "ws" : "wss",
        development: env.development,
      }),
    })
  );

  return config;
};
