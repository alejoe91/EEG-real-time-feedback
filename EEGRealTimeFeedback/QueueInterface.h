/*
EEGRealTime Feedback

Alessio Buccino
*/

#ifndef QUEUEINTERFACE_H
#define QUEUEINTERFACE_H

#include<stdlib.h>
#include <iostream>


template <class T>

class QueueInterface  {

	virtual bool enqueue(T data) = 0;
	virtual T dequeue() = 0;
	virtual T getEntry(int index) = 0;
	virtual bool isEmpty() = 0;
	virtual bool isFull() = 0;

};
#endif