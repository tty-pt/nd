const ACTIONS = {
  LOOK: 0,
  KILL: 1,
  SHOP: 2,
  DRINK: 3,
  OPEN: 4,
  CHOP: 5,
  FILL: 6,
  GET: 7,
  TALK: 8,
  PUT: 9,
  EQUIP: 10,
  DROP: 11,
  EAT: 12,
  13: 13,
  INVENTORY: 14,
  K: 15,
  J: 16,
};

export
const ACTIONS_LABEL = {
  [ACTIONS.LOOK]: "look",
  [ACTIONS.KILL]: "kill",
  [ACTIONS.SHOP]: "shop",
  [ACTIONS.DRINK]: "drink",
  [ACTIONS.OPEN]: "open",
  [ACTIONS.CHOP]: "chop",
  [ACTIONS.FILL]: "fill",
  [ACTIONS.GET]: "get",
  [ACTIONS.TALK]: "talk",
  [ACTIONS.PUT]: "put",
  [ACTIONS.EQUIP]: "equip",
  [ACTIONS.DROP]: "drop",
  [ACTIONS.EAT]: "eat",
  [ACTIONS.RESERVERD]: "reserved",
  [ACTIONS.INVENTORY]: "inventory",
  [ACTIONS.K]: "K",
  [ACTIONS.J]: "J",
};

export default ACTIONS
