package kbp

import (
	"errors"
	"fmt"

	"github.com/golang/glog"
	hid "github.com/sstallion/go-hid"
)

var (
	NoUniqueDeviceFound = errors.New("Device is not unique")
	NoDeviceFound       = errors.New("No such device")
)

type DeviceQueryParams struct {
	UsageID     uint16
	UsagePage   uint16
	VendorID    uint16
	ProductID   uint16
	ProductName string
	VendorName  string
}

func (d *DeviceQueryParams) String() string {
	return fmt.Sprintf("(vendor name/id: %s/%04x, product name/id: %s/%04x, usage id/page %04x/%04x)",
		d.VendorName, d.VendorID, d.ProductName, d.ProductID, d.UsageID, d.UsagePage)
}

func Init() error {
	e := hid.Init()
	glog.Infof("Initializing hid, error: %v", e)
	return e
}

func Ls() {
	fmt.Println("path: ID vendorid productid manufacturer devicename")
	hid.Enumerate(hid.VendorIDAny, hid.ProductIDAny, func(info *hid.DeviceInfo) error {
		fmt.Printf("%s: ID %04x:%04x-%d %s mfr: \"%s\" prod: \"%s\" (s/n: %s) (iface no: %d, usage id: %04x, usage page: %04x)\n",
			info.Path,
			info.VendorID,
			info.ProductID,
			info.ReleaseNbr,
			info.BusType.String(),
			info.MfrStr,
			info.ProductStr,
			info.SerialNbr,
			info.InterfaceNbr,
			info.Usage,
			info.UsagePage)
		return nil
	})
}

type DeviceInfo struct {
	hid.DeviceInfo
}

func openDevice(device DeviceInfo) (dev *hid.Device, err error) {
	glog.Infof("Using device %v", device)
	devs, err := openDevices(device)
	glog.Infof("Found %d devices", len(devs))
	if err != nil {
		return
	}
	if len(devs) > 1 {
		err = NoUniqueDeviceFound
		return
	}
	if len(devs) == 0 {
		err = NoDeviceFound
		return
	}
	dev, err = hid.OpenPath(devs[0].Path)
	return
}

func (d DeviceInfo) String() string {
	return fmt.Sprintf("Vendor: %s/%04x, Product: %s/%04x, Usage id/page: %04x/%04x, Path: %s", d.MfrStr, d.VendorID, d.ProductStr, d.ProductID, d.Usage, d.UsagePage, d.Path)
}

func from(i []*hid.DeviceInfo) []DeviceInfo {
	result := make([]DeviceInfo, 0, len(i))
	for _, dev := range i {
		result = append(result, DeviceInfo{*dev})
	}
	return result
}

func NewQueryParams() *DeviceQueryParams {
	return &DeviceQueryParams{
		VendorID:    hid.VendorIDAny,
		ProductID:   hid.ProductIDAny,
		UsageID:     DefaultUsageID,
		UsagePage:   DefaultUsagePage,
		VendorName:  "",
		ProductName: "",
	}
}

func (p *DeviceQueryParams) WithProductID(pid uint16) *DeviceQueryParams {
	p.VendorID = pid
	return p
}
func (p *DeviceQueryParams) WithVendorID(vid uint16) *DeviceQueryParams {
	p.VendorID = vid
	return p
}
func (p *DeviceQueryParams) WithUsageID(usage uint16) *DeviceQueryParams {
	p.UsageID = usage
	return p
}
func (p *DeviceQueryParams) WithUsagePage(usage uint16) *DeviceQueryParams {
	p.UsagePage = usage
	return p
}

func (p *DeviceQueryParams) match(d *hid.DeviceInfo) bool {
	if p.ProductID != hid.ProductIDAny && p.ProductID != d.ProductID {
		// product doesn't match
		return false
	}
	if p.VendorID != hid.VendorIDAny && p.VendorID != d.VendorID {
		// vendor doesn't match
		return false
	}
	if p.UsageID != 0 && p.UsageID != d.Usage {
		// usage id doesn't match
		return false
	}
	if p.UsagePage != 0 && p.UsagePage != d.UsagePage {
		// usage page doesn't match
		return false
	}
	if p.ProductName != "" && p.ProductName != d.ProductStr {
		// product doesn't match
		return false
	}
	if p.VendorName != "" && p.VendorName != d.MfrStr {
		// vendor doesn't match
		return false
	}
	return true
}

func openDevices(d DeviceInfo) (results []*hid.DeviceInfo, e error) {
	q := &DeviceQueryParams{
		VendorName:  d.MfrStr,
		ProductName: d.ProductStr,
		VendorID:    d.VendorID,
		ProductID:   d.ProductID,
		UsageID:     d.Usage,
		UsagePage:   d.UsagePage,
	}
	return openQuery(q)
}

func openQuery(p *DeviceQueryParams) (results []*hid.DeviceInfo, e error) {
	glog.Infof("Querying with %v", p)
	results = make([]*hid.DeviceInfo, 0)
	hid.Enumerate(p.VendorID, p.ProductID, func(info *hid.DeviceInfo) error {
		if p.match(info) {
			results = append(results, info)
			glog.Infof("Found device... %04x:%04x\n", info.VendorID, info.ProductID)
		}
		return nil
	})
	return
}

func Query(p *DeviceQueryParams) (d []DeviceInfo, e error) {
	dev, e := openQuery(p)
	if dev != nil {
		d = from(dev)
	}
	return
}

func Exit() error {
	e := hid.Exit()
	glog.Infof("Shutting down hid, error: %v", e)
	return e
}
