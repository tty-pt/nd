#include "mcp.js"
#include "tty.js"
#include "canvas.js"
#include "vendor.js"

#define CONFIG_PROTO "wss"

class Modal extends React.Component {
	constructor(props) {
		super(props);
		// this.ref = React.createRef();
		this.el = props.el;
	}

	componentDidMount() {
		this.el.onclick = e => {
			if (e.target == this.el)
				this.props.setOpen(false);
		};
		this.el.className = 'modal abs sfv vn c fcc oh f';
		// this.el.classList.remove('dn');
	}

	componentWillUnmount() {
		this.el.classList.add('dn');
	}

	onKeyDown(e) {
		if (e.keyCode == 27) // escape
			this.props.setOpen(false);
	}

	render() {
		return ReactDOM.createPortal(
			(<span
				onKeyDown={e => this.onKeyDown(e)}
				onClick={e => e.stopPropagation()}
                                className="r c0 p oa"
			>
				{ this.props.children }
                        </span>),
			this.el,
		);
	}
}

const modal = document.querySelector('.modal');

function _useModal(el, Component, props = {}) {
	const [ isOpen, setOpen ] = useState(false);

	const modal = isOpen ? (
		<Modal setOpen={setOpen} el={el}>
			<Component
				{ ...props }
				close={() => setOpen(false)}
			/>
		</Modal>
	) : null;

	return [ modal, isOpen, setOpen ];
}

const useModal = _useModal.bind(null, modal);

// window.onorientationchange = scroll_reset;

mcp_init();

const atiles = [
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQklEQVQ4jWNgGL7g4OFb/9ExWZpwYZyaiXEBhiH4NBNSA+fg8wpWW5H1ETIAl98xvEHIEJyacfmdEB8vICsdjFAAAGW58imbroFwAAAAAElFTkSuQmCC",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jWNgoAY4ePjW/4OHb/0nWw+MQ6whWNUTawhedYQMIcoSXIpI8ia6YlLDCMMQcmKJdGdTxQW4/IyNT7KziXYJMemAoBewOROfHG5BAi4lRT1OAAA/Xu7MVtQQRgAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAOUlEQVQ4jWNgGJTg4OFb/3FhkjXjEiNaMzFyWBXB+Li8RHsDKPYCIZfQxwCSvYBLIcWJiSTNQw8AAO8uLsItXTaGAAAAAElFTkSuQmCC",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgIBIcPHzrPzImWREyn2wDyHIBuhhJBhDUgM8QdDZJmim2nWwDcBlCkmZ0Q8iynSRN2AwYGP9T3QAYnyQDyAEAd7gQfVonw9EAAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAVElEQVQ4jaXRywkAMAgDUPffStzCZey1FE3UCt7Cw48IKTUPNQ+Wo8AauYEV8gIQycJVQ6ALwwlWq7DAGEBIegO07+iV3wBDWgBC2kCFjIAMGQOoDj9EP5M9YZdfAAAAAElFTkSuQmCC",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGLTg4OFb/5ExyRoGjwHIcmQZgi5HsiHY5EgyBJccQUOIsWBwGEKRAcMEAABAgauIKxMDjQAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAUElEQVQ4jWNgwAEOHr71H5cc0QBmCEWGUcUlZNuMjskyBJmmnwHYnEySNw4evvX/////TLhcRFAzJfLUMYAQxmsAIUMIaqaKAdgMIkkjKQAABfenzXQV7xsAAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcwWQYQkiPJFQQNgClGp8lOraMANwAA27bDE5yTd30AAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAM0lEQVQ4jWNgGFTg4OFb/4nFOA0g1iK8EoRsHhgXkGQAxS6gyACKo5FYV1DkBeprHtoAAOijqCv2+qTkAAAAAElFTkSuQmCC",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAWElEQVQ4jc3QwQ0AMAQFUPtvJbawjF4qaUVQcair/18EwB4kFiQW6IyW24gF/kKewQwpQWNABJcBWxq7IgU03C5HUH2ZZJBYwrPOx1no6mWI3bv5yiVebgFV9xc3cxaEggAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAALElEQVQ4jWNgGF7g4OFb/5ExsXI4FeHDRLuCJM3EGEJWWIxEA3AZQrTmEQYAvLnktS1HPJUAAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNggIKDh2/9P3j41n8GcgBMM9mGoBswuAwh2UBChhBlENUMwGcw0Qaga6KaKwgaAFNMtmZ8BtHPAOSAI9sgipI2ukuwAQABHugSAyRIzQAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAQUlEQVQ4jWNgoDY4ePjWf7LkYRLoNLoasg2AaR6iBiBrpq0BuBQOEQPQFeIzHKcBMMWEXEeyAfhcR7Iisg3AJQcA7LE1fLFqxRoAAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAR0lEQVQ4jWNgoCU4ePjWfximSDNZhlDFAGw02a4gWTNJtuOzBZ84hhwxziXKS9gUURyVJGlEt5EkQwgFKE6DSLGForRBdQAAjPakzh7dkTsAAAAASUVORK5CYII=",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAANElEQVQ4jWNgGFTg4OFb/4nBBA0hRw7FBWTJE+U8fOqI0YxX7agBVDKAomhElqQoKdMdAADVVJq360zbaQAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAASUlEQVQ4jWNgoBU4ePjWf2RMsSFUcwFRLkNXgG4ANjbRzsZrAC7N+LxDlM0EXUOKs3GKE+tsYgwmLEkzA0hNvlgNIKSJ9gaQigF3uf+dQNoz9QAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAATElEQVQ4jWNgoDU4ePjWf4o0wzDVNCOL4zQYnwJkMawGEDIdqwEwBqnORjGAWGdjFSfF2QQNwOYdUg0mLEkzA4hKJIQMIKSJ9gaQigFrg/+dXV9AmAAAAABJRU5ErkJggg==",
        "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/9hAAAAEklEQVQ4jWNgGAWjYBSMAggAAAQQAAF/TXiOAAAAAElFTkSuQmCC"
];

