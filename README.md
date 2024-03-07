# Notecard C Zephyr Module

This repo is a [Zephyr](https://zephyrproject.org/) module required for pulling and building the [note-c](https://github.com/blues/note-c) library to control the [blues.io](https://blues.io) Notecard. 
Currently the submodule for `note-c` tracks the `master` branch.

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
      url: https://github.com/arribada/notecard-lib-zephyr
```

Then, run `west update` to pull the module into your project.

### Overlays

The helper functions are provided from the [note-zephyr](https://github.com/blues/note-zephyr) repo and provide controls to communicate with a Notecard attached to the host via `i2c` or `serial`.

As such, the helper functions expect the target device to use a DT alias of `notecard` for the `i2c` or `serial` bus.
You should ensure the host device you are targeting has this alias defined in its DT overlay.
When using serial, be sure to make the serial peripheral of your host, e.g. `usart0` on the Swan MCU or `uart0` on the ESP32 HUZZAH.

For example, `i2c`:
```dts
// using the i2c0 bus
/ {
    aliases {
        notecard = &i2c0;
    };
};
```

For example, `serial`:
```dts
// using the usart0 bus 
/ {
    aliases {
        notecard = &usart0;
    };
};
```

### Project Config

Ensure your project's `prj.conf` file includes the following:

For `i2c`:
```conf
CONFIG_NOTECARD=y
CONFIG_NOTECARD_I2C=y
```

For `serial`:
```conf
CONFIG_NOTECARD=y
CONFIG_NOTECARD_SERIAL=y
```

> Note: Currently, these options (`i2c` & `serial`) are mutually exclusive as the `note-c` library does not support both protocols simultaneously.

### Example

Included in this repo is a simple example for using the `west` module to send a note using the Notecard. 
See the [example](example/README.md) for more details.

## License

This repo is licensed under the [MIT License](LICENSE) and uses the following repos:

- [note-c](https://github.com/blues/note-c) - [MIT License]
- [note-zephyr](https://github.com/blues/note-zephyr) - [MIT License]
