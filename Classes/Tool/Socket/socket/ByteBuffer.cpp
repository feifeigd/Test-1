#include "ByteBuffer.h"


ByteBuffer::ByteBuffer(int capacity)
{
	buffer = new char[capacity];
	position = 0;
	this->capacity = capacity;
	this->limit = capacity;
}

ByteBuffer::ByteBuffer(char* data,int offset,int size)
{
	buffer = new char[size];
	position = 0;
	memcpy(buffer,data+offset,size);
	this->capacity = size;
	this->limit = this->capacity;
}

ByteBuffer::~ByteBuffer()
{
	SAFE_DELETE_ARRAY( buffer);
}

void ByteBuffer::reset()
{
	position = 0;
	this->limit = capacity;
}

int ByteBuffer::remaining()
{
	return limit - position;
}

void ByteBuffer::put(const char* bytes,int offset,int len)
{
	if(position + len > capacity)
	{
		return;
	}
	memcpy(buffer+position,bytes+offset,len);
	position += len;
}
void ByteBuffer::putByte(unsigned char n){
	if(position + 1 > capacity){
		//printf("error ByteBuffer::putByte----position=%d,len=%d,capacity=%d\n",position,1,capacity);
		return;
	}
	buffer[position] = n;
	position++;
}

void ByteBuffer::put(int n){
	if(position + 1 > capacity){
		//printf("error ByteBuffer::put---position=%d,len=%d,capacity=%d\n",position,1,capacity);
		return;
	}
	buffer[position] = (char)n;
	position++;
}

void ByteBuffer::putInt(int n){
	if(position + 4 > capacity){
		//printf("error ByteBuffer::putInt--position=%d,len=%d,capacity=%d\n",position,4,capacity);
		return;
	}
	for(int i = 0 ; i < 4 ; i++){
		buffer[position] = (char)((n >> (8 *(3 - i))) & 0xFF);
		position++;
	}
}
void ByteBuffer::setIntAt(int n,int index){
	if(index + 4 > capacity){
		//printf("error ByteBuffer::setIntAt--index=%d,len=%d,capacity=%d\n",index,4,capacity);
		return;
	}
	for(int i = 0 ; i < 4 ; i++){
		buffer[index] = (char)((n >> (8 *(3 - i))) & 0xFF);
		index++;
	}
}

void ByteBuffer::putFloat(float n){
	if(position + 4 > capacity){
		//printf("error -ByteBuffer::putFloat---position=%d,len=%d,capacity=%d\n",position,4,capacity);
		return;
	}
	unsigned char* pBytes;
	pBytes = (unsigned char*)&n;

	buffer[position++] = pBytes[3];
	buffer[position++] = pBytes[2];
	buffer[position++] = pBytes[1];
	buffer[position++] = pBytes[0];
	
	
	//return floatValue;
}

void ByteBuffer::putShort(short n){
	if(position + 2 > capacity){
		//printf("error -ByteBuffer::putShort---position=%d,len=%d,capacity=%d\n",position,2,capacity);
		return;
		
	}
	for(int i = 0 ; i < 2 ; i++){
		buffer[position] = (char)((n >> (8 *(1 - i))) & 0xFF);
		position++;
	}
}

void ByteBuffer::putLong(long long n){
	if(position + 8 > capacity){
		//printf("error ByteBuffer::putLong-position=%d,len=%d,capacity=%d\n",position,8,capacity);
		return;
	}
	
	for(int i = 0 ; i < 8 ; i++){
		buffer[position] = (char)((n >> (8 *(7 - i))) & 0xFF);
		position++;
	}
}
void ByteBuffer::putUTF(const char* str){
	short len = strlen(str);
	if(position + 2+len > capacity){
		//printf("error ByteBuffer::putUTF----position=%d,len=%d,capacity=%d\n",position,2+len,capacity);
		return;
	}
	putShort(len);
	if( len>0){
		put(str,0,len);
	}
}
void ByteBuffer::putUTF(const std::string& str){
	putUTF(str.c_str());
	
}