const GameContext = React.createContext({});

function useSession(onOpen, onMessage, onClose) {
        const [ session, setSession ] = useState(null);

        const connect = useCallback(() => {
                const ws = new WebSocket(CONFIG_PROTO + "://" + window.location.hostname + ':4201', 'text');
                ws.binaryType = 'arraybuffer';
                setSession(ws);
        }, []);

        const updateOpenHandler = () => {
                if (!session) return;
                session.addEventListener('open', onOpen);
                return () => {
                        session.removeEventListener('open', onOpen);
                };
        };

        const updateMessageHandler = () => {
                if (!session) return;
                session.addEventListener('message', onMessage);
                return () => {
                        session.removeEventListener('message', onMessage);
                };
        };

        const updateCloseHandler = () => {
                if (!session) return;
                session.addEventListener('close', onClose);
                return () => {
                        session.removeEventListener('close', onClose);
                };
        };

        useEffect(connect, []);
        useEffect(updateOpenHandler, [session, onOpen]);
        useEffect(updateMessageHandler, [session, onMessage]);
        useEffect(updateCloseHandler, [session, onClose]);

        function sendMessage(text) {
                // console.log("sendMessage", text);
                session.send(text + "\n");
        }

        return [connect, sendMessage, session];
}

function webLookReducer(state, action) {
        const dbref = parseInt(action.dbref);
        const { room } = action;

        const ret = {
                ...state,
                ...(room ? {
                        here: dbref,
                        target: null,
                } : {
                        target: dbref,
                }),
                objects: {
                        ...state.objects,
                        [dbref]: {
                                ...action,
                                contents: {},
                        },
                },
                terminal: state.terminal + (action.description ? "\nYou see: " + action.description : ""),
        };

        // console.log("WEB-LOOK", state, action, ret);

        return ret;
}

function webLookContentReducer(state, action) {
        const dbref = parseInt(action.dbref);
        const loc = parseInt(action.loc);
        return {
                ...state,
                objects: {
                        ...state.objects,
                        [loc]: {
                                ...state.objects[loc],
                                contents: {
                                        ...state.objects[loc].contents,
                                        [dbref]: {
                                                ...action,
                                                icon: tty_proc(action.icon),
                                                pname: tty_proc(action.pname),
                                        },
                                },
                        },
                }
        };
}

