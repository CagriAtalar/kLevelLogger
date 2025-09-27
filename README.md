# kLevelLogger

A logger program running in kernel space.

## Requirements

Install build tools and kernel headers:

```bash
sudo apt-get install build-essential
sudo apt-get install linux-headers-$(uname -r)
```

## Build

Go into the `kLevelLogger` directory and run:

```bash
make
```

## Load module

```bash
sudo insmod keylogger.ko
```

## View logs

```bash
dmesg | tail -n 10
```

(You can change the number depending on how many messages you want.)

If your system uses `journalctl` you can also watch kernel messages with:

```bash
sudo journalctl -k -f
```

## Unload module

```bash
sudo rmmod keylogger
```

## Notes

* Running kernel-space code can crash your system if there are bugs — test inside a VM.
* `keymap` and behavior are based on a simple US QWERTY assumption; different layouts may behave differently.

---

