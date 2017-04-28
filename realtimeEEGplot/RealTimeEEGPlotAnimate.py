### REAL TIME PLOT WITH ANIMATION ###

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

## SET DEQUE PARAM

maxLen = 50
tt = deque()
values = deque()


## SET FIGURE

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
xLength = maxLen*3/4;
ax = plt.axes(xlim=(0, xLength), ylim=(-3, 3), xlabel = 'time [s]', ylabel = 'Feedback')
ax.set_axis_bgcolor((0, 1, 0))
line, = ax.plot([], [], lw=2)
line.set_color('red')

# initialization function: plot the background of each frame
def init():
    line.set_data([], [])
    return line,


## DEFINE ANIMATION FUNCTION (read from port inside)

# animation function.  This is called sequentially
def animate(i):
     try:
          data = s.recv(1024);
          val = struct.unpack('d', data)
          print val
          if len(values) == 0:
              tt.append(0.5)
              values.append(val)
                            
          elif len(values) < maxLen:
              tt.append(tt[-1] + 0.5)
              values.append(val)
                               
          else:
              tt.popleft()
              tt.append(tt[-1]+0.5)
              values.popleft()
              values.append(val)
              ax.set_xlim(tt[0],tt[-1] + xLength/4);
          line.set_data(tt, values)
          return line,   
            
     except socket.timeout:
          print 'Socket timeout'
          s.close()
          plt.close()
          sys.exit()
    
    

# call the animator.  blit=True means only re-draw the parts that have changed
anim = animation.FuncAnimation(fig, animate, init_func=init, interval = 40, blit=True)

plt.show()
