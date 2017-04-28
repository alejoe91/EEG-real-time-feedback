import matplotlib.pyplot as plt
import matplotlib.animation as animation
import socket,sys,struct,time

from collections import deque

### SETUP SOCKET CONNECTION ###

server_address = ('localhost', 8888)
     
s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
print 'Socket created'

s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
s.settimeout(15)
     
#Bind socket to local host and port
try:
    s.bind((server_address))
except socket.error as msg:
    print 'Bind failed. Error Code : ' + str(msg[0]) + ' Message ' + msg[1]
    sys.exit()
         
print 'Socket bind complete'


### SETUP DEQUEUE TO UPDATE AND PLOT DATA ###

maxLen = 200

tt = deque()
values = deque()

fig = plt.figure()
ax = plt.axes(xlim=(0, 100), ylim=(-5, 5))
line, = ax.plot([], [], lw=2)

def init():
    line.set_data([], [])
    return line,

"""
#line, = plt.plot(tt,values)
plt.ion()
plt.xlabel('time [s]')
plt.ylabel('Feedback')
plt.show()
"""

 
## Receive and Update plot
try:
    while 1:
        try:
            data = s.recv(1024);
            val = struct.unpack('d', data)
            print val
            if len(values) == 0:
                tt.append(0.5)
                values.append(val)

                plt.plot(tt, values)
                plt.draw()
                #time.sleep(0.1)
                plt.pause(0.001)
                """
                line.set_xdata(tt)
                line.set_ydata(values)
                plt.draw()
                """               
            elif len(values) < maxLen:
                tt.append(tt[-1] + 0.5)
                values.append(val)

                plt.plot(tt, values)
                plt.draw()
                #time.sleep(0.1)
                plt.pause(0.001)
                """
                line.set_xdata(tt)
                line.set_ydata(values)
                plt.draw()
                """               
            else:
                tt.pop()
                tt.append(tt[-1]+0.5)
                values.pop()
                values.append(val)

                plt.plot(tt, values)
                plt.draw()
                #time.sleep(0.1)
                plt.pause(0.001)
                """
                line.set_xdata(tt)
                line.set_ydata(values)
                plt.draw()
                """         
            
        except socket.timeout:
            print 'Socket timeout'
            s.close()
            sys.exit()
except KeyboardInterrupt:
    pass

s.close()
