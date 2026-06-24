[!]
  lib/wms_error.inc — shared shell for the error_NNN.html pages.

  SELF-CONTAINED ON PURPOSE: it does NOT include config.inc. config.inc runs
  wms_connect.inc (DB connect) and wms_auth.inc (login guard), both of which can
  [redirect] — and wms_connect.inc redirects to /error_500.html on DB failure, so
  pulling it in here would loop the 500 page. Each error_NNN.html sets
  [errcode]/[errtitle]/[errmsg] then includes this. WebDNA-processed.
[/!]
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>[errcode] — WebDNA MySQLAdmin</title>
<link href="/assets/vendor/bootstrap/bootstrap.min.css" rel="stylesheet">
<link href="/assets/vendor/bootstrap-icons/bootstrap-icons.min.css" rel="stylesheet">
<link href="/assets/css/custom.css" rel="stylesheet">
<style>
  body{background:linear-gradient(135deg,#C61D23,#0d1424);min-height:100vh;display:grid;place-items:center;margin:0;}
  .err-card{width:460px;max-width:92vw;border:0;border-radius:16px;box-shadow:0 18px 50px rgba(0,0,0,.35);text-align:center;}
  .err-card .code{font-size:3.5rem;font-weight:700;color:#C61D23;line-height:1;}
  .err-card .code i{color:#e8772e;}
</style>
</head>
<body>
<div class="card err-card">
  <div class="card-body p-4">
    <div class="code"><i class="bi bi-exclamation-octagon"></i> [errcode]</div>
    <h1 class="h5 mt-3">[errtitle]</h1>
    <p class="legend mb-4">[errmsg]</p>
    <a class="btn btn-success" href="/"><i class="bi bi-house"></i> Dashboard</a>
    <a class="btn btn-outline-secondary" href="/login.html"><i class="bi bi-box-arrow-in-right"></i> Sign in</a>
  </div>
</div>
</body>
</html>