function webInReducer(state, action) {
        const dbref = parseInt(action.dbref);
        const loc = parseInt(action.loc);

        if (!state.objects[loc]) {
                console.warn("web-in: actionable of loc", loc, "is not available");
                return state;
        }

        return {
                ...state,
                objects: {
                        ...state.objects,
                        [loc]: {
                                ...state.objects[loc],
                                contents: {
                                        ...state.objects[loc].contents,
                                        [dbref]: {
                                                ...action,
                                                icon: tty_proc(action.icon),
                                                pname: tty_proc(action.pname),
                                        },
                                }
                        }
                }
        };
}

function gameReducer(state, action) {
        // console.log(action, state);

        switch (action.key) {
                case 'inband':
                        if (action.data != "\n\r")
                                return {
                                        ...state,
                                        terminal: state.terminal + tty_proc(action.data),
                                }
                        break;

                case 'web-view':
                        return {
                                ...state,
                                view: tty_proc(action.data),
                        }

                case 'web-look':
                        return webLookReducer(state, action);

                case 'web-look-content':
                        return webLookContentReducer(state, action);

                case 'web-in':
                        return webInReducer(state, action);

                case 'web-out':
                        const dbref = parseInt(action.dbref);
                        const loc = parseInt(action.loc);

                        if (!state.objects[loc])
                                return state;

                        let newContents = { ...state.objects[loc].contents };
                        delete newContents[dbref];

                        return {
                                ...state,
                                objects: {
                                        ...state.objects,
                                        [loc]: {
                                                ...state.objects[loc],
                                                contents: newContents
                                        }
                                },
                                dialog: !state.dialog || dbref == parseInt(state.dialog.npc)
                                        ? null : state.dialog,
                        };

                case 'web-auth-success':
                        return {
                                ...state,
                                me: parseInt(action.player),
                                authFail: null,
                        };

                case 'web-auth-fail':
                        return {
                                ...state,
                                me: parseInt(action.player),
                                authFail: true,
                        };


                case 'web-stats':
                        return {
                                ...state,
                                stats: action,
                        };

                case 'web-bars':
                        return {
                                ...state,
                                bars: action,
                        };

                case 'web-dialog-start':
                        return {
                                ...state,
                                dialog: {
                                        ...action,
                                        answers: [],
                                },
                        };

                case 'web-dialog-answer':
                        return {
                                ...state,
                                dialog: {
                                        ...state.dialog,
                                        answers: state.dialog.answers.concat([action]),
                                },
                        };

                case 'web-dialog-stop':
                        return {
                                ...state,
                                dialog: null,
                        };

                case 'web-equipment':
                        return {
                                ...state,
                                equipment: {},
                        };

                case 'web-equipment-item':
                        return {
                                ...state,
                                equipment: {
                                        ...(state.equipment || {}),
                                        [parseInt(action.eql)]: {
                                                ...action,
                                                pname: tty_proc(action.pname),
                                                icon: tty_proc(action.icon),
                                        },
                                },
                        };
        }

        return state;
}

function GameContextProvider(props) {
        const { children } = props;

        const [state, dispatch] = useReducer(
                gameReducer,
                {
                        terminal: "",
                        objects: {},
                        target: null,
                        here: null,
                }
        );

        function onMessage(ev) {
                const mcp_arr = mcp_proc(ev.data);
                for (let i = 0; i < mcp_arr.length; i++) {
                        // console.log(mcp_arr[i]);
                        dispatch(mcp_arr[i]);
                }
        }

        function output(text) {
                dispatch({
                        key: "inband", 
                        data: "\n" + text,
                });
        }

        function onOpen() {
                output("socket connection open");
        }

        function onClose() {
                output("socket connection closed");
        }

        const [ connect, sendMessage, session ] = useSession(onOpen, onMessage, onClose);

        return <GameContext.Provider
                value={{
                        ...state, 
                        session,
                        sendMessage,
                        dispatch,
                }}
        >
                { children }
        </GameContext.Provider>
        
}

function Terminal() {
        const { terminal } = useContext(GameContext);
        const ref = useRef(null);

        useEffect(() => {
                ref.current.scrollTop = ref.current.scrollHeight;
        }, [terminal]);

        // console.log(context);
        return (<pre id="term" ref={ref} className="fg oa"
                dangerouslySetInnerHTML={{ __html: terminal }}>
        </pre>)
}

