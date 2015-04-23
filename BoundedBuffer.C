#include "BoundedBuffer.H"
using namespace std;

Item::Item(char p, string m){
	person = p;
	message = m;
}

BoundedBuffer::BoundedBuffer(int size, Semaphore* sem){
	s = sem;
	maxSize = size;
	numFinished = 0;
}

BoundedBuffer::~BoundedBuffer(){
}

void BoundedBuffer::add(Item item){
	s->P();
	if(buffer.size() < maxSize){
		buffer.push_back(item);
		s->V();
	}
	else{
		s->V();
		while (true){
			s->P();
			if(buffer.size() < maxSize){
				buffer.push_back(item);
				s->V();
				break;
			}
			s->V();
			usleep(10000);
		}
	}
}

Item BoundedBuffer::remove(){
	s->P();
	if(buffer.size() == 0){
		s->V();
		return Item('n',"NULL");
	}
	Item item(buffer[0].getPerson(),buffer[0].getMessage());
	item.setData(buffer[0].getData());
	buffer.erase(buffer.begin());
	s->V();
	return item;
}