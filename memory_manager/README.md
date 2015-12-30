memory_manager
==============
A library for managing memory generically within the C programming language.  Uses malloc and free exclusively.  In production environments it's extremely important to manage memory such that applications don't run out of memory and fail/crash and also has the ability to collect garbage (wasted memory) similar to the Java garbage collection.

Compilation properties:
- DEBUG=true (will print nothing otherwise)

Configurable properties:
- starting_size
- max_size
- block_size
- growth_factor
- thread_safe (boolean)
- security_policies:
  - checksum (checks block of memory before any read to be same as last write)
  - encrypted (password, cert)
  - random_access (on writes)
- checkpoint_policy:
  - interval (ms)
  - blocking (boolean)
  - types:
    - disk:
      - directory
      - format (timestamped formatting)
    - ram (probably a bad idea)
    - hadoop/cloudbase?
    - ...
