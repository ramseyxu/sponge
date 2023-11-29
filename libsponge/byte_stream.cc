#include "byte_stream.hh"
#include "cassert"
#include "cstdio"
#include <cstdarg>

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

#define DEBUG__

void debug_print(const char* format, ...) {
#ifdef DEBUG__
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
#endif
}

ByteStream::ByteStream(const size_t capacity) :
buffer(vector<char>(capacity)){ debug_print("construct cap %lu\n", capacity); }

size_t ByteStream::copy_to_buffer(const string &data) {
    if (remaining_capacity() == 0)
        return 0;
    if (tail < head) {
        size_t copy_len = min(data.size(), head - tail);
        copy(data.begin(), data.begin() + copy_len, buffer.begin() + tail);
        return copy_len;
    }
    size_t firstPart = min(data.size(), buffer.size() - tail);
    copy(data.begin(), data.begin() + firstPart, buffer.begin() + tail);
    if (firstPart < data.size()) {
        size_t secondPart = min(head, data.size() - firstPart);
        copy(data.begin() + firstPart, data.begin() + firstPart + secondPart, buffer.begin());
        return firstPart + secondPart;
    }
    return firstPart;
}

string ByteStream::copy_from_buffer(size_t len) const {
    if (buffer_empty())
        return "";
    if (head < tail) {
        size_t copy_len = min(len, tail - head);
        return string(buffer.begin() + head, buffer.begin() + head + copy_len);
    }
    size_t firstPart = min(len, buffer.size() - head);
    size_t secondPart = firstPart == len ? 0 : min(tail, len - firstPart);
    string ret(firstPart + secondPart, '\0');
    copy(buffer.begin() + head, buffer.begin() + head + firstPart, ret.begin());
    if (firstPart < len) {
        copy(buffer.begin(), buffer.begin() + secondPart, ret.begin() + firstPart);
    }
    debug_print("copy from buffer len %lu head %lu tail %lu buffer %s ret %s\n",
    len, head, tail, buffer.data(), ret.c_str());
    return ret;
}

size_t ByteStream::write(const string &data) {
    size_t written_bytes = copy_to_buffer(data);
    tail = (tail + written_bytes) % buffer.size();
    bytes_written_ += written_bytes;
    buffer_size_ += written_bytes;
    assert(buffer_size_ <= buffer.size());
    debug_print("after write %s\n, head %lu tail %lu buffer %s, written %lu\n"
    , data.c_str(), head, tail, buffer.data(), written_bytes);
    return written_bytes;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return copy_from_buffer(len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    auto pop_len = min(len, buffer_size_);
    head = (head + pop_len) % buffer.size();
    bytes_read_ += pop_len;

    printf("after pop head %lu tail %lu\n", head, tail);

    assert(buffer_size_ >= pop_len);
    buffer_size_ -= pop_len;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    auto ret = peek_output(len);
    pop_output(ret.size());
    return ret;
}

void ByteStream::end_input() { input_ended_ = true; }

bool ByteStream::input_ended() const { return input_ended_; }

size_t ByteStream::buffer_size() const { return buffer_size_; }

bool ByteStream::buffer_empty() const { return buffer_size_ == 0; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return bytes_written_; }

size_t ByteStream::bytes_read() const { return bytes_read_; }

size_t ByteStream::remaining_capacity() const { return buffer.size() - buffer_size(); }
