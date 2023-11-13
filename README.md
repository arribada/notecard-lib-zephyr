# Notecard C Zephyr Module

This repo is a [Zephyr](https://zephyrproject.org/) module required for pulling and building the [note-c](https://github.com/blues/note-c) library to control the [blues.io](https://blues.io) Notecard. Currently the submodule for `note-c` is pinned to the `master` branch.

## Usage

To use this module, add the following to your Zephyr project's `west.yml` manifest file:

```yaml
  projects:
    # Your other modules here
    - name: notecard
      path: modules/notecard
      revision: main
      submodules: true
      url: https://github.com/arribada/lib-notecard-zephyr
```

Then, run `west update` to pull the module into your project.
