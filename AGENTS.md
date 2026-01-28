### Behavior
- Challenge me if my idea contains a bad smell or if I miss something.
- Never update the code but if I explicitily said to.

### Comments
- Short, specific, and action‑oriented (describe the role/intent, not implementation).
- Usage context included when helpful, in one short clause.
- Keep it synthetic, but if the usage/context is very complexe, prefer exaustivness
- Use @param lines for parameters when the meaning isn’t obvious.
- Add a blank line between a function and the next comment (if it exists).
- Do not add a blank line between a function and its comment.
- Do not comment overriden function with default implementation (that do nothing), if there is a lot of them, add a comment to clarify why they exist and where they come frome

### Architecure decision
- When adding UI or utility code, prefer reusable components in Common/ (or a shared module) over local/private classes. If something could reasonably be reused in another panel, design it as a shared component with clean APIs and minimal coupling.
- If you choose a local/private class instead, briefly justify why reuse isn’t warranted.