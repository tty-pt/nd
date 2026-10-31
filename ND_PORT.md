# ND_PORT — what is missing to complete the axil-nd port

> Successor to the removed `PORT_ND.md` (that file was the full workload /
> session log; it is intentionally gone — this file carries only what is still
> needed, plus every currently-known fact that helps finish the port).
>
> Status as of 2026-09-25: engine imported, store corm-ified, modding stack on
> libxylem, spatial map on libislet, interface split into real XY providers,
> wired into the axil loop (tick / command routing / auth / lifecycle;
> `./test` passes; commit `5a99ef9` in `~/axil-nd`), raw-telnet guest path
> (gap 1) + static client & asset delivery (gap 2) + QSESSION cleanup (gap 5)
> + gap-6 BCP + persistence regression cases in test.sh LANDED, AND the
> persistence blocker is FIXED + VERIFIED (two-boot probe passes — see the
> Persistence audit below). This file lists ONLY the remaining work,
> cross-referenced against the dependency sources
> (`~/site/external/{axil,libxylem,libislet}`, verified 2026-09-25).

## Library usage findings — verified against the sources

Most of what we flagged as "missing" is already built into axil/islet/xylem and
just not wired up yet. Library sources are the ground truth; the port should
call these APIs, not reimplement them.

### axil (`~/site/external/axil`)
- **Static file serving is built in**: `serve.allow` glob ACL → realpath guard →
  index.html default → mime map → ETag / If-Modified-Since / 304
  (`request_handle_static`, libaxil.c:2049-2112; axil-posix.c:200-317).
  Handler-level helpers: `axil_respond_file(fd,path,exts)` (libaxil.c:2900),
  `axil_sendfile(fd,path)` (axil.h:427). Route patterns: `axil_register_handler`
  supports `:param` + terminal `*` catch-alls; `axil_register_fallback_handler`
  runs after static, registered handlers, and `axil_config.default_handler`.
  **Precedence**: static runs BEFORE registered handlers (libaxil.c:2649 vs
  2665) — the WS upgrade must sit on a non-served path (e.g. `GET:/nd/ws`) or
  assets must be served inside `handle_nd`.
- **Raw accepts**: WEAK `axil_accept(fd)` fires on every accept pre-WS
  (libaxil.c:748; `DF_ACCEPTED` marks it). Raw lines already flow
  `on_axil_parse` → axil `cmd_parse` → `cmd_proc` → axil-registered nd commands.
  Returning <0 from `on_axil_parse` skips `cmd_parse` (the WS path does this).
- **Telnet**: send-side WEAK helpers `axil_send_telnet_cmd` + `TELNET_CMD` macro
  (axil.h:332-340) — already used by the module; negotiation is NOT
  reimplemented. There is NO peer-option stripper in axil (only Win32 IAC
  constants) → `strip_telnet` (libaxil-nd.c:109) is genuinely new.
- **WS delivery**: axil assembles one frame at a time (ws.c:161-228, payload
  ≤ `BUFSIZ`, unmasked in place, partial frame held in `frame_map`). Frames are
  NOT aligned to newlines → the module's `ws_lb` line buffer is still required.
  **Caveat (verified in ws.c:214)**: the mask + payload of a frame must arrive
  in a SINGLE `lower_read` — if a frame payload spans TCP segments, `ws_read`
  errors out (`bad frame mk size`). Big client payloads (large paste, long
  lines near 8 KB) will break; test the real client before shipping.
- **Identity provenance**: `axil_auth(fd, username)` sets `REMOTE_USER` +
  `DF_AUTHENTICATED` on the connection (axil-posix.c:354-366). Upstreamed fix:
  `axil_auth` now supports virtual / non-passwd users by safely falling back to
  default server credentials (`axil_pw`) and returning 0 when `getpwnam()` is NULL,
  enabling `world.c:744` to strictly enforce `if (axil_auth(fd, user)) return NOTHING;`
  without rejecting virtual game players. It is reached
  either by the platform calling the WEAK `axil_auth_check` (and then
  `axil_auth`), by a WS-phase handler calling `axil_auth` directly, or by
  `-A`/`AXIL_AUTOAUTH` (dev/test auto-auth of WS connections only). Raw
  accepts get none of these → raw guests boot via the `connect` command
  (gap 1, DONE), which calls `axil_auth(fd, name)` itself.