function RoomTitleAndArt() {
        const { here, objects } = useContext(GameContext);
        const obj = objects[here];

        if (!obj)
                return null;

        const src = "art/" + (obj.art || "unknown.jpg");

        return (<div className="vn fg f fic">
                <div className="tm pxs tac">{ obj.name }</div>
                <img className="sr2" src={src} />
        </div>);
}

function Tabs(props) {
        const { children } = props;
        const [ activeTab, setActiveTab ] = useState(0);

        return (<div className="fg vn">
                <div className="_n">
                        { children.map((child, idx) => {
                                const { label } = child.props;
                                return (<a key={idx}
                                        onClick={() => setActiveTab(idx)}
                                        className={activeTab == idx ? 'ps c0' : 'ps'}
                                >
                                        {label}
                                </a>);
                        }) }
                </div>
                { children.map((child, idx) => {
                        // const { label } = child.props;
                        if (idx != activeTab)
                                return null;

                        return child;
                }) }
        </div>);
}

function Stat(props) {
        const { label, value } = props;

        return (<div className="_s">
                <div className="tb">{label}</div><div>{value}</div>
        </div>);
}

const EQL_HEAD = 1;
const EQL_NECK = 2;
const EQL_CHEST = 3;
const EQL_BACK = 4;
const EQL_RHAND = 5;
const EQL_LFINGER = 6;
const EQL_RFINGER = 7;
const EQL_PANTS = 8;

const eql_label = {
        [EQL_HEAD]: "hand",
        [EQL_NECK]: "neck",
        [EQL_CHEST]: "chest",
        [EQL_BACK]: "back",
        [EQL_RHAND]: "weapon",
        [EQL_LFINGER]: "lfinger",
        [EQL_RFINGER]: "rfinger",
        [EQL_PANTS]: "grieves",
};

function Equipment(props) {
        const { eql } = props;
        const { equipment, sendMessage } = useContext(GameContext);

        if (!equipment)
                return null;

        if (!equipment[eql])
                return <div className="sl c0"></div>;

        return (<Avatar
                item={equipment[eql]}
                size="l"
                onClick={() => sendMessage("unequip " + eql_label[eql])}
        />);
}

function PlayerTabs() {
        const { me, stats } = useContext(GameContext);

        if (!me)
                return null;

        return (<Tabs>
                <div label="stats" className="ps _s f">
                        <div className="vs fg">
                                <Stat label="str" value={stats.str} />
                                <Stat label="con" value={stats.con} />
                                <Stat label="dex" value={stats.dex} />
                                <Stat label="int" value={stats.int} />
                                <Stat label="wiz" value={stats.wiz} />
                        </div>
                        <div className="vs fg">
                                <Stat label="dodge" value={stats.dodge} />
                                <Stat label="dmg" value={stats.dmg} />
                                <Stat label="mdmg" value={stats.mdmg} />
                                <Stat label="def" value={stats.def} />
                                <Stat label="mdef" value={stats.mdef} />
                        </div>
                </div>
                <div label="equipment" className="ps vs">
                        <div className="_s f">
                                <div className="sl"></div>
                                <div className="sl"></div>
                                <Equipment eql={EQL_HEAD} />
                                <div className="sl"></div>
                                <div className="sl"></div>
                        </div>
                        <div className="_s f">
                                <Equipment eql={EQL_RHAND} />
                                <div className="sl"></div>
                                <Equipment eql={EQL_NECK} />
                                <Equipment eql={EQL_BACK} />
                                <div className="sl"></div>
                        </div>
                        <div className="_s f">
                                <Equipment eql={EQL_RFINGER} />
                                <div className="sl"></div>
                                <Equipment eql={EQL_CHEST} />
                                <div className="sl"></div>
                                <Equipment eql={EQL_LFINGER} />
                        </div>
                        <div className="_s f">
                                <div className="sl"></div>
                                <div className="sl"></div>
                                <Equipment eql={EQL_PANTS} />
                                <div className="sl"></div>
                                <div className="sl"></div>
                        </div>
                </div>
        </Tabs>);
}

