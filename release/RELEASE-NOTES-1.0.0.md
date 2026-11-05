# SkiFree for webOS 1.0.0 — local candidate

The opening now reads **WebOS 1.0.0** beneath the crossed-out original 1.04. The original mountain, artwork and attribution remain, with no added lift cables and centered controls.

Original is the default and follows the reconstructed SkiFree 1.04 behavior. Recreation and its gentler Yeti Chase remain separate choices on Green and Yellow; Blue opens help.

The latest local revision replaces Recreation's dark HUD bar with compact white status areas and hides footer controls while skiing. Menus and the stationary Original opening retain their choices; physical colour shortcuts remain active.

Qualifying Original scores now prompt for three arcade-style initials, starting at AAA. Keyboard, remote arrows/OK and pointer plus/minus controls are supported. Existing scores import with --- initials into `scores-initials-v1.txt`; the original `scores-v104.txt` and prototype `record.txt` remain untouched. Original's ranking and course rules are unchanged.

The November 5 correction fixes a real TV save failure: SDL's preference path
pointed at the protected installation root. Scores now live in a writable
per-runtime-user state directory, and startup verifies an actual save before
reporting it as available. The corrected package runs and creates/replaces its
score file on the C3 without root. Existing installation-root named and numeric
tables are preserved during import. Owner-confirmed non-empty records survived
a fresh session and the later UI update.

**Back → High Scores** now browses the three saved Original course tables without
finishing another run. Original’s status box is hidden on the opening screen;
after downhill movement it shows the active course, LIVE statistics and a separate
BEST initials/result row. Original ranking and gameplay rules are unchanged.

Score storage rejects incomplete, malformed or incorrectly ranked tables before adopting them. Saves retain the temporary-file/replace approach; failure and interrupted-save states preserve recovery material, disable further writes for that session and show a clear menu warning. Older binaries do not read the new initials file.

Owner playtesting provided positive aggregate input feedback. The source, build, package and TV evidence is recorded in [verification](../native/docs/VERIFICATION.md). Full Windows runtime equivalence and compatibility with every LG TV have not been established. This remains a local candidate; no public release or social post has been sent.


The corrected package passed four Windows suites, four Linux sanitizer suites,
83 native UI cases, six source-boundary tests, seven stage-boundary cases and
six package-permission tests. All 104 staged / 105 packaged files matched.
The old numeric file and the real non-empty named table remain unchanged. Broader
migration routes remain unverified. See the verification record for exact artifact identities and scope.
