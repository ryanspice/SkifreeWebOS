# 1.1.0 plan — optional online high scores

Status: **planned, not implemented**. Updated 2026-11-05. The application and
release tooling remain at 1.0.0. This plan does not add networking to that build.

## Goal and scope

Share completed Original results with a small public leaderboard while keeping
local play, ranking and saving independent of the network. Provide an obvious
**Settings → Online high scores: Off / On** control. Start with anonymous
three-letter initials and the existing Slalom, Tree Slalom and Freestyle tables.

Allow roughly **2–4 working days**, including implementation, host checks,
packaging and a C3 acceptance pass. This is an estimate assuming usable PHP
hosting and a compatible ARM HTTPS client. Hosting or certificate/toolchain
problems can extend it. Account linking and replay verification are later work.

## Player experience

- Online scores default to **Off**, including after upgrading from 1.0.0. No
  endpoint requests, registration, background uploads or online refreshes occur
  until enabled. Offer the setting from the pause menu without a startup modal.
- Explain before enabling: “Share your three-letter initials and completed
  Original results with the public leaderboard.” Link a short privacy notice.
- Keep **Local / Online** views and the three course selectors in High Scores.
  Show cached online data with its last-refresh time, or a clear unavailable
  message. A network problem must not resemble the local save-failure warning.
- Save local results first. Online mode can offer initials for an eligible
  result even when it misses the local top ten; offline behavior stays as it is.
  Upload only after initials are confirmed. Do not silently publish the old
  local score table, including imported scores with `---` initials.
- Show the submitted result and rank separately when it is outside the visible
  top 50. For 1.1.0 show one best result per anonymous installation and course;
  initials are display text and need not be unique. Existing exact ties stay
  ahead, ordered by server acceptance sequence. No seasonal boards initially.
- Turning Off cancels work, stops refresh/retry scheduling, clears unsent
  submissions and invalidates late worker responses. A request already received
  by the server cannot be recalled. Turning Off does not remove published rows;
  expose a separate **Remove my online results** action with an explanation.
- Removal is an explicit online request while sharing is enabled. It deletes
  that installation's public results and pending submissions, then switches
  sharing Off. Handle unavailable-server failures without claiming deletion.

## Hosting decision

Preferred first candidate: the personal-site PHP host, at a proposed stable
`https://new.ryanspice.com/api/skifree/v1/` base URL. Confirm the production host
name before embedding it: the local portfolio repository describes ryanspice.com
as its deployed destination. Do not depend on redirects for POST requests.

The personal-site source already contains PHP. Canopy Digital has PHP API
infrastructure and is an alternative if this becomes a shared games service.
Keep login.ryanspice.com focused on its existing static sign-in launcher; no
account login is required for the first leaderboard. This is a source-level
hosting assessment, not verification of live PHP/database capabilities.

