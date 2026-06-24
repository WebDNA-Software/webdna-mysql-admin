# WebDNA MySQLAdmin

A WebDNA re-implementation of phpMyAdmin — a web UI to administer MySQL/MariaDB,
built to the house style of the cloud-hosting-console (server-rendered, one
WebDNA-processed `.html` per page, Bootstrap 5.3 + jQuery from CDN).

This is **v1 / Core + Import-Export**. It talks to MySQL through WebDNA's native
`[sqlconnect]` / `[sqlexecute]` / `[sqlresult]` contexts.

## What works

| Area | Page | Notes |
|------|------|-------|
| Login | `login.html` | Admin-account login (this tool's own users); first-run creates the first admin. |
| Dashboard | `index.html` | Server version, current user, database count, database list. |
| Databases | `databases.html` | List with size/collation; **CREATE** and **DROP** database. |
| Tables | `db.html` | Tables in a database; **DROP** and **TRUNCATE** table. |
| Browse | `table.html` | Paginated rows, dynamic columns. |
| Structure | `structure.html` | Columns + indexes (from `information_schema`). |
| SQL | `sql.html` | Run any SQL; result grid for single `SELECT`/`WITH`. |
| Import | `import.html` | Paste a SQL script; statements split on `;` and run in order. |
| Export | `export.html` | SQL dump (DDL + INSERTs) for a DB or table; CSV for a table. |
| Users | `users.html` | Manage the admin accounts that can sign in to this tool. |

## Two credential layers (important)

Keep these separate — they are unrelated:

1. **MySQL connection** — ONE static account (`WMS_DB_*` in `config.inc`), shared
   by everyone. This is what the tool administers *with*.
2. **Admin-system login** — each person has their own account in `db/admins.db`,
   used only to sign in to *this tool*. Passwords are stored encrypted with
   `[encrypt method=AES]`, seeded by `SITE-SEED` from `config.inc` (login verifies
   by re-encrypting and comparing). Sessions are a unique token (`[cart]`) in a
   cookie mapped to `db/sessions.db`; no credentials are stored in the browser.
   (AES is reversible; moving to a one-way salted KDF is on the roadmap.)

## Setup

1. **Serve this folder** with a WebDNA-enabled web server, with `.html` handled
   by WebDNA. The root (`/`) should serve `index.html` through WebDNA.

2. **Config** — copy and edit the per-box config:
   ```sh
   cp config.inc.example config.inc
   ```
   Set `WMS_SESSION_KEY` and `SITE-SEED` (both long random strings; `SITE-SEED`
   is the admin-password encryption seed) and the static MySQL account
   `WMS_DB_HOST` / `WMS_DB_PORT` / `WMS_DB_USER` / `WMS_DB_PASS`.

3. **Runtime stores** — create the live DBs from their templates:
   ```sh
   cp db/admins.db.example    db/admins.db
   cp db/sessions.db.example db/sessions.db
   ```

4. **Open `/`.** With no admin accounts yet, you're sent to `/login.html`, which
   offers to **create the first admin**. After that it's a normal login; add more
   people on the **Users** page. See [`FIRSTRUN.md`](FIRSTRUN.md) for the full
   first-run walkthrough, prerequisites, and troubleshooting.

## The one place to configure: the connection

The MySQL connection is opened **once per request** in
[`lib/wms_connect.inc`](lib/wms_connect.inc) (included by `config.inc`) as a named
persistent connection, `myAdmin`:

```
[sqlconnect dbtype=MySQL&host=[WMS_DB_HOST]&database=[WMS_DB_NAME]&uid=[WMS_DB_USER]&pwd=[WMS_DB_PASS]&conn_var=myAdmin]
  [text]DBtest=[SQL_SERVERTYPE][/text]   [!]-- "mysql" on success --[/!]
[/sqlconnect]
```

`WMS_DB_*` come from `config.inc`. If the connect fails (`[SQL_SERVERTYPE]` isn't
`mysql`) it redirects to `/error_500.html`. Every page then runs queries against
that connection by reference — the SQL is the **body** of `[sqlexecute]`, results
are captured into a named `result_var`, and read back with
`[sqlresult result_ref=…]` (columns by their SELECT alias):

```
[sqlexecute conn_ref=myAdmin&result_var=result1]
SELECT a, b FROM ...
[/sqlexecute]
[sqlresult result_ref=result1][founditems] [a] [b] [/founditems][/sqlresult]
```

(Non-SELECT statements omit `result_var`.) If your build differs, adjust the one
`[sqlconnect]` in `lib/wms_connect.inc`.

## Architecture

- **One include per page.** Every page's first line is `[include file=config.inc]`
  (your convention). `config.inc` sets the static config, then loads
  [`lib/wms_connect.inc`](lib/wms_connect.inc) (opens the `myAdmin` MySQL connection),
  [`lib/wms_db.inc`](lib/wms_db.inc) (helpers) and [`lib/wms_auth.inc`](lib/wms_auth.inc)
  (the admin-login guard).
- **Admin login / sessions.** The guard reads the `wms_token` cookie, looks it up
  in `db/sessions.db`, and sets `[WMS_AUTHED]` / `[WMS_ADMIN_USER]`. No valid
  token → redirect to `/login.html`, unless the page declared
  `[text]WMS_PUBLIC=1[/text]` first (login/logout do). This authenticates the
  *person*; the MySQL connection is the separate static `WMS_DB_*` account.
- **Shell.** [`lib/wms_head.inc`](lib/wms_head.inc) / [`lib/wms_foot.inc`](lib/wms_foot.inc)
  wrap each page (sidebar + topbar). Set `[text]nav=…[/text]` and
  `[text]page_title=…[/text]` before including the head.
- **Helpers.** [`lib/wms_db.inc`](lib/wms_db.inc): `[sqlid]` (identifier quoting),
  `[sqlval]`/`[csvval]` (value escaping via `[grep]`), `[wms_int]` (sanitise URL
  ints for LIMIT/OFFSET), `[wms_size]` (human bytes). The MySQL connection itself
  is opened in [`lib/wms_connect.inc`](lib/wms_connect.inc). Admin passwords use
  `[encrypt method=AES&seed=[SITE-SEED]]` directly in `login.html` / `users.html`.
- **Dynamic columns.** WebDNA reads result columns by name, so for unknown column
  sets we read the column list (`GROUP_CONCAT`) and render each cell with
  `[interpret][[colname]][/interpret]` — the same dynamic-tag trick the cloud
  console uses for JSON.

## Conventions followed (from cloud-hosting-console)

- Single-branch conditionals use `[showif COND]…[/showif]`, not `[if COND][then]…[/then][/if]`.
  `[if][then][else][/if]` is kept only where there are genuinely two branches.
- `[text]…[/text]` defaults to no output, so no `show=F` is written. When a value
  *should* render, use lowercase `[text show=t]…[/text]`.
- Use `1`/`0`, **not** `T`/`F`, in conditionals (WebDNA treats a bare `T`/`F` as a boolean).
- Use `[switch]`/`[case]` for values with hyphens (WebDNA `[if]` mis-parses `-`).
- Not-equal is `!`, **not** `!=` (which mis-parses). Quote and parenthesize `[if]`
  operands so empty/spaced values compare safely: `[if ("[x]"="")]`, `[if ("[a]"!"[b]")]`.
- Avoid WebDNA reserved words as field/variable/param names — this app uses
  `myadminuser` (not `username`), `admins.db` (not `users.db`), the `dbtable` URL
  param (not `table`), and `newsql` (not `sql`).
- Bootstrap/icons/jQuery from CDN; custom CSS served static (not WebDNA-processed).

## Verify against YOUR WebDNA build

A few things depend on your exact WebDNA + MySQL setup. They're isolated so you
can confirm/adjust in one spot each:

1. **Connection params** — the `[sqlconnect …&conn_var=myAdmin]` in
   `lib/wms_connect.inc`. Built to your proven SQLConnect syntax
   (`dbtype=MySQL&host=…&database=…&uid=…&pwd=…`), validated with `[SQL_SERVERTYPE]`.
2. **WebDNA `[search]` "match all"** — first-run detection and the user list use
   `[search db=db/admins.db&neMYADMINUSERdatarq=[blank]]` (myadminuser not-equal
   blank = all rows); exact lookups use `eqMYADMINUSERdatarq=[uname]`. If your build
   needs different forms, adjust them in `login.html` / `users.html`.
3. **`[grep]` replace semantics** — `[sqlval]`/`[csvval]`/username-sanitising rely
   on regex replace. Spot-check an export of data containing quotes/backslashes.
4. **`[delete db=…&eqFIELDdata=…]`** — used by `users.html` (delete user) and
   `logout.html` (prune session). Confirm a delete actually removes the row.
5. **SQL error reporting** — pages currently surface result *metadata*
   (`[numrowsaffected]`, `[insertid]`, rows found). If your build exposes a SQL
   error string/`[sqlinfo]`, we can surface failures inline (roadmap).

## Roadmap (next milestones)

- **Row editing**: inline edit / insert / delete (needs primary-key detection).
- **Create table** UI (column builder) and ALTER (add/drop/modify column, index).
- **MySQL user & privilege admin** (`CREATE USER`, `GRANT`/`REVOKE`) — distinct
  from this tool's own admin accounts.
- **Admin accounts**: replace reversible AES storage with a one-way salted KDF,
  password change, session TTL/idle expiry, optional roles (read-only vs full).
- **Import**: `.sql`/`.csv` file upload; CSV-to-table with column mapping;
  quoted-`;` aware statement splitting.
- **Export**: real file download (`Content-Disposition`), gzip, true `NULL`
  handling, row limits/chunking.
- **Server**: status/variables, process list, charsets/engines.
- Vendor Bootstrap/jQuery locally for locked-down/offline boxes.

## Scope decisions (agreed at kickoff)

- v1 scope: **Core browse/SQL + Import/Export**.
- MySQL connection: **one static account** in `config.inc` (shared).
- App access: **per-admin login accounts** for this tool (`db/admins.db`),
  separate from the MySQL connection.
- Look & feel: **cloud-console house style**.
- Target: **MySQL 8.x** (uses `information_schema`, `utf8mb4`).
