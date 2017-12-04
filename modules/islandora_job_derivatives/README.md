# Islandora Job Derivatives

## Introduction

Sends ingest derivatives to Islandora Job. In the future could do all derivatives.

## Installation

Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.

## Troubleshooting/Issues

There is a technical limitation: this module can’t handle transitive
derivatives on objects other than the one that initiated the derivative chain.
For instance paged content aggregate PDF and OCR datastreams. There are UI
options to prevent these derivatives during ingest and manually run them. In
the same vein 409's may also result if derivatives are kicked off and then
additional processing of an object continues in the parent thread, usually
through object/ds ingested/modified hooks. Programming for thread safety can
be facilitated by only using derivatives and pre-fedora modification hooks.

Having problems or solved a problem? Contact [discoverygarden](http://support.discoverygarden.ca).

## Maintainers/Sponsors

Current maintainers:

* [William Panting](https://github.com/willtp87)

This project has been sponsored by:

* [discoverygarden](http://www.discoverygarden.ca)

## Development

If you would like to contribute to this module, please check out our helpful
[Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers)
info, [Developers](http://islandora.ca/developers) section on Islandora.ca and
contact [discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
