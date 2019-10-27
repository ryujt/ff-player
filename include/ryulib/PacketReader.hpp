#ifndef RYULIB_PACKETREADER_HPP
#define RYULIB_PACKETREADER_HPP

class PacketReader
{
public:
	PacketReader() 
	{
		buffer_ = (char*) malloc(PACKETREADER_BUFFER_SIZE);
		data_ = (char*) malloc(PACKETREADER_BUFFER_SIZE);
	}

	~PacketReader() 
	{
		if (buffer_ != nullptr) delete buffer_;
		if (data_ != nullptr) delete data_;
	}

	void clear()
	{
		size_ = 0;
	}

	void write(const void *data, const int size)
	{
		char* tail = buffer_ + size_;
		size_ = size_ + size;
		memcpy(tail, data, size);
	}

	void* read(const int size)
	{
		if (canRead(size) == false) return nullptr;

		memcpy(data_, buffer_, size);

		size_ = size_ - size;
		if (size_ > 0) {
			char* tail = buffer_ + size;
			memcpy(buffer_, tail, size_);
		}

		return data_;
	}

	bool canRead(const int size)
	{
		return size_ >= size;
	}

private:
	static const int PACKETREADER_BUFFER_SIZE = 1024 * 1024;
	char* buffer_ = nullptr;
	char* data_ = nullptr;
	int size_ = 0;
};

#endif  // RYULIB_PACKETREADER_HPP