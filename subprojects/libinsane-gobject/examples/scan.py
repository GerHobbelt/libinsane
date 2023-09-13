#!/usr/bin/env python3

# source ./activate_test_env.sh
# subprojects/libinsane-gobject/examples/scan.py

import PIL.Image
import sys
import traceback

import gi
gi.require_version('Libinsane', '0.1')
from gi.repository import GObject  # noqa: E402


from gi.repository import Libinsane  # noqa: E402


#! [ExampleLogger]
class ExampleLogger(GObject.GObject, Libinsane.Logger):
    def do_log(self, lvl, msg):
        if lvl < Libinsane.LogLevel.ERROR:
            return
        print("{}: {}".format(lvl.value_nick, msg))
#! [ExampleLogger]


#! [ExampleListGetDevice]
def get_device(api, dev_id):
    print("Looking for scan devices ...")
    if dev_id is None:
        devs = api.list_devices(Libinsane.DeviceLocations.ANY)
        print("Found {} devices".format(len(devs)))
        for dev in devs:
            print("[{}] : [{}]".format(dev.get_dev_id(), dev.to_string()))
        dev_id = devs[0].get_dev_id()
    print("Will use device {}".format(dev_id))
    dev = api.get_device(dev_id)
    print("Using device {}".format(dev.get_name()))
    return dev
#! [ExampleListGetDevice]


#! [ExampleListGetScanSource]
def get_source(dev, source_name):
    print("Looking for scan sources ...")
    sources = dev.get_children()
    print("Available scan sources:")
    for src in sources:
        print("- {}".format(src.get_name()))
        if src.get_name() == source_name:
            source = src
            break
    else:
        if source_name is None:
            source = sources[0] if len(sources) > 0 else dev
        elif source_name == "root":
            source = dev
        else:
            print("Source '{}' not found".format(source_name))
            sys.exit(2)
    print("Will use scan source {}".format(source.get_name()))
    return source
#! [ExampleListGetScanSource]


#! [ExampleSetOption]
def set_opt(item, opt_name, opt_value):
    try:
        print("Setting {} to {}".format(opt_name, opt_value))
        opts = item.get_options()
        opts = {opt.get_name(): opt for opt in opts}
        if opt_name not in opts:
            print("Option '{}' not found".format(opt_name))
            return
        print("- Old {}: {}".format(opt_name, opts[opt_name].get_value()))
        print("- Allowed values: {}".format(opts[opt_name].get_constraint()))
        opts[opt_name].set_value(opt_value)
        opts = item.get_options()
        opts = {opt.get_name(): opt for opt in opts}
        print("- New {}: {}".format(opt_name, opts[opt_name].get_value()))
    except Exception as exc:
        print("Failed to set {} to {}: {}".format(
            opt_name, opt_value, str(exc)
        ))
        traceback.print_exc()
    finally:
        print("")
#! [ExampleSetOption]


#! [ExampleListOptions]
def list_opts(item):
    opts = item.get_options()
    print("Options:")
    for opt in opts:
        try:
            print("- {}={} ({})".format(
                opt.get_name(), opt.get_value(), opt.get_constraint()
            ))
        except Exception as exc:
            print("Failed to read option {}: {}".format(
                opt.get_name(), str(exc)
            ))
    print("")
#! [ExampleListOptions]


#! [ExampleRawToImg]
def raw_to_img(params, img_bytes):
    fmt = params.get_format()
    if fmt == Libinsane.ImgFormat.RAW_RGB_24:
        (w, h) = (
            params.get_width(),
            int(len(img_bytes) / 3 / params.get_width())
        )
        mode = "RGB"
    elif fmt == Libinsane.ImgFormat.GRAYSCALE_8:
        (w, h) = (
            params.get_width(),
            int(len(img_bytes) / params.get_width())
        )
        mode = "L"
    elif fmt == Libinsane.ImgFormat.BW_1:
        assert()  # TODO
    else:
        assert()  # unexpected format
    print("Mode: {} : Size: {}x{}".format(mode, w, h))
    return PIL.Image.frombuffer(mode, (w, h), img_bytes, "raw", mode, 0, 1)
#! [ExampleRawToImg]


#! [ExampleScan]
def scan(source, output_file):
    session = source.scan_start()

    scan_params = source.get_scan_parameters()
    print("Expected scan parameters: {} ; {}x{} = {} bytes".format(
          scan_params.get_format(),
          scan_params.get_width(), scan_params.get_height(),
          scan_params.get_image_size()))
    total = scan_params.get_image_size()

    try:
        page_nb = 0
        while not session.end_of_feed() and page_nb < 20:
            img = []
            r = 0
            if output_file is not None:
                out = output_file.format(page_nb)
            else:
                out = None
            print("Scanning page {} --> {}".format(page_nb, out))
            while True:
                data = session.read_bytes(32 * 1024)
                data = data.get_data()
                img.append(data)
                r += len(data)
                print("Got {} bytes => {}/{} bytes".format(
                    len(data), r, total)
                )
                if session.end_of_page():
                    break
            img = b"".join(img)
            img = raw_to_img(scan_params, img)
            if out is not None:
                img.save(out)
            page_nb += 1
        if page_nb == 0:
            print("No page in feeder ?")
    finally:
        session.cancel()
#! [ExampleScan]


def main():
#! [ExampleSetLogger]
    Libinsane.register_logger(ExampleLogger())
#! [ExampleSetLogger]

    if len(sys.argv) > 1 and (sys.argv[1] == "-h" or sys.argv[1] == "--help"):
        print(
            "Syntax: {}"
            " [<output file>]"
            " [<scan dev id> [<scan source name>]]".format(sys.argv[0])
        )
        sys.exit(1)

    output_file = sys.argv[1] if len(sys.argv) > 1 else None
    dev_id = None
    source = None
    source_name = None
    if len(sys.argv) > 2:
        dev_id = sys.argv[2]
    if len(sys.argv) > 3:
        source_name = sys.argv[3]

    print("Will write the scan result into {}".format(output_file))

#! [ExampleInit]
    api = Libinsane.Api.new_safebet()
#! [ExampleInit]

    dev = get_device(api, dev_id)
    source = get_source(dev, source_name)

    # set the options
#! [ExampleOptsToSet]
    set_opt(source, 'resolution', 300)
#! [ExampleOptsToSet]

    list_opts(source)

    print("Scanning ...")
    scan(source, output_file)
    print("Scan done")


if __name__ == "__main__":
    main()
