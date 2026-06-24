[!] Shared page shell for WebDNA MySQLAdmin — dark/red sidebar + topbar,
    Bootstrap 5.3. Server-rendered multi-page model: one WebDNA-processed
    .html per page; the sidebar links to real URLs; the active item is set by
    [text]nav=...[/text] before this include. Also set [text]page_title=...[/text].
    Requires config.inc to have run first (provides [WMS_ADMIN_USER]/[WMS_DB_HOST]).
    Closed by lib/wms_foot.inc.
    Bootstrap, Bootstrap Icons and jQuery are vendored under /assets/vendor/ (no
    CDN), so the tool works on locked-down/offline boxes. [/!]
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>[WMS_APP_NAME] — [page_title]</title>
<link href="/assets/vendor/bootstrap/bootstrap.min.css" rel="stylesheet">
<link href="/assets/vendor/bootstrap-icons/bootstrap-icons.min.css" rel="stylesheet">
<link href="/assets/css/custom.css" rel="stylesheet">
</head>
<body>
<div class="app">
  <aside class="sidebar" id="sidebar">
    <div class="brand"><i class="bi bi-database-fill-gear"></i> [WMS_APP_NAME]</div>

    <div class="nav-group">Server</div>
    <a class="nav-link[switch value=[nav]][case value=server] active[/case][/switch]" href="/"><i class="bi bi-hdd-network"></i> Dashboard</a>
    <a class="nav-link[switch value=[nav]][case value=databases] active[/case][/switch]" href="/databases.html"><i class="bi bi-stack"></i> Databases</a>

    <div class="nav-group">Query</div>
    <a class="nav-link[switch value=[nav]][case value=sql] active[/case][/switch]" href="/sql.html"><i class="bi bi-terminal"></i> SQL</a>

    <div class="nav-group">Transfer</div>
    <a class="nav-link[switch value=[nav]][case value=import] active[/case][/switch]" href="/import.html"><i class="bi bi-box-arrow-in-down"></i> Import</a>
    <a class="nav-link[switch value=[nav]][case value=export] active[/case][/switch]" href="/export.html"><i class="bi bi-box-arrow-up"></i> Export</a>

    <div class="nav-group">Admin</div>
    <a class="nav-link[switch value=[nav]][case value=users] active[/case][/switch]" href="/users.html"><i class="bi bi-people"></i> Users</a>
  </aside>

  <div class="main">
    <div class="topbar">
      <button class="btn btn-sm btn-light d-md-none" onclick="document.getElementById('sidebar').classList.toggle('open')"><i class="bi bi-list"></i></button>
      <div class="conn-pill"><i class="bi bi-person-circle"></i> [WMS_ADMIN_USER] <span class="text-muted">&middot;</span> <i class="bi bi-plug-fill text-success"></i> [WMS_DB_HOST]:[WMS_DB_PORT]</div>
      <div class="ms-auto d-flex align-items-center gap-2">
        <a class="btn btn-sm btn-outline-secondary" href="/sql.html"><i class="bi bi-terminal"></i> New query</a>
        <a class="btn btn-sm btn-outline-danger" href="/logout.html"><i class="bi bi-box-arrow-right"></i> Log out</a>
      </div>
    </div>
    <div class="content">
