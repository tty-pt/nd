import setup from "@tty-pt/scripts";
import App from "./App";

setup(App, "./App", document.getElementById("main"), () => require("./App").default);
