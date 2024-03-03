package kbp

import (
	"errors"
	"fmt"
	"time"

	"github.com/golang/glog"
	hid "github.com/sstallion/go-hid"
)

var (
	UnsupportedCommand = errors.New("Unsupported command")
	TransferAborted    = errors.New("Transfer aborted")
)

const (
	DefaultUsageID   = 0x0061
	DefaultUsagePage = 0xff60
)

const (
	CMD_NOOP     = 0x00
	CMD_ERR      = 0x01
	CMD_ACK      = 0x02
	CMD_NACK     = 0x03
	CMD_CONT     = 0x04
	CMD_ABORT    = 0x05
	CMD_COMPLETE = 0x06

	// Debug commands; hello and echo
	CMD_HELLO = 0x30 // 0
	CMD_ECHO  = 0x31

	// Control commands
	CMD_OLED_OFF = 0x40 // @
	CMD_OLED_ON  = 0x41

	// OLED programming commands
	CMD_OLED_UPDATE = 0x50 // P
	CMD_OLED_RESET  = 0x51
)

func prepareMessage(buffer []byte, cmd uint8, data []byte) {
	buffer[0] = 'm'
	buffer[1] = 'l'
	buffer[2] = cmd
	if len(data) > 0 {
		copy(buffer[3:], data)
	}
}

func prepareLayerMessage(buffer []byte, data []byte, cmd uint8, layer uint8, length uint8) {
	prepareMessage(buffer, cmd, nil)
	buffer[3] = layer
	buffer[4] = length
	if length > 0 {
		copy(buffer[5:], data[:length])
	}
}

func prepareStartMessage(buffer []byte, data []byte, cmd uint8, layer uint8, length uint8) {
	prepareLayerMessage(buffer, data, cmd, layer, length)
}

func prepareContinueMessage(buffer []byte, data []byte, layer uint8, length uint8) {
	prepareLayerMessage(buffer, data, CMD_CONT, layer, length)
}

func prepareCompleteMessage(buffer []byte, layer uint8) {
	prepareLayerMessage(buffer, nil, CMD_COMPLETE, layer, 0)
}

func handleAckOrNack(dev *hid.Device) (response []byte, err error) {
	recv := make([]byte, 32)
	got, err := dev.ReadWithTimeout(recv, time.Duration(10*time.Second))
	glog.Infof("Received message: %v", recv)
	switch {
	case got <= 0:
		glog.Infof("Got <=0 response (%d); err: %v", got, err)
		err = TransferAborted
		return
	case err != nil:
		glog.Errorf("Got error %v", err)
		err = TransferAborted
		return
	case recv[0] != CMD_ACK:
		glog.Errorf("Got response code %v", recv[0])
		err = TransferAborted
		return
	default:
		glog.Infof("Got ACK")
		response = recv
	}
	return
}

func sendSegmented(dev *hid.Device, cmd uint8, layer uint8, data []byte) error {
	buf := make([]byte, 32)

	switch cmd {
	case CMD_OLED_UPDATE:
		glog.Infof("Sending OLED update")
	default:
		return UnsupportedCommand
	}

	var tot uint8 = uint8(len(data))
	var l uint8 = 0
	var err error
	isStart := true

	for tot > 0 {
		glog.Infof("Writing %s", data)

		l = tot
		if tot > 25 {
			// 2 byte header, 1 byte command, 1 byte layer, 1 byte length
			l = 25
			glog.Infof("Chunking %d of %d\n", l, tot)
		} else {
			glog.Infof("No chunking necessary, total bytes %d\n", tot)
		}

		if isStart {
			prepareStartMessage(buf, data[:l], CMD_OLED_UPDATE, layer, l)
		} else {
			prepareContinueMessage(buf, data[:l], layer, l)
		}
		data = data[l:]
		tot -= l

		glog.Infof("Sending %v", buf)
		_, err = dev.Write(buf)
		isStart = false
		if err != nil {
			return err
		}

		_, err = handleAckOrNack(dev)
		if err != nil {
			return err
		}
		glog.Infof("Wrote %d; %d bytes remaining", l, tot)
	}
	// Prepare and send completion message.
	prepareCompleteMessage(buf, layer)
	glog.Infof("Sending completion message %v", buf)
	dev.Write(buf)
	_, err = handleAckOrNack(dev)
	if err != nil {
		return err
	}

	return nil
}

func sendCommand(device DeviceInfo, buffer []byte) error {
	glog.Infof("Sending %x: %v", buffer[2], buffer)
	_, err := SendRaw(device, buffer)
	if err != nil {
		return err
	}
	return nil
}

func SendLayerReset(device DeviceInfo) error {
	buffer := make([]byte, 32)
	prepareMessage(buffer, CMD_OLED_RESET, nil)
	return sendCommand(device, buffer)
}

func SendOledState(device DeviceInfo, on bool) error {
	buffer := make([]byte, 32)
	var cmd uint8 = CMD_OLED_OFF
	if on {
		cmd = CMD_OLED_ON
	}
	prepareMessage(buffer, cmd, nil)
	return sendCommand(device, buffer)
}

func SendHello(device DeviceInfo) error {
	buffer := make([]byte, 32)
	prepareMessage(buffer, CMD_HELLO, nil)
	glog.Infof("Sending hello message %v", buffer)
	if resp, err := SendRaw(device, buffer); err != nil {
		return err
	} else {
		// resp should be: < ACK HELLO message >
		txt := string(resp[2:])
		fmt.Printf("Got response: %s\n", txt)
	}
	return nil
}

func SendEcho(device DeviceInfo, txt string) error {
	buffer := make([]byte, 32)
	// send buffer will be < m l ECHO txt >, so txt has to be < 29 bytes.
	bytes := []byte(txt)
	if len(bytes) > 29 {
		bytes = bytes[:29]
	}
	prepareMessage(buffer, CMD_ECHO, []byte(txt))
	glog.Infof("Sending echo message %v", buffer)
	if resp, err := SendRaw(device, buffer); err != nil {
		return err
	} else {
		// resp should be: < ACK message >
		echo := string(resp[1:])
		fmt.Printf("Got response: %s\n", echo)
	}
	return nil
}

func SendLayerUpdate(device DeviceInfo, layer uint8, txt string) error {
	dev, err := openDevice(device)
	if err != nil {
		return err
	}
	defer dev.Close()
	return sendSegmented(dev, CMD_OLED_UPDATE, layer, []byte(txt))
}

func SendRaw(device DeviceInfo, data []byte) (response []byte, err error) {
	dev, err := openDevice(device)
	defer dev.Close()
	if err != nil {
		return
	}
	t, err := dev.Write(data)
	if err != nil {
		return
	}
	glog.Infof("Sent %d bytes", t)
	response, err = handleAckOrNack(dev)
	return
}
