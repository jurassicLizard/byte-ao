# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).


## [Unreleased]

### Added
- Added partial move constructor and corresponding unit tests


## [0.3.0] - 2025-06-01

### Security
- Added a secure memory wiping option to function `clear()`

### Added
- Added a second resize overload with padding direction as second parameter for convenience
- Added comprehensive unit tests for resize functionality and overload equivalence
- Added EZeroPadDir padding direction control to resize operations
- Added MSB_PAD and LSB_PAD support for resize operations

### Changed
- Added documentation for resize functionality
- Enhanced resize functionality to support a configurable padding direction (MSB_PAD/LSB_PAD)
- Updated resize method signature to include EZeroPadDir parameter
- Improved resize behavior with better control over zero-padding placement
- Updated documentation with detailed resize examples and padding direction explanations
- Enhanced function with optional secure purging parameter (`bsecure_purge`) `clear()`
- now supports secure memory wiping before clearing when `bsecure_purge=true` `clear()`


### Fixed
- Improved resize implementation to properly handle padding direction in all scenarios

## [0.2.0] - 2025-05-31

### Added
- Added an Equality comparison overload to ByteArray
- Added empty() member function to ByteArray
- Added create_from_string(std::string_view) factory function
- Added as_hex_string utility for displaying the byte array as a hex string
- Added create_with_prealloc static constructor to preallocate space before creating the byte array
- Added documentation to ByteArray(ByteArray& other, const size_t num_bytes)


### Fixed
- Added template restrictions for iterator constructor to avoid ambiguity between constructors
- Fixed Constructor ByteArray(ByteArray& other, const size_t num_bytes) not padding correctly


### Changed
- Updated Readme.md with examples
- Reorganized examples for better visibility
- Removed `const char *` and replaced it with `std::string_view` in ByteArray hex constructor




## [0.1.0-alpha.1] - 2025-05-23

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