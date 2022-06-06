Palette Icon Engine
===================

First of all, this is NOT usual developer library, and it should NOT be linked to app somehow. This library is a Qt plugin implementing just another icon engine.

This plugin provides palette-based icon engine, i.e. color of generated icons depends on system/app palette. Only SVG images can be used as source icon.

Why
---

Initially this plugin is appeared as part of [Digital Clock 4](https://github.com/Kolcha/DigitalClock4) just to make some icons used in settings dialog have the same color as text (depending on used theme), to get good contrast on dark themes on Linux, also to make overall app look better. Look at the [screenshots](https://digitalclock4.sourceforge.io/) to see the effect.

How to use
----------

First of all, build it as any other Qt app/library and **install** (`make install`). Installed plugin goes into `<path to Qt>/plugins/iconengines/` (where `svgicon` plugin is located). Build/installation process should be done for any Qt version you are interested in. Correct work is not guaranteed if plugin is used with different Qt version rather it was compiled.

To draw some icons in app using this engine, just change corresponding icons files extension from '*.svg*' to one of next supported: '*.p' '.pal' '.pie'*. And then create *QIcon* object as usual:

```cpp
QIcon icon(":/icons/icon.svg.p");
```

Renamed icons also can be picked from Designer.

It is not strictly necessary to rename files (or assign aliases) to have one of supported extensions, it is enough "just request" such file (i.e. provide filename with desired extension, but NO such file exists) on `QIcon` creation. In such case, plugin will remove any supported extension and will use .svg file as is. This approach may be very convenient when you want to use the same image in several places but in different ways (only in some cases image must be customized).

How it works
------------

This engine is very simple - internally it uses [`QSvgRenderer`](https://doc.qt.io/qt-5/qsvgrenderer.html) to draw original SVG file and then just "colorize" it according to palette.

[`QPalette::WindowText`](https://doc.qt.io/qt-5/qpalette.html) value is used as color source, enabled/disabled state is also respected.

Environment
-----------

Tested with Qt 5.9 and above, but should work even with older versions. It even doesn't require any modern C++ features.

Tested and works fine in Linux (with different DE), Windows, macOS. It is maybe not so useful in Windows and macOS, but it should be shipped with an app, because renamed icons are not handled correctly by default [`QSvgIconEngine`](https://code.qt.io/cgit/qt/qtsvg.git/tree/src/plugins/iconengines/svgiconengine).