- **Command dispatch**: axil's `cmd_proc`/`cmd_parse` are **static/private**.
  `ws_command` (libaxil-nd.c) mirrors cmd_proc (argv[0] trim, auth gate,
  cb-or-vim, tail flush). See "Library-change candidates" below.
- **Raw-telnet guest path (gap 1, DONE)**: raw fds are detected on the FIRST
  non-HTTP line in `on_axil_parse` (`is_http_method()` in libaxil-nd.c —
  GET/POST/HEAD/PUT/OPTIONS/DELETE/PATCH/PRI + space are passed through
  untouched because the fd is not `DF_WEBSOCKET` yet during the WS-upgrade
  handshake). On raw-mark, the module emits WILL ECHO / WONT SGA / DO NAWS +
  a short banner, then lets `cmd_proc` dispatch. Pre-auth only CF_NOAUTH
  commands pass (libaxil.c:874-876), so the `connect` command boots the
  player; otherwise non-connect raw input is silently dropped. `do_connect`
  + the shared `nd_player_login` land the guest exactly like a WS user.
- **Raw fds never fire `on_axil_disconnect`**: that hook is gated on
  `DF_CONNECTED`, set only at WS upgrade (libaxil.c:2744/413). Dead raw fds
  therefore linger in `fds_hd`; v1 accepts offline-persist semantics (next
  `connect <name>` rebinds through the re-login branch). See Open audits.
- **Hooks already used correctly**: `axil_update` (tick), `axil_connect`
  (WS-only), `axil_disconnect`, `axil_command`, `axil_vim`, `axil_auth_check`,
  `axil_ws_upgrade`, `DF_AUTHENTICATED` / `CF_NOAUTH` / `CF_NOTRIM`. Extra:
  per-fd WEAK `axil_fd_tick` + `DF_EXTERN` for streaming fds (unused here).
- **axil-auth**: QSESSION session login is a separate lib/module backed by corm.
  Our hand-rolled `./sessions/<sid>` check (world.c:672) does NOT match it and
  is effectively dead — see gap 5.

### libxylem (`~/site/external/libxylem`)
- `xy-mod.h` declares ONE `static struct xy_ctx xy` + WEAK `get_xy_ptr()` per
  TU and one injection target → folding `nd_xy.c`/`nd_events.c`/`nd_api.c` into
  `libaxil-nd.c` is the sanctioned single-module-ctx pattern, NOT a workaround.
- Provided but unused here: regions (`xy_region_each`, `xy_with_region`), claim
  gate (`xy_require_claim`), `xy_deny`, per-region state
  (`XY_REGION_STATE`/`XY_REGION_INIT`/`XY_RS`). Cosmetic aliases in xy-pp.h
  (`XY_LISTENER`, `XY_HOOK_DECL`).
- The io game API (`papi/nd-xy.h` + `nd_xy.c` provider), SIC events
  (`nd_events.c`), and engine adapters (`nd_api.c`) are textbook `XY_IMPL`/
  `XY_CALL` — no gap.

### libislet (`~/site/external/libislet`)
- Box/rect query: `islet_iter_N` + `islet_next` (islet.h:270-329), box INCLUSIVE
  on the far edge — `map.c` uses it; the volume overflow warning (<1M points)
  covers our ≤7×7 boxes.
- Auto-grow + file persistence + **exit autosave via libcorm destructor**
  documented (islet.h:182-192) → no per-tick `corm_save()` needed; explicit
  `corm_save()` on `do_save`/`close_all(0)` suffices.
- Reverse index thing→pos is NOT in islet → ND's corm `map_w` (u32→pos) is
  correct to keep.
- `morton_set/get_1..4_2_32` (morton.h) exist for encoding; nd's morton was
  dropped per user directive (raw `pos_t` bytes as the u64 key).
- Seam-splitter `map_range_safe` is genuine ND geometry (iters are inclusive;
  the Z-order seam at int16 COORD_MAX must be split caller-side). Keep.

### Store layout — verified against the original `~/nd/src/interface.c`
- Original: `qdb_config.file = STD_DB` (interface.c:658) = ONE file holding
  ALL persistent tables (`qdb_open`): st, sl, action, ndt, entity, player,
  contents, obs, skel, drop, adrop, element, wts, awts, biome, module_id.
  In-memory `qmap_open`: fds, dplayer, vtf, situc, sica, sican, bcp, hd, obj,
  mod. → "same database for a lot of different things".