function MiniMap(props) {
        const { view, target } = useContext(GameContext);

        if (target)
                return null;

        return <div className="fac"><pre id="map" dangerouslySetInnerHTML={{ __html: view }}></pre></div>;
}

function TargetTitleAndArt() {
        const { target, objects } = useContext(GameContext);

        if (!target)
                return null;

        const obj = objects[target];

        const src = "art/" + (obj.art || "unknown_small.jpg");

        return (<>
                <div className="tm pxs tac">{obj.name}</div>
                <img className="sr1 fac" src={src} />
        </>);
}

function Avatar(props) {
        const { item, size = "xl", ...rest } = props;

        if (!item)
                return null;

        if (item.avatar)
                return <img
                        className={"s_" + size + " sv" + size}
                        src={"art/" + item.avatar}
                        { ...rest }
                />;
        else
                return <span
                        className={"s" + size + " t" + size + " tcv"}
                        dangerouslySetInnerHTML={{ __html: item.icon }}
                        { ...rest }
                />;
}

function ContentsItem(props) {
        const { item, onClick, activeItem, isShop } = props;
        const className = "f fic pxs _s " + (activeItem == item.dbref ? 'c0' : "");

        return (<a className={className} onClick={onClick}>
                <Avatar item={item} />
                <span dangerouslySetInnerHTML={{
                        __html: item.pname + (isShop ? " " + item.price + "P" : ""),
                }}></span>
        </a>);
}

function Contents(props) {
        const { onItemClick, activeItem } = props;
        const { target, here, objects } = useContext(GameContext);

        const obj = target ? objects[target] : objects[here];

        if (!here)
                return null;

        const contentsEl = Object.keys(obj.contents).map(k => {
                const item = obj.contents[k];

                return <ContentsItem key={item.dbref} item={item}
                        isShop={obj.shop}
                        activeItem={activeItem}
                        onClick={e => onItemClick(e, item)} />;
        });

        return (<div className="vn fg oa icec">
                { contentsEl }
        </div>);
}

function RB(props) {
        const { children, onClick } = props;
        return <a className="round txl ps c0" onClick={onClick}>{ children }</a>;
}

function RBT(props) {
        return <a className="round txl ps"></a>;
}

function RBI(props) {
        const { onClick, src } = props;
        return (<a className="round ps c0" onClick={onClick}>
                <img className="svl s_l" src={atiles[src]} />
        </a>);
}

function Directions() {
        const { sendMessage } = useContext(GameContext);

        return (<div className="vn tar tnow abs al">
                <div className="_n">
                        <RBT />
                        <RB onClick={() => sendMessage("k")}>&uarr;</RB>
                        <RBI onClick={() => sendMessage("K")} src={ACT_K} />
                </div>
                <div className="_n">
                        <RB onClick={() => sendMessage("h")}>&larr;</RB>
                        <RBT />
                        <RB onClick={() => sendMessage("l")}>&rarr;</RB>
                </div>
                <div className="_n">
                        <RBT />
                        <RB onClick={() => sendMessage("j")}>&darr;</RB>
                        <RBI onClick={() => sendMessage("J")} src={ACT_J} />
                </div>
        </div>);
}

function ContentsAndActions(props) {
        const { sendMessage, here, me, target, objects } = useContext(GameContext);
        const [ actions, setActions ] = useState([]);
        const [ activeItem, setActiveItem ] = useState(null);

        function onItemClick(ev, item) {
                let newActions = [];

                if (item.loc == here) {
                        for (let p = 0; p < 9; p++) {
                                if (!(parseInt(item.actions) & (1 << p)))
                                        continue;

                                let id = actions_lbl[p];
                                newActions.push([p, function () {
                                        sendMessage(id + " #" + item.dbref);
                                }]);
                        }
                } else if (item.loc == me) {
                        // action_add(ACT_PUT, function () {
                        //         output("\nPUT is not implemented yet!");
                        // });

                        newActions.push([ACT_EQUIP, function () {
                                sendMessage("equip #" + item.dbref);
                        }]);

                        newActions.push([ACT_DROP, function () {
                                sendMessage("drop #" + item.dbref);
                        }]);

                        newActions.push([ACT_EAT, function () {
                                sendMessage("eat #" + item.dbref);
                        }]);

                        newActions.push([ACT_SHOP, function () {
                                sendMessage("sell #" + item.dbref);
                        }]);

                } else if (objects[item.loc].shop) {
                        newActions.push([ACT_SHOP, function () {
                                sendMessage("buy #" + item.dbref);
                        }]);
                } else {
                        newActions.push([ACT_GET, function () {
                                sendMessage("get #" + target + "=#" + item.dbref);
                        }]);
                }

                setActions(newActions);
                setActiveItem(item.dbref);
        }

        const actionsEl = actions.map(([p, cb]) => (
                <RBI key={p} onClick={cb} src={p} />
        ));

        return (<>
                <Contents onItemClick={onItemClick} activeItem={activeItem} />

                <div className="_n icec">
                        { actionsEl }
                </div>
        </>);
}

