# TCP redirection tool
The TCP traffic redirecting tool. This project utilizes [eBPF](https://ebpf.io/) to monitor the traffic on a machine. Once the traffic is supposed to be sent and receive on the local machine via TCP, it will be redirected on socket level. Since the travelling of that packet will not go all the way down to lower network stack level, it will perform a faster message exchange as a result.

## Environment
| Name   | Description      |
| ---    | ---              |
| Device | Raspberry Pi 5   |
| OS     | Ubuntu 23.10     |
| kernel | 6.5.0-1005-raspi |

## Requirements
- fundamental
    ```
    $ apt install -y make clang llvm 
    ```

- bcc
    ```
    $ apt install bpfcc-tools linux-headers-$(uname -r)
    ```

- libbpf
    ```
    $ apt install libbpf-dev
    ```

- bpftool
    ```
    $ apt install linux-tools-common
    ```
