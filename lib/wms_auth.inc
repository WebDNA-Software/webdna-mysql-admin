[!]
-------------------------------------------------------------------------------
  lib/wms_auth.inc  —  admin-login guard (NOT the MySQL connection)
-------------------------------------------------------------------------------
  Loaded for you by config.inc (after the static config + helpers). Do not
  include it directly.

  It authenticates the PERSON using this tool, which is separate from the MySQL
  connection (that is the single static WMS_DB_* account in config.inc):
    1. reads the wms_token cookie
    2. looks the token up in db/sessions.db
    3. on hit  -> [WMS_AUTHED]=1 and [WMS_ADMIN_USER]=<that admin's username>
    4. on miss -> clears the cookie and redirects to /login.html
                  UNLESS the page declared itself public first with
                  [text]WMS_PUBLIC=1[/text] (login.html / logout.html do this).
-------------------------------------------------------------------------------
[/!]
[text]WMS_AUTHED=0[/text]
[text]WMS_ADMIN_USER=[/text]
[text]WMS_TOKEN=[getcookie name=wms_token][/text]

[showif [WMS_TOKEN]!]
  [search db=db/sessions.db&eqTOKENdatarq=[WMS_TOKEN]]
    [founditems]
      [text]WMS_AUTHED=1[/text]
      [text]WMS_ADMIN_USER=[myadminuser][/text]
    [/founditems]
  [/search]
[/showif]

[if ("[WMS_AUTHED]"!"1")&("[WMS_PUBLIC]"!"1")]
  [then]
    [setcookie name=wms_token&value=&path=/&expires=-1]
    [redirect /login.html]
  [/then]
  [else][/else]
[/if]