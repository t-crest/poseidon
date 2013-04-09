#include "file.hpp"

using namespace std;

namespace snts {

file::file(const string& file_path, ios_base::openmode mode) 
:	file_path(file_path), file_stream(file_path.c_str(), mode)
{
}

file::~file() {
	if (this->file_stream.is_open())
		this->file_stream.close();
}

string file::getline() {
	string ret;
	this->check_for_input();
	std::getline(this->file_stream, ret);
	return ret;
}

void file::check_for_input() const {
	if (!this->file_stream.good()) {
		throw file_read_error("Could not read file " + this->file_path);
	}
}

////////////////////////////////////////////////////////////////////////////////

file_read_error::file_read_error(const string& msg)
:	runtime_error(msg)
{
}

}
