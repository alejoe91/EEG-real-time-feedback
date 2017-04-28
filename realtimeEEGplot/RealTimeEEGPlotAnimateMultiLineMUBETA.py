## real time plot animate with multiple values MU + BETA ##

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
muGA = deque()
muc34 = deque()
bGA = deque()
bc34 = deque()


## SET FIGURE

# First set up the figure, the axis, and the plot element we want to animate
fig = plt.figure()
fig.canvas.draw()

xLength = maxLen*3/4;
ax = plt.axes(xlim=(0, xLength), ylim=(-4, 4), xlabel = 'time [s]', ylabel = 'Feedback')
ax.set_axis_bgcolor((0, 0, 0))
plt.setp( ax.get_xticklabels(), visible=False)

line1, = ax.plot([], [], lw=4)
line2, = ax.plot([], [], lw=1)
line3, = ax.plot([], [], lw=4)
line4, = ax.plot([], [], lw=1)
line1.set_color('yellow')
line2.set_color('orange')
line3.set_color('red')
line4.set_color('magenta')

plt.legend([line1,line2,line3,line4],['Grand Average MU','MU C3_C4', 'Grand Average BETA', 'BETA C3_C4'], bbox_to_anchor=(0., 1.02, 1., .102), loc=3,ncol=4, mode="expand", borderaxespad=0.)

"""
t2 = 0
t1 = time.time()
t0 = time.time()
"""

# initialization function: plot the background of each frame
def init():
    line1.set_data([], [])
    line2.set_data([], [])
    line3.set_data([], [])
    line4.set_data([], [])
    
    return line1,line2,line3,line4,



## DEFINE ANIMATION FUNCTION (read from port inside)

# animation function.  This is called sequentially
def animate(i):
     try:
          
          data = s.recv(32);
          val = struct.unpack('4d', data)

          if len(muGA) == 0:
              tt.append(0.5)
              muGA.append(val[0])
              muc34.append(val[1])
              bGA.append(val[2])
              bc34.append(val[3])
                            
          elif len(muGA) < maxLen:
              tt.append(tt[-1] + 0.5)
              muGA.append(val[0])
              muc34.append(val[1])
              bGA.append(val[2])
              bc34.append(val[3])
                               
          else:
              tt.popleft()
              tt.append(tt[-1]+0.5)
              muGA.popleft()
              muc34.popleft()
              bGA.popleft()
              bc34.popleft() 
              muGA.append(val[0])
              muc34.append(val[1])
              bGA.append(val[2])
              bc34.append(val[3])
              ax.set_xlim(tt[0],tt[-1] + xLength/4);
              
          line1.set_data(tt, muGA)
          line2.set_data(tt, muc34)
          line3.set_data(tt, bGA)
          line4.set_data(tt, bc34)
          
          return line1,line2,line3,line4,
            
     except socket.timeout:
          print 'Socket timeout'
          s.close()
          plt.close()
          sys.exit()
    
    

# call the animator.  blit=True means only re-draw the parts that have changed
anim = animation.FuncAnimation(fig, animate, init_func=init, interval = 1, blit=True)

plt.show()
