# First run — creating the initial admin

There is **no default username or password**. On first run the tool detects that
it has no admin accounts and lets you *create* the first one. After that it's a
normal login, and you add more people on the **Users** page.

This document covers only the first-admin bootstrap. For general install steps
see [`README.md`](README.md).

## How it works

The two credential layers are unrelated — don't confuse them (see
[`README.md`](README.md#two-credential-layers-important)):

- **MySQL connection** — one static account (`WMS_DB_*` in `config.inc`), shared.
- **Admin-system login** — per-person accounts in `db/admins.db`. This is what you
  create on first run.

When you open `/login.html`, the page counts the rows in `db/admins.db`:

```
[search db=db/admins.db&neMYADMINUSERdatarq=[blank]&max=5000][text]nusers=[numfound][/text][/search]
```

- `nusers = 0` → it shows the **"First run — create the initial admin account"**
  form (`action=setup`).
- `nusers > 0` → it shows the normal **"Sign in"** form (`action=login`).

On submit, the setup branch encrypts the password with
`[encrypt method=AES&seed=[SITE-SEED]]`, appends the account to `db/admins.db`,
creates a session token (`[cart]`) in `db/sessions.db`, sets the `wms_token`
cookie, and redirects you in — already signed in.

## Prerequisites (do these before first run)

All of these are normal install steps; first-run setup silently fails without them.

1. **`config.inc` exists and is filled in** (it is gitignored — copy the template):
   ```sh
   cp config.inc.example config.inc
   ```
2. **`SITE-SEED` is a real long random string** — *not* the `CHANGE-ME-...`
   placeholder. It is the AES seed used to encrypt every admin password. **Set it
   before you create the first admin** — changing it afterwards invalidates every
   admin password (login can no longer match them; you'd have to re-bootstrap).
3. **MySQL is *not* needed for login.** The static account (`WMS_DB_*`) is
   required to *use* the tool (it administers MySQL), but admin passwords are
   encrypted by WebDNA's `[encrypt]`, so you can create the first admin and sign
   in even while MySQL is unreachable.
4. **Runtime DBs exist** (created from their templates, gitignored):
   ```sh
   cp db/admins.db.example    db/admins.db
   cp db/sessions.db.example db/sessions.db
   ```
   `db/admins.db` must have **no admin rows** for first-run mode to trigger.

## Steps

1. Open `/` (or `/login.html`). With no admin accounts you'll see
   **"First run — create the initial admin account."**
2. Enter the **username** and **password** you want for your admin login. These
   are *your* choice — there is nothing to look up.
3. Click **Create admin & sign in**. The account is written to `db/admins.db` and
   you're taken straight to the dashboard, signed in.
4. Add any further admins from the **Users** page. First-run mode never reappears
   while at least one account exists.

## Troubleshooting

**I see "Sign in", not "First run."**
`db/admins.db` already contains at least one admin. Either sign in with that
existing account, or re-bootstrap by replacing the file with the empty template:
```sh
cp db/admins.db.example db/admins.db
```
(That removes all admin accounts — it does not touch any MySQL data.)

**I created the admin but can't sign back in.**
Almost always one of:
- `SITE-SEED` was changed after the account was created → the re-encrypted
  password no longer matches the stored value. Re-bootstrap
  (`cp db/admins.db.example db/admins.db`) and create the admin again with the
  current seed.
- Your WebDNA `[encrypt method=AES]` isn't deterministic for a fixed seed (e.g. it
  uses a random IV), so the login-time re-encryption never matches what was
  stored. In that case switch the verify step to `[decrypt]`-and-compare.

**The login page shows raw tags like `[WMS_APP_NAME]` / `[err]`.**
The page isn't being WebDNA-processed (so `config.inc` never ran). Check that the
web server hands `.html` to WebDNA and that `login.html`'s top logic block is not
commented out.

**Forgot the admin password.**
There's no built-in recovery flow — re-bootstrap with
`cp db/admins.db.example db/admins.db` and create a new first admin. (The stored
value is AES, so it's technically decryptable with `SITE-SEED`, but there's no UI
for that.)
