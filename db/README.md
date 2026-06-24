# db/

Local WebDNA databases (tab-delimited). These back the **app's own** state — the
admin accounts and login sessions for this tool. They are **not** the MySQL data
being administered (that lives on the MySQL server, reached live via the static
`myAdmin` connection opened in `lib/wms_connect.inc`).

| file | purpose |
|------|---------|
| `admins.db.example` | schema template (tracked). Copy to `admins.db` at setup. |
| `admins.db` | admin accounts for THIS tool (gitignored). |
| `sessions.db.example` | schema template (tracked). Copy to `sessions.db`. |
| `sessions.db` | live login sessions (gitignored). |

## Setup

```sh
cp db/admins.db.example    db/admins.db
cp db/sessions.db.example db/sessions.db
```

On first visit, `login.html` sees `admins.db` is empty and lets you create the
first admin account. After that it's a normal login; manage further accounts on
the **Users** page.

## admins.db fields

| field | meaning |
|-------|---------|
| `myadminuser` | admin login name (restricted to `A-Z a-z 0-9 _ . @ -`) |
| `passhash` | the password encrypted with `[encrypt method=AES&seed=[SITE-SEED]]` |
| `created` | timestamp the account was created |

These are the people allowed to use the tool — **separate** from the single
MySQL account in `config.inc`.

## sessions.db fields

| field | meaning |
|-------|---------|
| `token` | unique session id (`[cart]`), also stored in the `wms_token` cookie |
| `myadminuser` | which `admins.db` admin this session belongs to |
| `created` | login timestamp |
| `lastseen` | last-request timestamp |

Logout clears the cookie. The token is the only thing in the browser; no
credentials are stored client-side.
