#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) { buffer.resize(capacity); }

pair<size_t, size_t> ByteStream::twoParts(size_t len) const {
    if (tail > head) {
        return {min(len, tail - head), 0};
    } else {
        size_t firstPart = min(len, buffer.size() - head);
        if (firstPart < len) {
            return {firstPart, min(len - firstPart, tail)};
        } else {
            return {firstPart, 0};
        }
    }
}

size_t ByteStream::copy_to_buffer(const string &data) {
    auto [firstPart, secondPart] = twoParts(data.size());
    copy(data.begin(), data.begin() + firstPart, buffer.begin() + head);
    if (firstPart < data.size()) {
        copy(data.begin() + firstPart, data.begin() + firstPart + secondPart, buffer.begin());
        return firstPart + secondPart;
    }
    return firstPart;
}

string ByteStream::copy_from_buffer(size_t len) const {
    auto [firstPart, secondPart] = twoParts(len);
    string ret(firstPart + secondPart, '\0');
    copy(buffer.begin() + head, buffer.begin() + head + firstPart, ret.begin());
    if (firstPart < len) {
        copy(buffer.begin(), buffer.begin() + secondPart, ret.begin() + firstPart);
    }
    return ret;
}

size_t ByteStream::write(const string &data) {
    size_t written_bytes = copy_to_buffer(data);
    tail = (tail + written_bytes) % buffer.size();
    bytes_written_ += written_bytes;
    return written_bytes;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    return copy_from_buffer(len);
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) {
    auto pop_len = min(len, buffer_size());
    head = (head + pop_len) % buffer.size();
    bytes_read_ += pop_len;
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

size_t ByteStream::buffer_size() const { return (tail + buffer.size() - head) % buffer.size(); }

bool ByteStream::buffer_empty() const { return head == tail; }

bool ByteStream::eof() const { return buffer_empty() && input_ended(); }

size_t ByteStream::bytes_written() const { return bytes_written_; }

size_t ByteStream::bytes_read() const { return bytes_read_; }

size_t ByteStream::remaining_capacity() const { return buffer.size() - buffer_size(); }
