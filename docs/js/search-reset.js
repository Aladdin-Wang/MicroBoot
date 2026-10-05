// Material observes keyup/focus. A native form reset can clear a focused
// search field without either event, so notify it after the reset completes.
document.addEventListener('reset', event => {
    const form = event.target;
    if (!(form instanceof HTMLFormElement) || !form.matches('.md-search__form')) return;
    const input = form.querySelector('[data-md-component="search-query"]');
    if (!input) return;
    requestAnimationFrame(() => {
        if (!event.defaultPrevented && input.isConnected) {
            input.dispatchEvent(new Event('keyup', { bubbles: true }));
        }
    });
});
