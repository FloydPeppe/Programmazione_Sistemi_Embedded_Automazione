#!/usr/bin/env python3

# Released by rdb under the Unlicense (unlicense.org)
# Based on information from:
# https://www.kernel.org/doc/Documentation/input/joystick-api.txt

import os, struct, array, socket, time, sys, signal
from fcntl import ioctl

from threading import Thread

# Iterate over the joystick devices.
#print('Available devices:')

#for fn in os.listdir('/dev/input'):
    #if fn.startswith('js'):
        #print('  /dev/input/%s' % (fn))

# We'll store the states here.
axis_states = {}
button_states = {}

# These constants were borrowed from linux/input.h
axis_names = {
    0x00 : 'x',
    0x01 : 'y',
    0x02 : 'z',
    0x03 : 'rx',
    0x04 : 'ry',
    0x05 : 'rz',

}


axis_map = []
button_map = []

# Open the joystick device.
fn = '/dev/input/js0'
#print('Opening %s...' % fn)
try:
    jsdev = open(fn, 'rb')
except:
    print("Disconnected joystick!")
    sys.exit(1)
    
# Get the device name.
#buf = bytearray(63)
buf = array.array('B', [0] * 64)
ioctl(jsdev, 0x80006a13 + (0x10000 * len(buf)), buf) # JSIOCGNAME(len)
js_name = buf.tobytes().rstrip(b'\x00').decode('utf-8')
#print('Device name: %s' % js_name)

# Get number of axes and buttons.
buf = array.array('B', [0])
ioctl(jsdev, 0x80016a11, buf) # JSIOCGAXES
num_axes = buf[0]

buf = array.array('B', [0])
ioctl(jsdev, 0x80016a12, buf) # JSIOCGBUTTONS
num_buttons = buf[0]

# Get the axis map.
buf = array.array('B', [0] * 0x40)
ioctl(jsdev, 0x80406a32, buf) # JSIOCGAXMAP

for axis in buf[:num_axes]:
    axis_name = axis_names.get(axis, 'unknown(0x%02x)' % axis)
    axis_map.append(axis_name)
    axis_states[axis_name] = 0.0

#print('%d axes found: %s' % (num_axes, ', '.join(axis_map)))
#print('%d buttons found: %s' % (num_buttons, ', '.join(button_map)))

# Connect ESP device
os.system("utils/bash/bin/ConnectESP.sh")

# Start TCP communication
TCP_IP = '192.168.4.1'
TCP_PORT = 23

#client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client = socket.socket()
client.settimeout(1);

try:
    client.connect((TCP_IP, TCP_PORT))
except:
    sys.exit(1)
    
class XYjoystick:

    x_curr= []
    y_curr= []
    connected= True
    
    def __init__(obj):
        obj.reset()
    
    def reset(obj):
        obj.x_curr= 512
        obj.y_curr= 512
        
    def do_read(obj):
        while True:
            try:
                evbuf = jsdev.read(8)
            except:
                obj.connected= False
                break
                
            if evbuf:
                tm, value, type, number = struct.unpack('IhBB', evbuf)
            
            if type & 0x02:
                axis = axis_map[number]
                if axis:
                    if axis == "x":
                        obj.x_curr = value
                    if axis == "rz":
                        obj.y_curr = value
    # CTRL_C callback      
    def CTRL_C_handler(obj, signum, frame):
        # Exit script
        sys.exit(0)
        
# Declare object        
XYjs= XYjoystick()

read_thread = Thread(target=XYjs.do_read)

# Set thread daemon, otherwise script hang when Main event loop exit and thread is active
read_thread.daemon= True

read_thread.start()

# Assign CTRL_C callback
signal.signal(signal.SIGINT, XYjs.CTRL_C_handler)


# Main event loop
while True:
    
    if not XYjs.connected:
        print("Disconnected joystick!\n")
        # Exit script
        sys.exit(1)
    
    time.sleep(0.001)

    # Formatting values as CSV string
    output_str= "%.6f,%.6f\n" % (9.9*XYjs.y_curr/2**15,9.9*XYjs.x_curr/2**15)

    print(output_str)

    try:
        client.send(bytes(output_str,"utf-8"))
    except:
        print("Server down\n")
        # Exit script
        sys.exit(1)
'''
        try:
            os.system("utils/bash/bin/ConnectESP.sh")
            client= socket.socket()
            client.settimeout(1);
            client.connect((TCP_IP, TCP_PORT))
        except:
            time.sleep(0.5)
'''            
