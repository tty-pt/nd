# NeverDark
> This is a game forked from fbmuck. Since then it has been modified extensively. It now runs on a browser as well as telnet / openssl client.

# Build
```sh
make
pnpm build
```

# Run
```sh
mkdir sessions
echo One >> sessions/a
cp game/std.db.ok game/std.db
./nd
```

- Open your browser on localhost:4201
- In the web terminal:
```sh
exit
auth a
X
```