- Port maps this 1:1: `corm_open(world_db(), NAME, …)` for every qdb table and
  `corm_open(NULL, NAME, …)` for every qmap table (world.c:356-391, io.c:39-40).
  corm.h:217-219 explicitly supports many logical DBs in one file (XXH32 id).
  The NULL-file tables are NOT a regression — they mirror the original
  in-memory tables exactly. `obj_hd` is in-memory in BOTH engines (the world
  is re-seeded each fresh boot / regenerated by `objects_init` + `mod_init`).

### Library-change candidates — only if we MUST
1. **`axil_dispatch(fd, line, len)`** (public wrapper over axil's private
   `cmd_parse`/`cmd_proc`): would delete the `ws_command` mirror and the manual
   `nd_flush` (axil's WEAK `axil_flush` already exists for the tail flush).
   Currently the module duplicates axil core logic that could drift. Only add if
   other consumers would use it; otherwise keep the mirror.
2. **`axil_strip_telnet`** (symmetric helper next to `axil_send_telnet_cmd`):
   only if other axil consumers need IAC stripping; otherwise keep in module.
Everything else is use-as-is; no islet/xylem changes indicated.

## Locked decisions (implemented — keep)

- Identity source = `REMOTE_USER` env (set by `axil_auth`); raw guests boot
  via the `connect <name>` command (gap 1, DONE). No QSESSION (gap 5, DONE).
- Raw streams ride axil's own table (`on_axil_parse` returns the STRIPPED length
  ≥0 so `cmd_parse`/`cmd_proc` dispatch the registered nd commands); WS uses the
  local `ws_command` cmd_proc-mirror and returns -1 to skip `cmd_parse`.
- `GET`/`POST`/`PRI` stay RESERVED (axil core HTTP commands) — skipped in
  `nd_register_commands` (world.c:624) and in `ws_command` (libaxil-nd.c:88).
- Telnet negotiation (WILL ECHO / WONT SGA / DO NAWS) is emitted in
  `on_axil_connect` for every connection — harmless over WS, needed for raw.
- Tick: `on_axil_update` receives ms; port to the engine's µs contract via
  `nd_update(dt * 1000)` (1 Hz tick, 2× multiplier in nd_update), matching
  `ndc_update`.
- Lifecycle: `on_axil_exit` → `close_all(0)` (map_sync + corm_save + mod_close,
  no process exit); `on_axil_disconnect` → `nd_disconnect`; `on_axil_vim` →
  `nd_vim`.

## Persistence audit (2026-09-25) — real bug: graceful shutdown erases the store

Reproduction and root cause, with pointers:

1. **Isolation test passes**: `corm_open(db,"player")` + `islet_open(db,"map")`
   on ONE file → put → `corm_save()` → reopen → value intact (42-byte store).
   corm+islet CAN share `world_db()`'s file (corm.h:217-219: one file, many
   XXH32-keyed logical databases).
2. **Engine live-save works**: raw `connect alice` + `save` (do_save →
   map_sync → corm_save, world.c:653-654) → store grows to 892 bytes
   (`GLS_BINARY_LSB_FIRST`); "player" holds alice.
3. **Graceful exit destroys it**: `kill -SEGV` (engine handler close_all,
   world.c:354; axil had set SIGSEGV→exit_all at axil.c:208, engine wins) →
   store truncates to 0 bytes and the next boot re-creates alice
   (`eng_object_add 1 -> 1`). Every orderly restart currently resets the
   world; test.sh never noticed (fresh mktemp store each run). SIGKILL (no
   destructor) is accidentally safer than graceful shutdown today.
4. **Root cause (libcorm source, ~/site/external/libcorm/src/libcorm.c)**:
   `corm_close(hd)` frees the map and REMOVES hd from its file's `ids`
   (libcorm.c:2231-2238). The process-exit destructor `corm_destruct`
   (libcorm.c:1005-1023) then runs `corm_save()` over files whose `ids` are
   empty → `corm_save_file` (libcorm.c:2743) computes size 0 and
   `ftruncate(fd, 0)` (libcorm.c:2758). I.e. closing file-backed handles
   before exit makes the exit-save destructive.

**Status: FIXED + VERIFIED (2026-09-25) — three changes in
`~/site/external/libcorm/src/libcorm.c`. Two-boot engine probe PASSES.**
- 1) `corm_destruct`'s destructive re-save: `corm_save_file` now returns
  early when `file->size == 0` (no dirty handles) — an existing store is
  never `ftruncate(0)`-ed. This is the "fix decision (a)" equivalent.
