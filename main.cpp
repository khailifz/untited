//
// Created by fangz on 2023/4/25.
//
#include <thread>
#include <stdio.h>
#include <iostream>
//#include "BlockingQueue.hpp"
#include "BlockingQueue_uint8.hpp"

using namespace doip_service;

//BlockingQueue<int> bq;

void thread1() {
	BlockingQueue &bq = BlockingQueue::GetInstance();
	std::shared_ptr<std::vector<uint8_t>> buf = std::make_shared<std::vector<uint8_t>>(100,1);

	while (1) {
		bq.Push(buf);
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
}

void thread2() {
	BlockingQueue &bq = BlockingQueue::GetInstance();
	std::shared_ptr<std::vector<uint8_t>> buf;
	//uint8_t i;
	while (1) {
		//int i = bq.Take(std::chrono::milliseconds(100));
		//printf("thread2 get i = %d\n", i);

		//try {
		int ret = bq.Take(buf, std::chrono::milliseconds(1000));
		if (ret == 0){
			//printf("thread2 get i = %d\n", buf->data()[0]);
			for (auto elem : *buf) {
				//std::cout << elem << std::endl;
				std::cout << std::dec << static_cast<int>(elem) << " ";
			}
		} else if (ret == -1) {
			printf("thread2 Take timeout\n");
		}

	}
}

//void thread3() {
//	BlockingQueue<int> &bq = BlockingQueue<int>::GetInstance();
//	while (1) {
//		int i = bq.Take();
//		printf("thread3 get i = %d\n", i);
//	}
//}

int main() {
	std::thread t1(thread1);
	std::thread t2(thread2);
	//std::thread t3(thread3);

	t1.join();
	t2.join();
	//t3.join();

	return 0;
}
