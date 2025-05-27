# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Security
- TODO : add secure wiping to clear function of ByteArray

### Added
- Added an Equality comparison overload to ByteArray
- added empty() member function to ByteArray
- added create_from_string(std::string_view) factory function
- Added as_hex_string utility for displaying the byte array as a hex string


### Fixed
- Added template restrictions for iterator constructor to avoid ambiguity between constructors

### Changed
- Updated Readme.md with examples
- Reorganized examples for better visibility



## [0.1.0-alpha.1]

### Added
- Added a new copy constructor that copies only n bytes as given in the constructor itself

### Changed
- Enhanced resize functionality with security options for purging and warnings
- Updated Readme.md with new examples


## [0.0.4] - 2025-05-21

### Added
- Clear utility for emptying the byte array
- resize utility for resizing the byte array
- Added byte-array concatenation feature
- Added prng byte-array generator factory function

### Fixed
- fixed issue with in-place complementation

## [0.0.3] - 2025-05-16

### Added
- One's complement operator (`operator~`) that flips all bits in a ByteArray
- Unit tests for complement operations
- Updated documentation with examples of complement operator usage

## [0.0.2] - 2025-05-13

### Added
- Secure memory wiping for sensitive data
- XOR operation with proper alignment semantics
- Multiple construction methods (hex strings, byte vectors, initializer lists)
- Conversion between ByteArray and uint64_t
- Comprehensive test suite for core functionality

### Fixed
- Memory handling for copy and move operations
- Bounds checking for array access

## [0.0.1] - 2025-04-21

### Added
- Initial implementation of ByteArray class
- Basic byte array construction and manipulation
- Core memory management functionality
- Project scaffolding and build system