- 2) `_corm_load` mismatch-skip was `mm + size + sizeof(uint32_t)` — the
  stored per-block `size` field equals the FULL block (4+8+4 header +
  records), so every skipped block overshot by 4+8=12 extra (total +16).
  Corrected to `mm + size - sizeof(uint32_t) - sizeof(size_t) - mm_start`.
- 3) `corm_load_file` (`static void corm_load_file(char*, uint32_t dbid,
  uint32_t hd)`) rewriten to SCAN the file by stored block sizes and load
  any block whose `lid == dbid` into the opening handle `hd`. The old walk
  paired `file->ids` (descending handle order) with on-disk blocks and
  succeeded only while one node per id; on a fresh multi-table reload each
  open consumed ONLY blocks 0..N-1, so early tables' blocks were consumed by
  a LATER open with a mismatching request → data silently lost / loaded into
  the wrong handle, and after enough tables the walk pointer left the mmap
  → SIGSEGV at boot2 (`_corm_load` garbage `mmaped` in gdb: frame world.c:366
  during `corm_open(db,"action"…)`).
- **Proven at corm level**: 4-table save→reload round-trip
  (`/tmp/cormt4.c`, tables st/action/player/contents in ONE file) — no
  crash; all values reload correctly. Full corm `make test` suite GREEN
  (exit 0) after each change.
- **Proven at engine level (two-boot probe, 2026-09-25, fixed local lib)**:
  boot1 `connect alice` + `save` + `kill -SEGV` leaves the store at **892
  bytes**; boot2 on the same store logs `nd_player_login: 'alice' (6/1)`
  (real ref 1, not `/4294967295`) and runs **zero** `eng_object_add` — alice
  REUSES the persisted player, no recreation, no crash; store reaches 1362B
  at the second exit. Do NOT re-verify persistence again — it is done.
- **Deployment note (NOT a verification gap)**: the system-wide
  `/lib/libcorm.so` + `/usr/lib/libcorm.so` predate fixes 2+3 (md5
  `42cbd4f2…` vs local `ecd7300c…`). For non-dev runs / a clean `./test`
  against the real installed stack, run `make && sudo make install PREFIX=/`
  in `~/site/external/libcorm` once. Correctness is already proven against
  the local build (via `LD_LIBRARY_PATH`).

## Fixed landmines — do not reintroduce

- **WS-upgrade corruption from raw detection (2026-09-25, REAL bug)**: raw
  "first line" detection runs on ANY non-`DF_WEBSOCKET` line — but a WS
  client is NOT yet `DF_WEBSOCKET` while its `GET /nd HTTP/1.1…` upgrade
  request is being parsed. The first build sent IAC + banner into that
  handshake (matching the "200 OK + COOP" failure signature). Fix that MUST
  stay: `is_http_method()` in libaxil-nd.c — HTTP request lines pass through;
  only the first NON-HTTP line marks the fd raw and triggers negotiation.
  Side effect: after this fix the historical ~1-in-4 "no 101" flake did not
  reproduce in 8/8 runs — see Known quirks.
- **Fresh-db `do_view` crash (blocker B)**: a new player with
  location=NOTHING/looked unmap got garbage `opos` from a missed
  `eng_map_where` and an unbounded biome-map read. Fixes that MUST stay:
  `eng_map_where`/mwhere zero the output pos on miss (map.c:194) and
  `biome_bg` clamps `i >= BIOME_MAX` to 0 = void (view.c:23-24).
- **Connect-time `[demo] on_enter` flake (blocker C)**: `ws_write` is
  synchronous and axil only tail-flushes after commands, so event frames
  buffered in `on_axil_connect` never reached the client reliably. Fix that
  MUST stay: explicit `nd_flush(player_ref)` at the end of the connect hook
  (libaxil-nd.c:163) — the ioc dedup self-flushes on every DIFFERING write, so
  only the last-buffered batch needs the manual flush. Tests use BOUNDED POLL
  windows, never fixed sleeps.

## Remaining functional gaps

1. ~~**Raw-telnet guest play path**~~ → **DONE** (2026-09-25): `connect`
   command (`CF_NOAUTH`), `nd_player_login` shared landing, raw detection +
   negotiation + banner in `on_axil_parse` (see Library findings). Tested by
   the raw `nc`/fd4 case in test.sh. Remaining wrinkle: raw-close prune (no
   `on_axil_disconnect` for raw fds) — offline-persist semantics accepted;
   see Open audits.

