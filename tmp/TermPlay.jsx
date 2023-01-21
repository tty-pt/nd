import React, { useRef, useEffect } from "react";
import { hot } from "react-hot-loader/root";
import "xterm/css/xterm.css";
import mcp from "mcp"
import { Terminal } from "xterm";
import { FitAddon } from "xterm-addon-fit";

const term = new Terminal();
const fitAddon = new FitAddon();

export default hot(function App() {
	const ref = useRef(null);

	useEffect(() => {
		term.loadAddon(fitAddon);
		term.open(ref.current);
		console.log(fitAddon.proposeDimensions());
		fitAddon.fit();
		term.write("\x1B[8;30;120t"
			+ "Hello from \x1B[1;3;31mxterm.js\x1B[0m $ ");
	}, []);

	return <div ref={ref}></div>;
});