Before implementation, check PHP version, PDO SQLite availability, a database
directory outside the public document root, HTTPS certificate chain, backup and
restore, host rate limiting, and a staging location. SQLite through
[PHP PDO_SQLITE](https://www.php.net/manual/en/ref.pdo-sqlite.php) is the proposed
small-service store. If the host lacks it, record an explicit backend decision
rather than silently changing the stack. Keep server configuration and credentials
on the server; they do not belong in the IPK or source ZIP.

## Service contract to implement

Version the API independently of the app. These routes are a proposed contract,
not existing public endpoints:

- `POST /installations`: create an anonymous random identity and scoped token
  only after opt-in. The token permits submissions and removal for that identity;
  it does not prove that a client or score is genuine. Never use a TV serial,
  MAC address, account identifier or shared secret compiled into the app.
- `POST /scores`: accept a unique random submission ID, course, integer result,
  three uppercase initials, app version and ruleset ID. Validate the schema of
  the client's eligibility metadata; it is a claim, not proof of play. The same
  identity, ID and payload return the original receipt; the same identity and ID
  with a changed payload are rejected.
- `GET /leaderboards?course=slalom&ruleset=original-1.04-standard-v1`: return at
  most 50 ranked best results, server timestamp and schema version. Tree Slalom
  and Freestyle use distinct course values; timed values are integer milliseconds,
  Freestyle values are signed integers. Negative Freestyle scores are valid.
- `GET /submissions/{id}`: authenticate ownership and obtain accepted/rejected
  status and current rank for the retained personal best. A slower new run can
  return “saved personal best unchanged” rather than a misleading new rank.
- `DELETE /installations/me/scores`: authenticate the installation, remove its
  public entries and return a deletion receipt. Define replay/deduplication
  tombstone handling so an old request cannot resurrect a deleted result.

Use strict JSON schemas, prepared database statements, bounded request bodies
and response sizes, fixed course/ruleset enums, transactions, duplicate protection,
per-installation and IP rate limits, and Retry-After responses. Validate limits
against real game results; do not invent a positive-only Freestyle restriction.
Expose stable error codes without stack traces. Add minimal moderation tooling
to remove/disable abusive entries and a backup/restore runbook.

Treat this as a **community leaderboard**: modified clients can invent scores.
Bounds and rate limits discourage abuse; they are not verified gameplay or
anti-cheat. Exclude Recreation/Chase. Track whether Fast Mode, single-step or
coordinate-nudge commands were used at any point in a run, even if later undone;
those runs keep local behavior but cannot enter the standard online board.
QA fixtures cannot submit. Preserve random course generation and document that
these are original generated courses, not a seeded daily time trial.

Publish a minimal data/retention policy before launch: public initials/result/
course, anonymous identity and token handling, short-lived abuse-prevention
metadata, operator access, deletion and backup expiration. Choose explicit
retention periods after checking actual host logging; do not promise anonymity
from the host or zero IP logging without evidence. Store only the current best
per installation/course plus bounded receipts needed for retries.

## Native architecture and persistence

Keep the reconstructed simulation and local score format unchanged. Introduce
small application-layer settings, online-score coordination and transport
modules; avoid growing `main.cpp` into the HTTP client or database adapter.

Use a background HTTPS worker with bounded queues, request timeouts, cancellation
and TLS peer/host verification. Evaluate a pinned libcurl build and its complete
ARM TLS dependency/certificate chain; HTTPS capability depends on its build, as
the [libcurl tutorial](https://curl.se/libcurl/c/libcurl-tutorial.html) describes.
Never disable verification to get the TV working. Audit any new bundled runtime
libraries and licenses through the existing stage/IPK gates.

Persist settings, private installation credentials and pending submissions in
separate versioned files under the existing non-root per-UID state directory.
Protect token files from other app users and redact tokens from logs/receipts.
The current local-score file and legacy import order remain authoritative.
Settings/queue corruption disables online work with a recoverable message;
it must not block valid local saves. Use atomic replacement and isolated tests.

Allocate a durable random submission ID per completed run; the in-memory result
serial is insufficient across launches. Capture eligibility and result at finish,
then enqueue once after confirmed initials and successful durable queue write.
`persist()` is called at startup and exit and must never trigger uploads.

Retry transient failures with bounded exponential backoff/jitter and server
Retry-After, not a request every frame. Stop automatic retry for invalid or
rejected results. Bound queue count/age and disclose expired unsent results.
Handle app pause, suspend, resume, exit and connectivity changes without delaying
rendering or shutdown. Disable/cancel uses a generation token so late responses
cannot re-enable online work or recreate a cleared queue.

## Delivery sequence and acceptance

1. **Hosting and contract:** verify the selected host and ARM HTTPS approach;
   settle retention, removal and moderation; implement/test the service against
   isolated data. Freeze API/ruleset semantics before wiring production calls.
2. **Native core of the feature:** settings, opt-in, eligibility tracking,
   durable IDs/queue and injectable fake transport. Preserve the 1.0.0 score
   format and old saves. Add the worker only after cancellation tests pass.
3. **TV presentation:** Settings, Local/Online tables, initials for online-only
   results, pending/rejected/offline states and explicit removal. Preserve
   controls, pause behavior and the current Original/Recreation distinction.
4. **Candidate:** after staging integration passes, align app title, metadata,
   CMake and release tools at 1.1.0. Package audited dependencies, install the
   1.1.0 candidate on the C3 and verify the full flow before preparing its tag.

Required evidence:

- Existing four core/storage suites and UI regressions still pass. Capture local
  score hashes before/after upgrade; preserve real records rather than seeding
  the TV with QA data. Repeat ASan/UBSan for changed persistence/coordinator code.
- Fake-transport tests prove zero calls while Off, no implicit old-score upload,
  one submission per run, identity/ID/payload conflict behavior, retries across
  relaunch, queue corruption, cancellation races and no queue resurrection.
- Server tests cover ranking/ties, valid negative Freestyle, schema limits,
  duplicate/concurrent submissions, rate limits, authorization, deletion/replay,
  moderation, transactions and backup restore using isolated fixtures.
- UI tests cover pointer/directions/keyboard settings, nonlocal-top-ten initials,
  course switching, cached/offline data and independent local/online warnings.
- C3 evidence covers non-root settings/queue survival, real TLS, disconnect and
  reconnect, suspend/relaunch, cancellation, a human-earned published result,
  explicit removal and unchanged local scores. Test another webOS runtime before
  claiming support for it. Host tests do not establish TV networking behavior.
- Release evidence includes new runtime/license audits, privacy/support docs,
  exact package/source hashes, staging/production URL checks and a rollback that
  leaves the offline game usable. Deploying the service, enabling public uploads
  and publishing 1.1.0 remain distinct release actions.

Account login, cross-device identity recovery, cloud-save synchronization,
historical-score backfill, seeded competitions and verified replays are outside
1.1.0. A public web leaderboard and Champions of Snowboarding link can follow
once the API works; promotional links need not appear in the game’s score flow.
