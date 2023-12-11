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

## Installing

The simplest install method is to copy this plugin folder into your project. If
it is already a C++ project, the Unreal Editor will automatically recognize it
and compile it the next time you open the project.

If you have a pure Blueprint project or want an installation like with plugins
from the Marketplace, the plugin is packaged for Windows hosted development on
[the releases page][releases]. Navigate to the Unreal Engine's Plugin folder
(Program Files > Epic Games > UE_5.3 > Engine > Plugins), and add the plugin
folder there.

[releases]: https://github.com/CAD97/ButtplugUnreal/releases

This plugin is *not* enabled by default. You will need to enable it from the
Plugins window in the editor (Edit > Plugins, IOT category).

## Technical Details

- Features:
  - All communication is mediated through the ButtplugSubsystem.
- Code Modules:
  - Buttplug (ClientOnlyNoCommandlet)
- Number of Blueprints: 0
- Number of C++ Classes: (TODO: docs)
- Network Replicated: No (client only)
- Supported Development Platforms:
  Windows (tested);
  Mac, Linux, LinuxArm64 (supported)
- Supported Target Build Platforms:
  Windows (tested);
  Mac, Linux, LinuxArm64 (supported);
  Android, iOS (probably)
- Documentation: https://github.com/CAD97/ButtplugUnreal/wiki
- Example Project: https://github.com/CAD97/ButtplugUnreal
- Important/Additional Notes:
  - [Butts Are Difficult (Ethics)][Buttplug Ethics]

[Buttplug Ethics]: https://buttplug-developer-guide.docs.buttplug.io/docs/dev-guide/intro/buttplug-ethics

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
