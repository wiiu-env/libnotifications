[![Publish Docker Image](https://github.com/wiiu-env/libnotifications/actions/workflows/push_image.yml/badge.svg)](https://github.com/wiiu-env/libnotifications/actions/workflows/push_image.yml)

# libnotifications
Requires the [NotificationModule](https://github.com/wiiu-env/NotificationModule) to be running via [WUMSLoader](https://github.com/wiiu-env/WUMSLoader).
Requires [wut](https://github.com/devkitPro/wut) for building.
Install via `make install`.

## Usage
Make also sure to define 
```
WUMS_ROOT := $(DEVKITPRO)/wums
```
and add `-lnotifications` to `LIBS` and `$(WUMS_ROOT)` to `LIBDIRS`.

After that you can simply include `<notifications/notifications.h>` to get access to the function patcher functions after calling `NotificationModule_InitLibrary()`.

## Use this lib in Dockerfiles.
A prebuilt version of this lib can found on dockerhub. To use it for your projects, add this to your Dockerfile.
```
[...]
COPY --from=wiiuenv/libnotifications:[tag] /artifacts $DEVKITPRO
[...]
```
Replace [tag] with a tag you want to use, a list of tags can be found [here](https://hub.docker.com/r/wiiuenv/libnotifications/tags). 
It's highly recommended to pin the version to the **latest date** instead of using `latest`.

## Format the code via docker
`docker run --rm -v ${PWD}:/src wiiuenv/clang-format:13.0.0-2 -r ./source ./include -i`