void ByteBuffer::putArray(std::vector<unsigned char>& a){
	putInt(a.size());
	for(size_t i=0;i< a.size();++i){
		put(a[i]);
	}	
}
void ByteBuffer::putArray(std::vector<bool>& a){
	putInt(a.size());
	for(size_t i=0;i< a.size();++i){
		put(a[i]);
	}	
}
void ByteBuffer::putArray(std::vector<short>& a){
	putInt(a.size());
	//short* d = a.data();
	for(size_t i=0;i< a.size();++i){
		putShort(a[i]);
	}
}
void ByteBuffer::putArray(std::vector<int>& a){
	putInt(a.size());
	//int* d = a.data();
	for(size_t i=0;i< a.size();++i){
		putInt(a[i]);
	}
}
void ByteBuffer::putArray(std::vector<long long>& a){
	putInt(a.size());

	for(size_t i=0;i< a.size();++i){
		putLong(a[i]);
	}
}
void ByteBuffer::putArray(std::vector<std::string>& a){
	putInt(a.size());
	
	for(size_t i=0;i< a.size();++i){
		putUTF(a[i]);
	}
}

void ByteBuffer::getArray(std::vector<unsigned char>& a){
	int size = getInt();
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = get();
	}
	
}
void ByteBuffer::getArray(std::vector<bool>& a){
	int size = getInt();
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getBoolean();
	}
	
}

void ByteBuffer::getArray(std::vector<short>& a){
	
	int size = getInt();
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getShort();
	}
}
void ByteBuffer::getArray(std::vector<int>& a){
	int size = getInt();
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getInt();
	}
}
void ByteBuffer::getArray(std::vector<long long>& a){
	
	int size = getInt();
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getLong();
	}
	
}
void ByteBuffer::getArray(std::vector<std::string>& a){
	int size =getInt();
	getArray(a,size);
}
void ByteBuffer::getArray(std::vector<unsigned char>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getByte();
	}
}
void ByteBuffer::getArray(std::vector<bool>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getBoolean();
	}	
}
void ByteBuffer::getArray(std::vector<short>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getShort();
	}
}
void ByteBuffer::getArray(std::vector<int>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getInt();
	}
}
void ByteBuffer::getArray(std::vector<long long>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		a[i] = getLong();
	}
}
void ByteBuffer::getArray(std::vector<std::string>& a, int size){
	a.resize(size);
	for(int i=0;i< size;++i){
		getUTF(a[i]);
	}
}

void ByteBuffer::getUTF(std::string& str){
	short len = getShort();		
	if( len>0){
		str.append(buffer+position,len);
		position+=len;
	}else {
		str="";
	}

}
std::string ByteBuffer::getUTF(){
	std::string str;
	getUTF(str);		
	return str;
}

int ByteBuffer::getPosition(){
	return position;
}

void ByteBuffer::setPosition(int p){
	if(p > limit) {
		//printf("error ByteBuffer::setPosition p> limit------------p=%d,limit=%d\n",p,limit);
	}
	position = p;
}

int ByteBuffer::getLimit(){
	return limit;
}

int ByteBuffer::getCapacity(){
	return this->capacity;
}

char* ByteBuffer::getBuffer(){
	return buffer;
}
char* ByteBuffer::toByteArray(){
	char* tmp = new char[position];
	memcpy( tmp,buffer,position);
	return tmp;
}

char ByteBuffer::get(){
	if(position < limit){
		return buffer[position++];
	}
	//printf("error ByteBuffer::get() position+1> limit------------position=%d,limit=%d\n",position,limit);
	return 0;	
}
unsigned char ByteBuffer::getByte(){
	if(position + 1 > limit){
		//printf("error ByteBuffer::getByte() position+1> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
	}
	return buffer[position++];
}
bool ByteBuffer::getBoolean(){
	if(position + 1 > limit){
		//printf("error ByteBuffer::getBoolean() position+1> limit------------position=%d,limit=%d\n",position,limit);
		return false;
	}
	return buffer[position++]!=0;
}

int ByteBuffer::getLength(int offset)
{
    int lengthPos = position+ offset;
	unsigned char* pos = new unsigned char[4]; //memory leak
    for(int i = 0;i < 4 ;i++)
    {
        pos[i] = buffer[lengthPos+i];
    }
    
    int length =  bytesToInt(pos);
	delete[] pos; //bugfix by rain:修正内存泄漏
	return length;
}

void ByteBuffer::getAsBytes(unsigned char* bytes)
{
    for(int i = 0 ; i < 4 ; i++){
		bytes[i]=  buffer[position];
		position++;
	}

}

