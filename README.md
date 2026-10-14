🔌 Small Router Reseter

A minimalistic watchdog-style project designed to automatically restart a router when Internet connectivity is lost.

The device monitors Internet availability (via ICMP ping) and toggles a relay to power-cycle the router if the connection fails for a specified period. It reports its state over MQTT through a WireGuard tunnel.

The firmware in `reseter/` runs on [JaszczurHAL](https://github.com/jaszczurtd/JaszczurHAL). The assemblies of `reseter/device_tree.json` describe the supported boards:

| Assembly | Board | Relay | Status LED |
| --- | --- | --- | --- |
| `picow` (default) | Raspberry Pi Pico W | GP0 | on-board LED |
| `pico2w` | Raspberry Pi Pico 2 W | GP0 | on-board LED |
| `esp32s3-zero` | Waveshare ESP32-S3-Zero | GPIO1 | on-board WS2812 |

The relay is active-high on every board. Build one assembly with:

```sh
../../libraries/JaszczurHAL/vscode/entry/jh-vscode build --project "$PWD" --assembly pico2w
```

The firmware links the private `Credentials` library from `../../libraries/Credentials`, built for each target: `src/cortex-m0plus/libCredentials.a` for the Pico W and `build/<target>/libCredentials.a` (`rp2350-arm`, `esp32s3`) for the others. The Credentials template of [Ford-Mondeo-MK-DPF-Tracker](https://github.com/jaszczurtd/Ford-Mondeo-MK-DPF-Tracker) builds these archives from your own data.

Ideal for home networks experiencing unstable Internet where a physical restart of the router often solves the issue.

📄 License

This project is licensed under the MIT License – free for personal and commercial use with proper attribution.
