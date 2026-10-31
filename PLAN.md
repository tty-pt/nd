# axil-nd port — implementation plan (2026-09-25, updated)

Scope (user-confirmed): **persistence blocker (libcorm fixes) → prove →
gap 6 BCP assertions → docs/quests**, then stop for review. No commits
unless asked. Source of truth: `~/nd/ND_PORT.md` (doc wins on conflict).
Status: all three phases DONE — `./test` 10/10 (5 consecutive runs stable),
gates green (107 `*_adapter` / 49 `eng_*` / 20 `_sic_adapter_call`; zero
`qdb_|qmap_|ndc_` in live TUs; `pgrep -x axil` clean). Nothing committed.

## Phase 1 — Persistence blocker — STATE: FIXED + VERIFIED. DONE.

Three corm bugs fixed in `~/site/external/libcorm/src/libcorm.c`:
1. **Guard** (persistence survives shutdown): `corm_save_file` early-returns
   when `file->size == 0` (no dirty handles) — never `ftruncate(0)` a store.
2. **Skip-arithmetic**: `_corm_load` mismatch branch now
   `mm + size - sizeof(uint32_t) - sizeof(size_t) - mm_start` (stored block
   size includes the 4+8+4 header; old `+ sizeof(uint32_t)` overshot +16/block).
3. **Multi-table load**: `corm_load_file(char *filename, uint32_t dbid,
   uint32_t hd)` rewritten as a block-scan — walk stored sizes, load any
   block whose `lid == dbid` into `hd`. Old `file->ids`-paired walk was
   order-dependent and either lost data or segfaulted on fresh reload of a
   multi-table store (nd boot2 crash: world.c:366 "action").
   Forward decl (libcorm.c:690) + call site (libcorm.c:946) updated.

VERIFIED (do NOT re-verify):
- corm `make test` GREEN (exit 0) after every change; 4-table single-file
  round-trip (`/tmp/cormt4.c`) reloads st/action/player/contents correctly.
- Engine two-boot probe (fixed LOCAL lib via LD_LIBRARY_PATH): boot1
  `connect alice`+`save`+`kill -SEGV` → store 892B; boot2 same store →
  login WARN `'alice' (6/1)`, ZERO `eng_object_add`, no crash, store 1362B.
  Persistence is proven; nothing left to prove.

ONLY user action remaining: one-shot system libcorm install for clean
non-dev runs/`./test` (sudo needs a tty): `cd ~/site/external/libcorm &&
make && sudo make install PREFIX=/`.

## Phase 2 — Gap 6: BCP frame assertions + persistence case in test.sh — DONE.
`./test` now 10/10, stable across 5 consecutive runs. Landed in test.sh:
- BCP frame greps on the **WS** capture (BCP is WS-only: `eng_nd_wwrite`,
  io.c:200, targets `DF_WEBSOCKET` fds; raw stays plain text): AUTH_SUCCESS
  (`236206`), VIEW_BUFFER (`236202`), ACTIONS (`236209`), TOD (`236208`).
  VIEW (iden 1) never fires at connect; there is NO `BCP_HP` iden (enum
  matches original nd; no bar fires at connect) — neither is asserted.
- Persistence regression case: two boots on one store (`$((port+1))` /
  `$((port+2))`, own logs for stderr greps). Boot A `connect alice`+`save`+
  `kill -SEGV` → store > 0 and `eng_object_add` in log. Boot B same store:
  `nd_player_login: 'alice'` with real ref, NO `eng_object_add`, NO
  `/4294967295` sentinel. Both pids in the EXIT trap. No spin/fixed-sleep
  beyond a 0.3s save settle.

## Phase 3 — Docs & quests (in progress)
- `axilnd-data-defaults.md`: updated to "FIXED + VERIFIED" (two-boot probe
  passes, do not re-verify). Remaining work tracked there: user one-shot
  system libcorm install; test coverage (persistence case + gap 6) LANDED in
  test.sh, `./test` 10/10.
- `ND_PORT.md`: Persistence audit → "FIXED + VERIFIED" block with the 3
  changes + measured probe numbers, deployment-only reinstall note, and
  corrected gap-6 frame list (no BCP_HP). Test registry item 6 updated:
  BCP + persistence cases DONE (WS-only frames, no VIEW/HP at connect).
- `~/nd/PLAN.md`: this file.
- No commits.

## Files touched (this session)
- `~/site/external/libcorm/src/libcorm.c` (3 fixes — applied)
- `/home/quirinpa/site/.pi/quest/future/axilnd-data-defaults.md` (docs)
- `~/nd/ND_PORT.md`, `~/nd/PLAN.md` (docs)
- `~/axil-nd/test.sh` (BCP + persistence regression cases — applied,
  `./test` 10/10)
- pending: system `/lib/libcorm.so` reinstall (user)
- helpers left in /tmp: `/tmp/cormt4.c` (and `/tmp/cormtest2.c`, compiled
  bins) — reusable round-trip probes.