2. ~~**Static client under `/nd`**~~ → **DONE** (2026-09-25):
   - Server route matching on `GET:/nd`, `GET:/nd/`, and wildcard `GET:/nd/*`
     implemented in `src/libaxil-nd.c`. Non-WebSocket requests to `/nd` serve
     `index.html` via `axil_sendfile()` (leveraging axil's pre-populated
     `mime_hd` and deferred streaming fix), while
     WebSocket upgrade requests (`HTTP_SEC_WEBSOCKET_KEY` present) continue
     to `axil_ws_upgrade(fd)`.
   - Asset resolution hierarchy (`resolve_nd_asset`) checks `AXIL_HTDOCS`,
     `./htdocs`, `.`, and `./art` with path traversal guards (`..` rejection).
   - Replaced redundant local `nd_sendfile` and `init_mime_types` implementations
     with direct delegation to `axil_sendfile(fd, path)` (matching the
     `axil-tty` `serve_htdocs` standard pattern).
   - Client HTML template staged in `htdocs/index.html` wrapping NeverDark DOM
     and linking `/nd/styles.css` + `/nd/index.js`.
   - Automated HTTP tests added in `test.sh` for `/nd`, `/nd/`, `/nd/styles.css`,
     `/nd/art/biome/void/1.jpeg`, 404 on missing assets, and directory traversal.

3. **Data defaults & first boot** — PARTIAL
   - ~~`world_db()` default is hard-coded `/var/nd/std.db`~~ → **DONE**
     (2026-09-25): default is now `"std.db"` cwd-relative (axil chdir()s into
     `axil_config.chroot` before modules load, axil.c:212 vs 230, so cwd IS
     the data root; `-C /var/nd` → `/var/nd/std.db`, dev host → `./std.db`).
     `AXIL_ND_DB` override kept. Verified: env-less boot creates `./std.db`.
   - First-boot seeding: semantic clarified — `game/std.db.ok` (and
     `game/data/minimal.db`) are ASCII object DUMPS (***END OF DUMP***), NOT
     stores; `game/std.db.new.ok` is a real qsys store (Berkeley Btree) that
     corm/islet auto-load → "drop seed into place" = copy it to the data-dir
     default. Fresh boot is ALREADY playable (self-seeds Room Zero + elements
     + skel + biome + `mod_init` demo, world.c:437-503), so seeding is
     optional flavor. NOTE: an existing store skips `mod_init` (existed
     branch → mod_load_all only) — the demo `[demo] on_enter` world is a
     FRESH-store artifact.
   - ~~⚠ **NEW BLOCKER (2026-09-25): orderly shutdown zeroes the store.**~~
     → **FIXED + VERIFIED.** See "Persistence audit" below. The engine
     two-boot probe passes (store survives restarted shutdown; players
     reuse). Only remaining action is the one-shot system libcorm install
     (see the audit).

4. **man/help** (needs axil-tty)
   - `do_man` (world.c:515) is a placeholder. Real man = execl via axil-tty +
     section-10 manpages in the chroot (`-laxil-tty`). Deferred unless axil-tty
     is adopted.

5. ~~**Identity cleanup**~~ → **DONE** (2026-09-25): the dead QSESSION
   branch (`axil_auth_check`) is deleted; `auth()` is REMOTE_USER-only; raw
   guests come through `connect` (item 1). No `./sessions/` handling anywhere.

## Tests still missing (P5 scope)

6. - ~~`nc` raw-telnet guest test~~ → **DONE** (test.sh raw fd4 case: connect,
      on_enter, IAC greps, `say` round-trip).
    - ~~BCP frame assertions (gap 6)~~ → **DONE** (2026-09-25): on connect the
      serializers in mcp.c emit `#b` + iden frames: AUTH_SUCCESS (iden 6 →
      hex `236206`), ACTIONS (`236209`), VIEW_BUFFER (`236202`, via
      `fbcp_view_buf`), TOD (`236208`). NOTE: BCP frames are **WS-only** —
      `eng_nd_wwrite` (io.c:200) writes them only to `DF_WEBSOCKET` fds; raw
      guests get plain text. VIEW (iden 1) does NOT fire at connect. There is
      No `BCP_HP` iden in mcp.c's enum (identical to the original nd mcp.c)
      and no `mcp_bar` fires at connect — the HP item in the original gap-6
      note was aspirational; neither is asserted.
    - ~~Persistence regression case~~ → **DONE** (2026-09-25): two boots on
      one store at `$((port+1))`/`$((port+2))`. Boot A: `connect alice` →
      `save` → `kill -SEGV`, asserts store > 0 and the stderr log fires
      `eng_object_add` (player created). Boot B on the same store: `connect
      alice` must log `nd_player_login: 'alice'` with the real ref — no
      `eng_object_add`, no `/4294967295` new-player sentinel — store still
      > 0. Both pids in the EXIT trap; logs captured per boot (WARN/`object.c`
      go to stderr). Stable across 5 consecutive runs.
    - Already covered: 101 + accept, verb round-trip, IAC negotiation greps.

