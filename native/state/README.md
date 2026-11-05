# Runtime state directory

The packaged `state/` directory is root-owned with Unix mode 01777. Its sticky
bit lets the non-root webOS application create a directory without granting it
write access to the installed executable, libraries or assets. Each runtime UID
creates and owns `user-<uid>/` for its score table and diagnostic receipts.

The score store proves a write/replace at startup before reporting saving as
available. It imports a valid table from the former installation-root location
only when the new table is absent, preferring named scores over numeric legacy
scores. It never overwrites those old files or `record.txt`.

Do not package runtime scores, temporary files or receipts. Uninstall or a change
of application UID may remove access to previous state; survival of those events
is not promised. No root service or TV-wide permission change is required.
