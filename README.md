
<!-- README.md is generated from README.Rmd. Please edit that file -->

# IdentiFly ![](docs/identifly48.png)

<!-- badges: start -->

<!-- badges: end -->

IdentiFly is a semi-automatic classification and landmark digitizing
tool based on geometric morphometrics. It was originally developed for
the identification of honey bee subspecies and other insect taxa, but it
can also be used to classify other biological or man-made objects that
differ in shape.

IdentiFly can be used as an alternative to `tpsDig` for placing
landmarks and semilandmarks in two-dimensional images, and it supports
full classification workflows based on reference configurations and
prototype data.

## Key features

- Landmark and semilandmark digitizing in 2D images
- Open image and directory workflows
- Landmark editing, and outlier detection
- Outline extraction and semilandmark alignment
- Classification based on geometric morphometrics
- Export of coordinates to TPS, CSV, and TXT formats

## Installation

IdentiFly is distributed as a Windows binary package.

1.  Download the ZIP archive:
    [IdentiFly-2_0-win32.zip](https://github.com/DrawWing/IdentiFly/releases/download/v2.0.0/IdentiFly-2_0-win32.zip)
2.  Unzip the folder to a location on your computer
3.  Run `IdentiFly.exe`

The project can also be built from source with Qt Creator on Linux and
macOS.

## Quick workflow

1.  Open an image using `File -> Open`.
2.  Switch to `Mode -> Add landmarks` and place corresponding points.
3.  Use `Select` or `Drag` mode to adjust incorrect landmark positions.
4.  Save the image as a `.dw.png` file.
5.  Set the classification file using
    `Classification -> Set classification`.
6.  Run `Classification -> Classify` for a single image or
    `Classify directory` for a batch.

## Semilandmarks and alignment

IdentiFly supports outline-based semilandmark placement and alignment.
The workflow includes:

- longest outline extraction,
- semilandmarks placed along an outline,
- alignment of semilandmarks in a directory,

## User guide

For a complete overview of the interface, menu structure, workflows,
measurements, and interpretation of classification results, see the
[USER_GUIDE](https://drawwing.github.io/IdentiFly/USER_GUIDE.html).

## Open source

IdentiFly is open-source software released under the GNU General Public
License, Version 3 (GPL-3.0).

## Related publications

Nawrocka A., Kandemir I., Fuchs S., Tofilski A. 2018. Computer software
for identification of honey bee subspecies and evolutionary lineages.
Apidologie 49: 172-184. <https://doi.org/10.1007/s13592-017-0538-y>

Kaur H., Ganie S.A., Tofilski A. 2024. Morphometric identification of an
unknown honey bee colony: an example from north India. Journal of
Apicultural Science 68:155-166. <https://doi.org/10.2478/JAS-2024-0013>
