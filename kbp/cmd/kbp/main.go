package main

import (
	"flag"
	"fmt"
	"io/ioutil"
	"os"
	"strconv"
	"strings"

	"github.com/golang/glog"
	kbp "github.com/ml8/3x3/kbp"
)

var (
	cmd   = flag.String("cmd", "", "one of: ls, deviceinfo, raw")
	reset = flag.Bool("reset", false, "Reset layer text to device-initialized text")
	layer = flag.Int("layer", -1, "Layer number (0-3) to update text for")
	text  = flag.String("text", "", "Text to set for the given layer")
	dev   = flag.String("device", ":::::", "Name of device as reported by -cmd=ls")
	fn    = flag.String("file", "", "name of file to read; alt. use stdin")
	oled  = flag.String("oled", "", "Turn oled on/off; value must be \"on\" or \"off\"")
	hi    = flag.Bool("hi", false, "Debug: hello message")
	echo  = flag.String("echo", "", "Text to echo")
)

func usage() {
	exe, _ := os.Executable()
	fmt.Printf(`
To program layer text:
	%[1]v -layer LAYER_NUM -text TEXT
	
	Set the layer text to TEXT for the given LAYER_NUM. 
	Use \\n for new lines; text will not wrap otherwise.

To reset layer text to device-default text:
	%[1]v -reset

To turn oled off/on:
	%[1]v -oled on|off

For raw programming and other utilities:
	%[1]v -cmd=[ls,deviceinfo,prog]

To use echo/hello debug functions:
	%[1]v -echo TEXT
	%[1]v -hi

To specific a device (for all commands), supply a device string:
	%[1]v -device DEVICE_STRING

	DEVICE_STRING format - VENDOR_NAME:PRODUCT_NAME:VENDOR_ID:PRODUCT_ID:USAGE_ID:USAGE_PAGE

	Any field above may be empty, e.g., "::6d6c:3333::" specifies a vendor and product id only,
	whereas "Marion Lang:ml8_9:::0001:" specifies vendor and product name as well as usage id.
	Exact matches are required for any populated field. All ID/PAGE fields must be base 16.
	By default: usage id and usage page set to generic hid interface for QMK devices, if you 
	wish to use other id/pages, or any, use 0 for both.
`, exe)
}

func ls() {
	kbp.Ls()
}

func parse(dev string) *kbp.DeviceQueryParams {
	props := strings.Split(dev, ":")
	if len(props) != 6 {
		glog.Errorf("Incorrect query string %s, parsed %d fields", dev, len(props))
		return nil
	}
	d := kbp.NewQueryParams()
	if props[0] != "" {
		d.VendorName = props[0]
	}
	if props[1] != "" {
		d.ProductName = props[1]
	}
	if props[2] != "" {
		t, _ := strconv.ParseUint(props[2], 16, 16)
		d.VendorID = uint16(t)
	}
	if props[3] != "" {
		t, _ := strconv.ParseUint(props[3], 16, 16)
		d.ProductID = uint16(t)
	}
	if props[4] != "" {
		t, _ := strconv.ParseUint(props[4], 16, 16)
		d.UsageID = uint16(t)
	}
	if props[5] != "" {
		t, _ := strconv.ParseUint(props[5], 16, 16)
		d.UsagePage = uint16(t)
	}
	glog.Infof("Using query params: %s", d)
	return d
}

func deviceinfo(dev string) {
	infos, e := kbp.Query(parse(dev))
	if e != nil {
		fmt.Printf("No device with spec %s found; use cmd=ls\n", dev)
		return
	}
	for _, info := range infos {
		fmt.Printf("%s\n", info)
	}
}

func getDev(dev string) (device kbp.DeviceInfo, err error) {
	devs, err := kbp.Query(parse(dev))
	if len(devs) != 1 || err != nil {
		fmt.Printf("No unique device found; found %d devices with spec %s. Errors: %v", len(devs), dev, err)
		usage()
		return
	}
	device = devs[0]
	return
}

func raw(dev string) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	data, e := readData()
	if e != nil {
		fmt.Println("Error reading data", e)
		return
	}
	resp, e := kbp.SendRaw(device, data)
	if e != nil {
		fmt.Println("Error sending data", e)
	} else {
		fmt.Println("OK")
		fmt.Printf("Response: %v\n", string(resp))
	}
}

func program(dev string, layer int, text string) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	fmt.Printf("Programming layer %d with %s\n", layer, text)
	text = strings.ReplaceAll(text, "\\n", "\n")
	err = kbp.SendLayerUpdate(device, uint8(layer), text)
	if err != nil {
		fmt.Println("Error programming layer data", err)
	} else {
		fmt.Println("OK")
	}
}

func resetOled(dev string) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	fmt.Printf("Resetting OLED text\n")
	err = kbp.SendLayerReset(device)
	if err != nil {
		fmt.Printf("Error resetting layer text: %v", err)
	} else {
		fmt.Println("OK")
	}
}

func hello(dev string) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	fmt.Printf("Sending hello message\n")
	err = kbp.SendHello(device)
	if err != nil {
		fmt.Printf("Error sending hello: %v", err)
	} else {
		fmt.Println("OK")
	}
}

func ping(dev string, txt string) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	fmt.Printf("Sending echo message\n")
	err = kbp.SendEcho(device, txt)
	if err != nil {
		fmt.Printf("Error sending echo: %v", err)
	} else {
		fmt.Println("OK")
	}
}

func toggleOled(dev string, on bool) {
	device, err := getDev(dev)
	if err != nil {
		return
	}
	fmt.Printf("Sending oled message, on = %v\n", on)
	err = kbp.SendOledState(device, on)
	if err != nil {
		fmt.Printf("Error sending oled state toggle: %v", err)
	} else {
		fmt.Println("OK")
	}
}

func readData() (bytes []byte, e error) {
	if *fn != "" {
		bytes, e = os.ReadFile(*fn)
		if e != nil {
			return
		}
	} else {
		bytes, e = ioutil.ReadAll(os.Stdin)
		if e != nil {
			return
		}
	}
	return
}

func main() {
	flag.Parse()
	kbp.Init()
	defer kbp.Exit()

	if *reset {
		resetOled(*dev)
		return
	}

	if *oled != "" && !(*oled == "on" || *oled == "off") {
		fmt.Printf("Invalid value for -oled")
		usage()
		return
	} else if *oled != "" {
		on := true
		if *oled == "off" {
			on = false
		}
		toggleOled(*dev, on)
		return
	}

	if *hi {
		hello(*dev)
		return
	}

	if *echo != "" {
		ping(*dev, *echo)
		return
	}

	if *layer != -1 || *text != "" {
		if *layer == -1 || *text == "" {
			fmt.Printf("When programming layer text, both -layer and -text must be supplied.\n")
			usage()
			return
		}
		if *layer < 0 || *layer > 3 {
			fmt.Printf("Device only supports 4 layers numbered 0-4\n")
			return
		}
		program(*dev, *layer, *text)
		return
	}

	// other tools
	switch *cmd {
	case "ls":
		ls()
		break
	case "deviceinfo":
		deviceinfo(*dev)
		break
	case "raw":
		raw(*dev)
	default:
		usage()
	}
}
