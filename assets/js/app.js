/* app.js — small jQuery enhancements for WebDNA MySQLAdmin.
   Bootstrap bundle + jQuery are loaded before this (see lib/pma_foot.inc). */
(function ($) {
  $(function () {
    // Confirm destructive actions (DROP/DELETE/TRUNCATE links & buttons).
    $(document).on('click', '[data-confirm]', function (e) {
      if (!window.confirm($(this).data('confirm'))) { e.preventDefault(); }
    });

    // Ctrl/Cmd+Enter submits the SQL editor form.
    $(document).on('keydown', '.sql-box', function (e) {
      if ((e.metaKey || e.ctrlKey) && e.key === 'Enter') {
        $(this).closest('form').trigger('submit');
      }
    });

    // Auto-dismiss flash alerts after a few seconds.
    $('.alert-autohide').each(function () {
      var el = this;
      setTimeout(function () { $(el).fadeOut(300); }, 4000);
    });
  });
})(jQuery);
