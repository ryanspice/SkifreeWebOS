# Security policy

## Supported versions

The current 1.0.x release candidate is supported. Older development builds are historical and do not receive security updates.

## Reporting a vulnerability

Do not open a public issue for a vulnerability. Use GitHub's private vulnerability reporting:

https://github.com/ryanspice/SkifreeWebOS/security/advisories/new

If that route is unavailable, contact the maintainer through https://ryanspice.com/ and ask for a private channel. Do not include credentials, private network details, device serial numbers or original game files in a public report.

Include:

- the app version and IPK SHA-256, if known;
- TV model and firmware only when relevant;
- reproduction steps and the observed impact;
- any suggested fix or mitigation.

## Scope

This project bundles a native SDL2/GLES2 application, 89 original-game sprites and third-party runtime libraries. Reports about the original SkiFree game, Chris Pirih's assets, or LG webOS itself should go to their respective owners. Questions about the bundled reconstruction or artwork rights are covered by [native/NOTICE.md](native/NOTICE.md), not by this security policy.

Reports are acknowledged and handled on a best-effort basis. No bug bounty or payment is offered.
