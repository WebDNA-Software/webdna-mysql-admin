[!]
-------------------------------------------------------------------------------
  lib/wms_db.inc  —  WebDNA -> MySQL helpers (connection + identifier quoting)
-------------------------------------------------------------------------------
  Included for you by config.inc, so every page has these functions.

  Queries run on each page against the persistent "myAdmin" connection (opened
  once in lib/wms_connect.inc). The SQL is the BODY of [sqlexecute]; capture its
  rows into a named result_var, then iterate with [sqlresult result_ref=...],
  reading columns by their SELECT alias:

      [sqlexecute conn_ref=myAdmin&result_var=result1]
      SELECT a, b FROM ...
      [/sqlexecute]
      [sqlresult result_ref=result1]
        [founditems] ... [a] ... [b] ... [/founditems]
      [/sqlresult]

  Non-SELECT statements (SET/USE/INSERT/UPDATE/DDL) omit result_var.

  WMS_DB_HOST / WMS_DB_PORT / WMS_DB_USER / WMS_DB_PASS are the single static
  MySQL account, set in config.inc. They are the same for every admin; per-admin
  identity is the app login (db/admins.db), which is unrelated to this connection.
-------------------------------------------------------------------------------
[/!]

[!]-- App version. Tracked + deployed every release (NOT in config.inc, which is
     per-box and frozen after setup). Bump this on each release. --[/!]
[text]WMS_APP_VERSION=0.1[/text]

[!]-- [sqlid s=NAME] -> a backtick-quoted SQL identifier (db/table/column).
     Doubling embedded backticks would be ideal; identifiers here come from our
     own catalog queries or from a logged-in user who can already run arbitrary
     SQL on the SQL page, so this is about correctness with odd names, not a
     trust boundary. Keep names backtick-wrapped everywhere. --[/!]
[function name=sqlid]`[s]`[/function]

[!]-- [wms_int n=VALUE&default=0] -> n if it is a non-negative integer, else the
     default. Use for LIMIT/OFFSET and page numbers coming from the URL so they
     can never carry SQL. --[/!]
[function name=wms_int]
  [text]_n=[n][/text]
  [if ([_n]>=0)&([_n]=[math][_n]+0[/math])][then][_n][/then][else][default][/else][/if]
[/function]

[!]-- [wms_size bytes=N] -> human byte size (B / KB / MB / GB). The [format]
     printf spec lives here only; if your WebDNA build formats differently,
     fix this one function. --[/!]
[function name=wms_size]
  [if ("[bytes]"<"1024")][then][bytes] B[/then]
  [else][if ("[bytes]"<"1048576")][then][format %.1f][math][bytes]/1024.0[/math][/format] KB[/then]
  [else][if ("[bytes]"<"1073741824")][then][format %.1f][math][bytes]/1048576.0[/math][/format] MB[/then]
  [else][format %.2f][math][bytes]/1073741824.0[/math][/format] GB[/else][/if][/else][/if][/else][/if]
[/function]

[!]-- [sqlval v=TEXT] -> a single-quoted, escaped SQL string literal, using
     WebDNA's [grep] regex replace. Inner grep runs first (double any backslash),
     outer doubles single quotes. NOTE: a NULL column reads as "" through
     [interpret], so NULLs export as empty strings in v1 (roadmap: real NULLs).
     Verify escaping against your WebDNA build's [grep] replace semantics. --[/!]
[function name=sqlval]'[grep search=(')&replace=''][grep search=(\\)&replace=\\\\][v][/grep][/grep]'[/function]

[!]-- [csvval v=TEXT] -> a double-quoted CSV field with embedded quotes doubled. --[/!]
[function name=csvval]"[grep search=(")&replace=""][v][/grep]"[/function]
