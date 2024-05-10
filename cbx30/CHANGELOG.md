# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/)
and this project adheres to [Semantic Versioning](http://semver.org/spec/v2.0.0.html).


## [00-06-00] - 2019-11-25
### Removed
- LED and power pin GPIO control

### Fixed
- Ported for crossbox

## [00-05-00] - 2018-09-05
### Added
- In case of power failiure start recovery procedure to retry and burn firmware in stored emmc flash
- Try recovery for `BLOADER_FAIL_RETRY_NUM` (3) times in a row, if not successful go to standby.
- README.md

### Fixed
- Linker script RAM allocation. Do not allocate used magic word and retry counter ram addresses.