int ByteBuffer::getInt(){
	if(position + 4 > limit){
		//printf("error ByteBuffer::getInt() position+4> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
	}
	int rt = 0;
	for(int i = 0 ; i < 4 ; i++){
		rt |=  ((buffer[position] & 0xFF) << (8 *(3 - i)));
		position++;
	}
	return rt;
}

float ByteBuffer::getFloat(){
	if(position + 4 > limit){
		//printf("error ByteBuffer::getFloat() position+4> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
	}
	float floatValue;
	unsigned char* pBytes;
	pBytes = (unsigned char*)&floatValue;
	pBytes[3] = buffer[position++];
	pBytes[2] = buffer[position++];
	pBytes[1] = buffer[position++];
	pBytes[0] = buffer[position++];
	return floatValue;
}

short ByteBuffer::getShort(){
	if(position + 2 > limit){
		//printf("error ByteBuffer::getShort() position+2> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
	}
	short n = 0;
	for(int i = 0 ; i < 2 ; i++){
		n |= ((buffer[position] & 0xFF) << (8 *(1 - i)));
		position++;
	}
	return n;
}

long long ByteBuffer::getLong(){
	if( position + 8 > limit ){
		
		
		//printf("error ByteBuffer::getLong() position+8> limit------------position=%d,limit=%d\n",position,limit);
		return 0;
		
	}
	long long n = 0;
	for(int i = 0 ; i < 8 ; i++){
		long long tmp = (buffer[position] & 0xFF);
		n  |= (tmp << (8 *(7 - i)));
		
		position++;
	}
	return n;
}
void ByteBuffer::get(char* bytes,int size){
	get(bytes,0,size);
}

void ByteBuffer::get(char* bytes,int offset,int len){
	if(position + len > limit){
		memset(bytes+offset, 0, len );
		//printf("error ByteBuffer::get(char* bytes,int offset,int len) position+len> limit------------position=%d,len=%d,limit=%d\n",position,len,limit);
		return;
	}
	memcpy(bytes+offset,buffer+position,len);
	position += len;
}
/**
 * makes a buffer ready for a new sequence of channel-read or relative put operations: It sets the limit to the capacity and the position to zero.
 */
void ByteBuffer::clear(){
	position = 0;
	this->limit = capacity;
}

/**
 * flip() makes a buffer ready for a new sequence of channel-write or relative get operations: It sets the limit to the current position and then sets the position to zero.
 */
void ByteBuffer::flip(){
	this->limit = position;
	position = 0;
}

void ByteBuffer::compact(){
	if(position > 0){
		for(int i = position; i < limit ; i++){
			buffer[i-position] = buffer[i];
		}
	}
	position = limit - position;
	limit = this->capacity;
}

void ByteBuffer::rewind(){
	position = 0;
}

void ByteBuffer::putBoolean(bool b)
{
	if( position+1> capacity ){
		//printf("error putBoolean position+len> limit------------position=%d,len=%d,capacity=%d\n",position,1,capacity);

		return;
	}
	
	if(b)
		buffer[position] =(1&0xff);
	else 
		buffer[position]=(0&0xff);
	position+=1;
	
}

//这个方法只能用于打开的是文本文件的时候
std::string ByteBuffer::getLine(){
    if( position>=capacity) return "";
	std::string line;
	for (int i = position; i<capacity; i++) {
		if (buffer[i] == '\n') {
			line.append(buffer + position, i - position);
			position = i + 1;
			return line;
		}
	}
	line.append(buffer + position, capacity - position);
	position = capacity +1;
	return line;
}

int ByteBuffer::bytesToInt(unsigned char* bytes)
{
	int addr = bytes[3] & 0xFF;
	addr |= ((bytes[2] << 8) & 0xFF00);
	addr |= ((bytes[1] << 16) & 0xFF0000);
	addr |= ((bytes[0] << 24) & 0xFF000000);
	return addr;
}

unsigned char* ByteBuffer::intToByte(int i)
{
	unsigned char* abyte0 = new unsigned char[4];
	abyte0[3] = (unsigned char)(0xff & i);
	abyte0[2] = (unsigned char)((0xff00 & i) >> 8);
	abyte0[1] = (unsigned char)((0xff0000 & i) >> 16);
	abyte0[0] = (unsigned char)((0xff000000 & i) >> 24);
	return abyte0;
}