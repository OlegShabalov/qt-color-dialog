# ColorDialog

## About

`ColorDialog` is a custom Qt color selection dialog. This class duplicates the functionality of the standard `QColorDialog`, but unlike it, it has a simpler and more understandable UI design.

## Usage

Just add the files `ColorDialog.hpp` and `ColorDialog.cpp` to your project and use it like a regular Qt dialog class.

Like the standard `QColorDialog`, this dialog provides a static method `getColor` for getting a color that has a similar interface. But you can use this class directly.

It is also possible to hide buttons or the alpha channel.

The `main.cpp` file provides a simple example of how to use this dialog.

## License

Licensed under the [LGPL Version 3](https://github.com/OlegShabalov/qt-color-dialog/blob/master/LICENSE).

## Screenshots

![Screenshot1: default](https://github.com/OlegShabalov/qt-color-dialog/blob/master/Screenshots/Screenshot1.png "Screenshot1: default")

![Screenshot2: semi-transparent color](https://github.com/OlegShabalov/qt-color-dialog/blob/master/Screenshots/Screenshot2.png "Screenshot2: semi-transparent color")

![Screenshot3: no alpha channel](https://github.com/OlegShabalov/qt-color-dialog/blob/master/Screenshots/Screenshot3.png "Screenshot3: no alpha channel")

![Screenshot4: no buttons](https://github.com/OlegShabalov/qt-color-dialog/blob/master/Screenshots/Screenshot4.png "Screenshot4: no buttons")