function Help() {
        return (<>
                <b>Help</b><br />
                <br />
                Hopefully on a mobile phone you will be able to use<br />
                the interface to interact with the game. But here are<br />
                the keyboard commands for the game:<br />
                <br />
                <b>Normal mode:</b><br />
                <p>
                        <b>s</b> To chat.<br />
                        <b>a</b> to send commands.<br />
                        <b>i</b> to access your inventory.<br />
                        <b>Left</b> or <b>h</b> to move west.<br />
                        <b>Right</b> or <b>l</b> to move east.<br />
                        <b>Up</b> or <b>k</b> to move north.<br />
                        <b>Down</b> or <b>j</b> to move south.<br />
                        <b>Shift+Up</b> or <b>K</b> to move up.<br />
                        <b>Shift+Down</b> or <b>J</b> to move down.<br />
                </p>
                <b>Input mode:</b><br />
                <p>
                        <b>Esc</b> to go back to normal mode.<br />
                        <b>Up</b> to travel up in history.<br />
                        <b>Down</b> to travel down in history.<br />
                        <b>Ctrl+u</b> to delete input text.<br />
                </p>
                this game is very early stage, and it still requires a<br />
                                lot of work. but i might not be able to provide it full-time.<br />
                <br />
                You can use it freely to chat with your friends.<br />
                Meme at will.<br />
                <br />
                You can check out the code <a href="https://github.com/tty-pt/neverdark">here</a>.<br />
                <br />
                Hopefully the users will be able to create game content in the future.<br />
                Meanwhile i will be working at the engine for that to be possible.<br />
                <br />
                The help command is very useful. Try issuing "help help".<br />

                                <p><b>help startingout</b> will get you further.</p>
        </>);
}

function Bar(props) {
        const { max, value, color } = props;
        const style = {
                width: (100 * value / max) + "%",
        };

        return (<div className="fg svs b0">
                <div className={"svf c" + color} style={style}></div>
        </div>);
}

function PlayerBars() {
        const { bars } = useContext(GameContext);

        if (!bars)
                return null;

        const { hp, hpMax, mp, mpMax } = bars;

        return (<div className="_s f ps">
                <Bar value={hp} max={hpMax} color="1" />
                <Bar value={mp} max={mpMax} color="12" />
        </div>);
}

function Dialog() {
        const { dialog, here, objects, sendMessage, dispatch } = useContext(GameContext);

        if (!dialog)
                return null;

        const item = objects[here].contents[dialog.npc];

        const { text } = dialog;

        let answersEl = dialog.answers.map(answer => (
                <a key={answer.id} className="ps c0 cf15" onClick={() => sendMessage("answer " + answer.id)}>
                        { answer.text }
                </a>

        ));

        if (!answersEl.length) {
                function endDialog() {
                        dispatch({
                                key: "web-dialog-stop",
                        });
                }

                answersEl = (<a key="endC"
                        className="ps c0 cf15" onClick={endDialog}>
                        End conversation
                </a>);
        }

        return (<>
                <div className="ps _s f c15 cf0">
                        <Avatar item={item} />
                        <div className="fg">
                                { text }
                        </div>
                </div>
                <div className="_ ps">
                        { answersEl }
                </div>
        </>);

}

