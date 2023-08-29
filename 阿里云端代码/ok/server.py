import pic
import tcpforward
import multiprocessing


def main():
    p1 = multiprocessing.Process(target=pic.main_sxt)
    p2 = multiprocessing.Process(target=tcpforward.main_tcp)
    p1.start()
    p2.start()
    #p1.join()
    #p2.join()

if __name__ == "__main__":
    main()
