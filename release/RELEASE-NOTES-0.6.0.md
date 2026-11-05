# SkiFree for webOS 0.6.0

The TV interface now uses the consistent name **SkiFree for webOS**, with **Original**, **Recreation** and **Yeti Chase** as the run choices. Menus have clearer focus states, quieter colour-button controls, concise instructions and visible original-game credit. Recreation shows when its boost is ready; its pause-menu boost action now resumes play and performs the action.

Original remains the default. Its game core and yeti behaviour are unchanged. The source preservation audit found 67 gameplay functions unchanged from the pinned reconstruction and inspected 22 platform adaptations. The bitmap and executable-table checks pass. These results do not establish perfect executable equivalence; see the [comparison report](../native/docs/ORIGINAL-COMPARISON.md).

Recreation's pursuit is more forgiving:

- Three seconds to get moving before Yeti Chase starts.
- A larger starting gap and lower pursuit speed, with a gentler increase over time.
- A smaller catch radius.
- Protection through a fall and 1.5 seconds after recovery, giving time to steer or boost.

These changes also apply when Recreation's regular slope reaches its 2,000-metre yeti threshold. They do not change Original.

Validation and TV installation evidence are recorded in [verification](../native/docs/VERIFICATION.md). The owner confirmed all four colours in 0.5.0 and reported that 0.6.0's gentler Yeti Chase feels better on the C3. The revised presentation and other input types remain separately unconfirmed. No public release or social post has been sent.