function Game() {
        const { sendMessage, session } = useContext(GameContext);
	const [ modal, isOpen, setOpen ] = useModal(Help, {});
        const input = useRef(null);

        function keyDownHandler(e) {
                if (document.activeElement == input.current) {
                        switch (e.keyCode) {
                                case 27:
                                        input.current.blur();
                                        break;
                                case 85: // u
                                        if (e.ctrlKey)
                                                input.current.value = "";
                                        break;
                        }
                        return;
                }

                switch (e.keyCode) {
                        case 83: // s
				if (e.shiftKey)
					sendMessage("sit");
				else {
					input.current.value = "say ";
					input.current.focus();
					e.preventDefault();
				}
				break;
                        case 65: // a
                                input.current.focus();
                                e.preventDefault();
                                break;
                        case 75: // k
                        case 38: // ArrowUp
                                if (e.shiftKey)
                                        sendMessage("K");
                                else
                                        sendMessage("k");
                                break;
                        case 74: // j
                        case 40: // ArrowDown
                                if (e.shiftKey)
                                        sendMessage("J");
                                else
                                        sendMessage("j");
                                break;
                        case 72: // h
                        case 37: // ArrowLeft
                                sendMessage("h");
                                break;
                        case 76: // l
                        case 39: // ArrowRight
                                sendMessage("l");
                                break;
                        case 73: // i
                                sendMessage("inventory");
                                break;
                        case 79: // o
                                sendMessage("look");
                                break;
			default:
				console.log(e);
                }
        }
        useEffect(() => {
                window.addEventListener('keydown', keyDownHandler);
                return () => window.removeEventListener('keydown', keyDownHandler);
        }, [session]);

        function toggle_help() {
                setOpen(!isOpen);
        }

        function onSubmit(e) {
                e.preventDefault();
                const fd = new FormData(e.target);
                sendMessage(fd.get("cmd"));
                input.current.value = "";
                input.current.blur();
        }

        return (<>
                <span className="vn f">
                        {/* <RB onClick={disconnect}>X</RB> */}
                        <RB onClick={toggle_help}>?</RB>
                        <RBI onClick={() => sendMessage('inventory')} src={ACT_OPEN} />
                        <RBI onClick={() => sendMessage('look')} src={ACT_LOOK} />
                </span>

                <form className="vn f fg s_f oa" onSubmit={onSubmit}>
                        <div className="_n f">
                                <PlayerTabs />
                                <RoomTitleAndArt />
                        </div>

                        <PlayerBars />

                        <Terminal />

                        <Dialog />

                        <input ref={input} name="cmd" className="cf"
                                autoComplete="off" autoCapitalize="off" />
                </form>

                <div className="vn f fg s_33">
                        <MiniMap />
                        <TargetTitleAndArt />
                        <Directions />
                        <ContentsAndActions />
                </div>

                { modal }
        </>);
}

const { BrowserRouter, Switch, Route, Redirect, useHistory } = ReactRouterDOM;

function Login() {
        const { sendMessage, me, authFail } = useContext(GameContext);

        function onSubmit(e) {
                const fd = new FormData(e.target);
                sendMessage("auth " + fd.get("user") + "=" + fd.get("pass"));
                e.preventDefault();
        }

        if (me)
                return <Redirect to="/" />;

        return (<div className="vn f sf fic fcc">
                <h1>Login</h1>
                <form className="vs" onSubmit={onSubmit}>
                        <label>
                                Username
                                <input name="user" />
                        </label>
                        <label>
                                Password
                                <input name="pass" type="password" />
                        </label>
                        <input type="submit" value="login" />
                </form>
                { authFail ? <div className="cf1">Invalid authentication</div> : null }
        </div>);
}

function AppRouter() {
        const { me } = useContext(GameContext);

        return (<BrowserRouter basename="/neverdark">
                <Switch>
                        <Route path="/login" component={Login} />
                        <Route path="/">
                                { me ? <Game /> : <Redirect to="/login" /> }
                        </Route>
                </Switch>
        </BrowserRouter>);
}

function App() {
        return (<GameContextProvider>
                <AppRouter />
        </GameContextProvider>);
}

ReactDOM.render(<App />, document.getElementById('main'));
