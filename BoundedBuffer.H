#include "semaphore.H"
#include <vector>
#include <string>
#include <unistd.h>
using namespace std;

class Item
{
public:
	Item(char p, string m);
	string getData() { return data; }
	void setData(string x) { data = x; }
	char getPerson() { return person; }
	void setPerson(char c) { person = c; }
	string getMessage() { return message; }
	void setMessage(string m) { message = m; }
private:
	string data;
	char person; //identifier for the person who the item belongs to
	string message;
};

class BoundedBuffer
{
public:
	BoundedBuffer(int size, Semaphore* sem);
	~BoundedBuffer();
	void add(Item item);
	int getMaxSize() { return maxSize; }
	void setMaxSize(int x) { maxSize = x; }
	Item remove();
	Semaphore* s;
	void finished(){s->P();
					numFinished++;
					s->V();}
	int numFinished;
	int getFinished(){s->P();
					int result = numFinished;
					s->V();
					return result;}
	int getSize() {return buffer.size(); }
private:
	vector<Item> buffer;
	int maxSize; 
};