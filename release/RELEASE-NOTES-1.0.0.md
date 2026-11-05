# SkiFree for webOS 1.0.0 — local candidate

The opening now reads **WebOS 1.0.0** beneath the crossed-out original 1.04. The original mountain, artwork and attribution remain, with no added lift cables and centered controls.

Original is the default and follows the reconstructed SkiFree 1.04 behavior. Recreation and its gentler Yeti Chase remain separate choices on Green and Yellow; Blue opens help.

Score storage now rejects incomplete, malformed or incorrectly ranked tables before changing the displayed table or writing over a file. Saves retain the existing temporary-file/replace approach; failure and interrupted-save states preserve recovery material, disable further writes for that session and show a clear menu warning. The existing scores-v104.txt format and old prototype record.txt are preserved.

Ryan reported that tested inputs seem good and authorized this local 1.0 candidate under the intended-original-gameplay bar. The final source, build, package and TV evidence belongs in [verification](../native/docs/VERIFICATION.md). This is not a claim of exhaustive EXE runtime equivalence or compatibility with every LG TV. Public release and social posts remain separate from the local candidate.


The local candidate passes four sanitizer suites (118 assertions), 55 native UI cases, seven package-boundary cases and the ARM/package audits. It has been installed and rendered on the C3. The existing empty score file survived update/relaunch; non-empty TV upgrade survival is not yet established. See the verification record for the distinction between host, device and owner evidence.
