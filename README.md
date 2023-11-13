# Notecard C Zephyr Module

This repo is a [Zephyr](https://zephyrproject.org/) module required for pulling and building the [note-c](https://github.com/blues/note-c) library to control the [blues.io](https://blues.io) Notecard. Currently the submodule for `note-c` is pinned to the `master` branch.

## Usage

To get started with this module, you will need to add it to your Zephyr project's `west.yml` manifest file and then add the required overlay to your Zephyr project's device tree.

### Manifest

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

### Overlays

The helper functions are provided from the [note-zephyr](https://github.com/blues/note-zephyr) repo and provide controls to communicate with a Notecard attached to the host via i2c.

As such, the helper functions expect the target device to use a DT alias of `notecard` for the i2c bus.
You should ensure the host device you are targetting has this alias defined in its DT overlay.

```dts
/ {
    aliases {
        // for example, using the i2c0 bus
        notecard = &i2c0;
    };
};
```

### Example

Coming soon.

## License

This repo is licensed under the [MIT License](LICENSE) and uses the following repos:

- [note-c](https://github.com/blues/note-c) - [MIT License]
- [note-zephyr](https://github.com/blues/note-zephyr) - [MIT License]
