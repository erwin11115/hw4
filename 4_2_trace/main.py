import pyb, sensor, image, time
enable_lens_corr = False
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time = 2000)
clock = time.clock()
uart = pyb.UART(3,9600,timeout_char=1000)
uart.init(9600,bits=8,parity = None, stop=1, timeout_char=1000)
hasFind = 0;
k = 1;
while(k):
   hasFind = 0;
   clock.tick()
   img = sensor.snapshot()
   if enable_lens_corr: img.lens_corr(1.8)
   for l in img.find_line_segments(merge_distance = 10, max_theta_diff = 5):
	  if ( l.y1() < 5 and l.x1() > 50):
		hasFind = 1
		img.draw_line(l.line(), color = (255, 0, 0))
		print(l)
		print_args = (l.theta())
		uart.write(("/goStraight/run 55 \r\n").encode())
		break
   if(hasFind != 1):
	  print("FPS %f" % clock.fps())
	  uart.write(("/stop/run \n").encode())
   k = 1
