#!/usr/bin/python2

# from here https://www.troyfawkes.com/learn-python-multithreading-queues-basics/

from Queue import Queue
from threading import Thread


def do_stuff(q):
    while True:
        print (q.get())
        q.task_done()

q = Queue(maxsize=0)
num_threads = 10

for i in range(num_threads):
    print 'threading'  
    worker = Thread(target=do_stuff, args=(q,))
    worker.setDaemon(True)
    worker.start()

# insert numbers in the queue
for x in range(3):
    print 'putting', x 
    q.put(x)

q.join()
