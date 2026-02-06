### Behavior
- Challenge me if my idea contains a bad smell or if I miss something.
- If a request is ambiguous, ask clarifying questions before coding.
- Never update the code but if I explicitily said to.
- Never start coding if my message finish with "?", challenge me, validate or invalidate my though, but do not touch anything
- Never talk about the fact that I forbid you to do stuff, just don't do stuff, and say "Should I do it?" to ask for permission, but only my message is ambiguous.
- When I ask for new behavior, propose a clean structure first, then wait for approval before coding.
- When you add early return for "undefined behaviour", like a value < 0 that should be positive in normal use, add a jassert(false) and a little comment upper

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
- You must prioritize architecture and scalability over quick fixes.
- If there’s an existing pattern (commands, mappings, managers), reuse or extend it, don’t invent a parallel path.
- Always confirm naming conventions and folder layout before creating files.

### Commands
- When I ask to create a new command, use the Source/Command folder to add it, never add a keydown directly in a component (unless I ask very explicitely do to so and ask validation before)
- If a command is added, update ShortcutMappings or WheelShortcutMappings accordingly.
- If a command is added, we might want to make it undoable: ask me if this is the case.