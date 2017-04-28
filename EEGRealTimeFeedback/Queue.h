/*
EEGRealTime Feedback

Alessio Buccino
*/

#pragma once

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>
#include <iostream>
#include <vector>

using namespace std;

#include "QueueInterface.h"

#define SIZE 5


template<class Type>
class Queue : public QueueInterface < Type > {

private:
	int front;
	int last;
	int length;
	int size;
	Type* queue;

public:
	

	Queue();
	Queue(int newSize);
	~Queue();
	bool enqueue(Type data);
	Type dequeue();
	Type getEntry(int index);
	bool isEmpty();
	bool isFull();

	void display();
	Type mean();
	int getFront();
	int getLast();
	int getLength();
	int getSize();
	bool vectorize(vector<Type>*);
	
	ostream& operator << (ostream& outs);
	istream& operator >> (istream& ins);
	
};
#endif

template<class T>
Queue<T>::Queue(void){
	length = 0;
	queue = new T[SIZE];
	front = -1;
	last = -1;
	size = SIZE;
}

template<class T>
Queue<T>::Queue(int newSize){
	size = newSize;
	length = 0;
	queue = new T[size];
	front = -1;
	last = -1;

	for (int i = 0; i < size; i++)
		queue[i] = 0;
}

template<class T>
Queue<T>::~Queue(void){
}

template<class T>
bool Queue<T>::enqueue(T data){

	if (isEmpty()){
		front = 0;
		last = 0;
		queue[last] = data;
		length++;
		return true;
	}
	else if (!isFull()){
		last = (last + 1) % size;
		queue[last] = data;
		length++;
		return true;
	
	}
	else
		return false;
}

template<class T>
T Queue<T>::dequeue(){
	T tmp;

	if (!isEmpty() && front != last)//more than one element
	{
		tmp = queue[front];
		queue[front] = 0;
		front = (front + 1) % size;
		length--;
		return tmp;
	}
	else if (!isEmpty() && front == last) //only one element
	{
		tmp = queue[front];
		queue[front] = 0;
		front = last = -1;
		length--;
		return tmp;
	}
	else
		return NULL;
}

template<class T>
T Queue<T>::getEntry(int index){
	if (index < length)
		return queue[(front + index) % size];
	else
		return NULL;
}

template<class T>
bool Queue<T>::isEmpty(){
	return (length == 0);
}

template<class T>
bool Queue<T>::isFull(){
	return (length == size);
}

template<class T>
void Queue<T>::display(){
	if (!isEmpty()) {
		if (front<last) {
			cout << endl;

			for (int i = front; i <= last; i++)
				cout << queue[i] << " ";

			cout << endl;
		}
		else if (last < front) {
			cout << endl;

			for (int i = front; i < length; i++)
				cout << queue[i] << " ";
			for (int i = 0; i <= last; i++)
				cout << queue[i] << " ";

			cout << endl;
		}
		else //1 element -> front = last
		{
			cout << endl;
			cout << queue[front];
			cout << endl;
		}
	}
	else
		cout << "Empty QUEUE!" << endl;
}

template<class T>
T Queue<T>::mean(){
	T mean = 0;
	// Compute mean if it's not empty
	if (!isEmpty()) {
		if (front<last) {			
			for (int i = front; i <= last; i++)
				mean += queue[i];
			return mean / length;
		}
		else if (last < front) {
			for (int i = front; i < size; i++)
				mean += queue[i];
			for (int i = 0; i <= last; i++)
				mean += queue[i];
			return mean / length;
		}
		else //1 single elemen
			return queue[front];
	}
	else {
		cout << "QUEUE is EMPTY." << endl << "Cannot compute MEAN" << endl;
		return NULL;
	}
}

template<class T>
int Queue<T>::getFront() {
	return front;
}

template<class T>
int Queue<T>::getLast() {
	return last;
}

template<class T>
int Queue<T>::getLength() {
	return length;
}

template<class T>
int Queue<T>::getSize() {
	return size;
}

template<class T>
bool Queue<T>::vectorize(vector<T>* v){
	//try to clear the vector before start pushing
	v->clear();
	// Compute mean only if full
	if (!isEmpty()) {
		if (front<last) {
			for (int i = front; i <= last; i++)
				v->push_back(queue[i]);
			return true;
		}
		else {
			for (int i = front; i < size; i++)
				v->push_back(queue[i]);
			for (int i = 0; i <= last; i++)
				v->push_back(queue[i]);
			return true;
		}
		
	}
	else {
		cout << "QUEUE is EMPTY." << endl << "Cannot VECTORIZE" << endl;
		return false;
	}


}

template<class T>
ostream& Queue<T>::operator <<(ostream& outs){

if (isFull()) {
	if (front<last) {
		for (int i = front; i <= last; i++)
			outs << queue[i];
		return outs;
	}
	else if (last < front) {
		for (int i = front; i < length; i++)
			outs << queue[i];
		for (int i = 0; i <= last; i++)
			outs << queue[i];
		return outs;
	}
}
else {
	return outs;
	}
}

template<class T>
istream& Queue<T>::operator >>(istream& ins){

		if (front<last) {
			for (int i = front; i <= last; i++)
				ins >> queue[i];
			return outs;
		}
		else if (last < front) {
			for (int i = front; i < length; i++)
				ins >> queue[i];
			for (int i = 0; i <= last; i++)
				ins >> queue[i];
			return ins;
		}	
}