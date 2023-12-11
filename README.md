# Buttplug (for) Unreal - Example Project

The root folder of this repository is a simple example project using the
Buttplug Unreal plugin, as well as serving as the development environment
for the plugin. For the actual plugin, see [Plugins/ButtplugUnreal](Plugins/ButtplugUnreal)
and [the wiki/docs](https://github.com/CAD97/ButtplugUnreal/wiki).

# Buttplug (for) Unreal

An client implementation of the [Buttplug IoST Protocol][Buttplug] for Unreal
Engine. This adds support for controlling intimate haptic/sensor hardware
(read: sex toys) from Blueprint (preferred) and C++ (possible) by connecting
to a separate server application (typically [Intiface Central].

[Buttplug]: https://buttplug.io/
[Intiface Central]: https://intiface.com/central/

*Please* read the [Buttplug Developer Guide] before implementing Buttplug
support into your game. There are ethical implications to controlling intimate
hardware that should not be ignored, due to how your users interact with it.
This plugin's documentation only aims to describe the client-side *how* of
Buttplug control. The [Buttplug Developer Guide] covers the *what* and *why*.

[Buttplug Developer Guide]: https://buttplug-developer-guide.docs.buttplug.io/docs/dev-guide

## Unreal Marketplace

I did look into packaging and offering this plugin on the Unreal Marketplace,
branded with the "Intiface" name instead of "Buttplug". Per the guidelines:

> **[Marketplace Guidelines], §1.1.e**  
> Epic reserves the right to decline or request changes to content that
> contains sensitive subject matter. This includes, but is not limited to,
> \[...\] sexual content, \[...\]

[Marketplace Guidelines]: https://www.unrealengine.com/en-US/marketplace-guidelines

I did directly ask (and was elevated to senior) Marketplace Support whether
they would be able/willing to accept such a product, and the answer was no
(as of December of 2023). It is *possible* that such a product submission
could sneak through and get approved, but it's not necessary for the plugin
to be usable, and I don't suggest anyone try to dance around that no, either.

The version of the plugin with the Intiface branding instead of Buttplug is
available upon request with evidence of supporting qDot monetarily (e.g. via
[Patreon][qDot#patreon] or [Github Sponsors][qDot#sponsor]). It has the same
BSD license terms as the Buttplug branded version, but I'm suggesting that you
shouldn't purposefully redistribute it in that source code form.

[qDot#patreon]: https://www.patreon.com/qdot
[qDot#sponsor]: https://github.com/sponsors/qdot

## License

This project is licensed under the BSD 3-Clause "New" or "Revised" License
(SPDX-License-Identifier: BSD-3-Clause; see BSD-3-Clause.txt or
https://spdx.org/licenses/BSD-3-Clause.html) or BSD Source Code Attribution
(SPDX-License-Identifier: BSD-Source-Code; see BSD-Source-Code.txt or
https://spdx.org/licenses/BSD-Source-Code.html), at your option.

> [!CAUTION]
This software is provided by the copyright holders and contributors "AS IS"
and any express or implied warranties, including, but not limited to, the
implied warranties of merchantability and fitness for a particular purpose are
disclaimed. In no event shall the copyright holder or contributors be liable
for any direct, indirect, incidental, special, exemplary, or consequential
damages (including, but not limited to, procurement of substitute goods or
services, loss of use, data, or profits; or business interruption) however
caused and on any theory of liability, whether in contract, strict liability,
or tort (including negligence or otherwise) arising in any way out of the use
of this software, even if advised of the possibility of such damage.

TL;DR (not legal advice): you are permitted to redistribute and use this work
in source and binary forms, with or without modification, for personal use or
commercial use, so long as any source code form redistribution of this work
retains this copyright notice and disclaimer.

BSD-3-Clause is offered as it is a standard, reviewed, OSI Approved license.
BSD-Source-Code is offered as it removes the clause requiring reproduction of
the copyright notice in binary distribution forms (i.e. packaged games).

The author believes that it is good practice to credit the authors of 3rd party
assets and code, even if they don't require it, and emplores you to consider if
there's room in your credits scroll to provide that credit. But the author also
acknowledges the norm in the industry, and does not *require* any credit. But(t)
it is greatly appreciated.

Intiface® is a registered trademark of [Nonpolynomial Labs, LLC][Nonpolynomial].
Used with permission.

[Nonpolynomial]: https://nonpolynomial.com/

#### Contribution

Unless you explicitly state otherwise, any contribution intentionally submitted
for inclusion in the work by you shall be dual licensed as above.