## Open audits (verify; likely no code change)

7. - map→islet: multi-value cells under NOOVERWRITE; `map_mwhere` sentinel
     (`130056652770671`) comparisons (view.c).
   - **Raw-close prune (NEW)**: raw fds never fire `on_axil_disconnect`, so
     after a raw guest's socket dies the fd stays in `fds_hd` forever →
     `eng_nd_write` keeps flushing into a dead fd and every reconnect of the
     same name leaks an entry. v1 accepts offline-persist semantics; a prune
     needs either a library change (axil close notifier, "only if MUST") or a
     periodic probe. Do NOT add the probe to `nd_update` casually.
   - mutation-during-iteration in `mod_load_all` (snapshot keys before re-put).
   - Dependency inventory: drop `-lqdb -ldb -lndc`; confirm
     `-lxxhash -lpcre -lbsd -lrt -lcrypto -lssl`.
   - WS fd accounting: `ws_lb[FD_SETSIZE]` static (8 KB × 1024 = 8 MB BSS) —
     fine, but review if `FD_SETSIZE` defaults change.
   - Keep the regression gates green on every change: `rg 'qdb_|qmap_|ndc_'`
     over live `src/*.c` code = 0 (remaining hits are comments in
     world.c/libaxil-nd.c + the DEAD legacy TU `interface.c`, which is not in
     the build path); `nm -D` census 107 `*_adapter` / 49 `eng_*` / 20 `sic`;
     `pgrep -x axil` clean after runs. Legacy `interface.c`/`libnd.c` remain
     present but dead (excluded by the make smoke target).

## P6 site integration (deferred, thin)

8. - Ship as package or `external/axil-nd` submodule; load via `start.sh -m`;
     data under site `var/nd`; optional `/nd` link. Zero site-specific code in
     the module.

## Open quest leftovers

9. - `axilnd-modcap-xy` (J4.8 step 2): S4 slices remain.
   - `libnd.a` / `struct nd` vtable compat: `shared_init` still fills the legacy
     surface; external content modules using `-lnd` are unverified.

## Known quirks (keep, not gaps)

- Engine-TU `.c` edits need `rm -f src/*.o && make smoke && make` (mk
  `DEP_FILES` is read before the target list). Run from `~/axil-nd` repo root.
- `make` prints `find: './htdocs': No such file or directory` — harmless
  (htdocs/ is gitignored/absent, same as axil-tty).
- The `demo.so` Makefile target omits `--no-execute-only` (ARM-only flag);
  `mods/demo/demo.so` rebuilds via `make demo`.
- `make test` only BAKES `test.sh` into a `test` artifact — run `./test`.
- Run `pgrep -x axil` between runs to catch stray daemons on fixed ports;
  test.sh's EXIT trap kills its own axil.
- Runtime launch: `( LD_LIBRARY_PATH=$PWD/lib axil -d -A -p $PORT
  -m /home/quirinpa/axil-nd/lib/axil-nd & )` — pass the SONAME path
  (`lib/axil-nd`, the loader appends `.so`). Inspect/modify
  `AXIL_ND_DB` on dev hosts (STD_DB `/var/nd/std.db` is not writable).
- test.sh "~1 in 4 no 101" flake (HTTP 200 + COOP instead of 101): after the
  raw-detection/`is_http_method` fix it did NOT reproduce in 8/8 runs
  (2026-09-25) — the fix may have removed an upgrade-corrupting write. Keep
  watching; if it reappears, first check `~/axil-tty/test.sh` (identical
  structure) for the same flake.
- `~/axil-nd/.pi/quest/future/1xA2sA.md` is a stray quest draft — do NOT commit
  it (`.pi/` is otherwise untracked by intent). Quest archives for the finished
  J4.8 work live in `/home/quirinpa/site/.pi/quest/archive/` (axilnd-p4-glue,
  axilnd-interface-split, axilnd-map-libislet).