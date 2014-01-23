import threading
import zmq

context = zmq.Context()
lock = threading.Semaphore()
do_continue = True


def eater():
    global do_continue
    socket = context.socket(zmq.PAIR)
    socket.bind("inproc://a")
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)
    lock.release()
    while do_continue:
        print "Going to poll"
        try:
           socks = dict(poller.poll())
        except zmq.ContextTerminated, e:
            return;
        except Exception, e:
            print e


        if socket in socks:
            message = socket.recv_string()
            print "Received: ", message
            if message != 'add' and message != '':
                print "ERROR: Received bad message: ", message
            socket.send_string("OK")
    print "yellow"
    socket.close()

def feeder():
    socket = context.socket(zmq.PAIR)
    socket.connect("inproc://a")
    for i in range(0,10):
        sender_string = "add"
        print i%2
        if i %2 == 0:
            sender_string = ""
        print "Sending string of size: ", len(sender_string), " for iteration:", i
        socket.send_string(sender_string)

        response = socket.recv_string()
        print "Got response: ", response
    socket.close()


def main():
    global do_continue
    lock.acquire()
    thread1 = threading.Thread(target=eater)
    thread1.start()
    lock.acquire()
    thread2 = threading.Thread(target=feeder)
    thread2.start()
    thread2.join()
    print "------------------------------between 2"
    thread3 = threading.Thread(target=feeder)
    thread3.start()
    thread3.join()
    do_continue = False
    context.term()

if __name__ == "__main__":
    main()
